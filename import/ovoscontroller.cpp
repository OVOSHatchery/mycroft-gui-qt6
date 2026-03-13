/*
 * Copyright 2018 by Marco Martin <mart@kde.org>
 * Copyright 2018 David Edmundson <davidedmundson@kde.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "ovoscontroller.h"
#include "guibusmessages.h"
#include "globalsettings.h"
#include "abstractdelegate.h"
#include "activeskillsmodel.h"
#include "abstractskillview.h"
#include "controllerconfig.h"

#include <QtGlobal>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>
#include <QProcess>
#include <QQmlPropertyMap>
#include <QStandardItemModel>
#include <QQmlEngine>
#include <QQmlContext>
#include <QUuid>
#include <QWebSocket>

OVOSController *OVOSController::instance()
{
    static OVOSController* s_self = nullptr;
    if (!s_self) {
        s_self = new OVOSController;
    }
    return s_self;
}


OVOSController::OVOSController(QObject *parent)
    : QObject(parent),
      m_appSettingObj(new GlobalSettings)
{

    m_useTls = qgetenv("MYCROFT_GUI_TLS").toInt() == 1 ||
               qgetenv("MYCROFT_GUI_TLS").toLower() == "true";
    m_authToken = QString::fromUtf8(qgetenv("MYCROFT_GUI_TOKEN"));

    // Initialize session_id: CLI args > env vars > config > "default"
    m_sessionId = QStringLiteral("default");

    // Initialize site_id: CLI args > env vars > config > auto-generated UUID
    QString envSiteId = QString::fromUtf8(qgetenv("MYCROFT_SITE_ID"));
    if (!envSiteId.isEmpty()) {
        m_siteId = envSiteId;
    } else {
        m_siteId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }

    connect(&m_mainWebSocket, &QWebSocket::connected, this,
            [this] () {
                m_reconnectTimer.stop();
                m_reconnectTimer.setInterval(1000);  // Reset backoff on successful connect
                emit socketStatusChanged();
            });
    connect(&m_mainWebSocket, &QWebSocket::disconnected, this, &OVOSController::closed);
    connect(&m_mainWebSocket, &QWebSocket::stateChanged, this,
            [this] (QAbstractSocket::SocketState state) {
                emit socketStatusChanged();
                if (state == QAbstractSocket::ConnectedState) {
                    qWarning() << "Main Socket connected, trying to connect gui";

                    sendRequest(QStringLiteral("mycroft.skills.all_loaded"), QVariantMap());
                } else {
                    if (m_serverReady) {
                        m_serverReady = false;
                        emit serverReadyChanged();
                    }
                }
            });

    connect(&m_mainWebSocket, &QWebSocket::textMessageReceived, this, &OVOSController::onMainSocketMessageReceived);

    // Reconnect timer with exponential backoff (1s, 2s, 4s, 8s, ... up to 30s)
    m_reconnectTimer.setInterval(1000);
    connect(&m_reconnectTimer, &QTimer::timeout, this, [this]() {
        QString host = QString::fromUtf8(qgetenv("MYCROFT_GUI_HOST")).isEmpty()
            ? m_appSettingObj->webSocketAddress()
            : QString::fromUtf8(qgetenv("MYCROFT_GUI_HOST"));
        int port = qgetenv("MYCROFT_GUI_PORT").toInt();
        if (port == 0) {
            port = 18181;
        }
        QString scheme = m_useTls ? QStringLiteral("wss") : QStringLiteral("ws");
        QString path = m_authToken.isEmpty() ? QStringLiteral("/gui") : QStringLiteral("/gui?token=") + m_authToken;
        QString socket = QStringLiteral("%1://%2:%3%4").arg(scheme).arg(host).arg(port).arg(path);
        m_mainWebSocket.open(QUrl(socket));
        // Exponential backoff: double interval up to 30s max
        int nextInterval = qMin(m_reconnectTimer.interval() * 2, 30000);
        m_reconnectTimer.setInterval(nextInterval);
    });
}


void OVOSController::start()
{
    QString host = QString::fromUtf8(qgetenv("MYCROFT_GUI_HOST")).isEmpty()
        ? m_appSettingObj->webSocketAddress()
        : QString::fromUtf8(qgetenv("MYCROFT_GUI_HOST"));
    int port = qgetenv("MYCROFT_GUI_PORT").toInt();
    if (port == 0) {
        port = 18181;
    }
    QString scheme = m_useTls ? QStringLiteral("wss") : QStringLiteral("ws");
    QString path = m_authToken.isEmpty() ? QStringLiteral("/gui") : QStringLiteral("/gui?token=") + m_authToken;
    QString socket = QStringLiteral("%1://%2:%3%4").arg(scheme).arg(host).arg(port).arg(path);
    m_mainWebSocket.open(QUrl(socket));
    connect(&m_mainWebSocket, &QWebSocket::errorOccurred,
            this, [this] (const QAbstractSocket::SocketError &error) {
        if (error != QAbstractSocket::HostNotFoundError && error != QAbstractSocket::ConnectionRefusedError) {
            qWarning() << "OVOS is running but the connection failed for some reason.";
            return;
        }

        m_reconnectTimer.start();
        emit socketStatusChanged();
    });
    emit socketStatusChanged();
}

void OVOSController::disconnectSocket()
{
    qDebug() << "in reconnect";
    m_mainWebSocket.close();
    m_reconnectTimer.stop();
    emit socketStatusChanged();
}

void OVOSController::reconnect()
{
    qDebug() << "in reconnect";
    m_mainWebSocket.close();
    m_reconnectTimer.start();
    emit socketStatusChanged();
}

void OVOSController::onMainSocketMessageReceived(const QString &message)
{
    auto doc = QJsonDocument::fromJson(message.toUtf8());

    if (doc.isEmpty()) {
        qWarning() << "Empty or invalid JSON message arrived on the main socket:" << message;
        return;
    }

    auto typeStr = doc[QStringLiteral("type")].toString();

    if (typeStr.isEmpty()) {
        qWarning() << "Empty type in the JSON message on the main socket";
        return;
    }

#ifdef DEBUG_OVOS_MESSAGEBUS
    qDebug() << "type" << typeStr;
#endif

    auto parsed = GuiBusMessages::parseMessage(typeStr);

    switch (parsed.layer) {

    // ========================================
    // WIRE PROTOCOL MESSAGES
    // ========================================
    case GuiBusMessages::MessageLayer::WIRE: {
        auto wire = parsed.wire;

        // Forwarded assistant event wrapper
        // Format: {"type": "mycroft.gui.forward.assistant", "event_type": "recognizer_loop:wakeword", "data": {...}}
        if (wire == GuiBusMessages::WireMessage::FORWARD_ASSISTANT) {
            const QString eventType = doc[QStringLiteral("event_type")].toString();
            if (!eventType.isEmpty()) {
                auto evt = GuiBusMessages::assistantFromString(eventType);
                handleAssistantEvent(evt, doc);
            }
            return;
        }

        // Forwarded shell event wrapper
        // Format: {"type": "mycroft.gui.forward.shell", "event_type": "gui.notification.set", "data": {...}}
        if (wire == GuiBusMessages::WireMessage::FORWARD_SHELL) {
            for (auto view : m_views) {
                view->handleIncomingMessage(message);
            }
            return;
        }

        // Namespace lifecycle — clear all skill data
        if (wire == GuiBusMessages::WireMessage::CLEAR_NAMESPACE) {
            const QString namespace_ = doc[QStringLiteral("data")][QStringLiteral("namespace")].toString();
            if (!namespace_.isEmpty()) {
                for (auto view : m_views) {
                    view->handleIncomingMessage(message);
                }
            }
            return;
        }

        // All other wire messages (session data, GUI pages, events) → forward to views
        for (auto view : m_views) {
            view->handleIncomingMessage(message);
        }
        return;
    }

    // Shell events sent directly by this client (bidirectional request/response)
    case GuiBusMessages::MessageLayer::SHELL:
        for (auto view : m_views) {
            view->handleIncomingMessage(message);
        }
        return;

    // Assistant events should only arrive via FORWARD_ASSISTANT wrapper
    case GuiBusMessages::MessageLayer::ASSISTANT:
        qWarning() << "Received bare assistant event (expected FORWARD_ASSISTANT wrapper):" << typeStr;
        return;

    case GuiBusMessages::MessageLayer::UNKNOWN:
        if (typeStr.contains(QLatin1Char(':')) && !doc[QStringLiteral("data")][QStringLiteral("utterance")].toString().isEmpty()) {
            const QString skill = typeStr.split(QLatin1Char(':')).first();
            if (skill.contains(QLatin1Char('.'))) {
                qDebug() << "Current skill:" << skill;
                emit utteranceManagedBySkill(skill);
            }
        }
        return;
    }
}

void OVOSController::handleAssistantEvent(GuiBusMessages::AssistantEvent evt, const QJsonDocument &doc)
{
    if (evt == GuiBusMessages::AssistantEvent::INTENT_FAILURE) {
        m_isListening = false;
        emit isListeningChanged();
        emit notUnderstood();
    }

    if (evt == GuiBusMessages::AssistantEvent::RECOGNIZER_AUDIO_OUTPUT_START) {
        m_isSpeaking = true;
        emit isSpeakingChanged();
        return;
    }

    if (evt == GuiBusMessages::AssistantEvent::RECOGNIZER_AUDIO_OUTPUT_END) {
        m_isSpeaking = false;
        emit isSpeakingChanged();
        return;
    }

    if (evt == GuiBusMessages::AssistantEvent::RECOGNIZER_WAKEWORD) {
        m_isListening = true;
        emit isListeningChanged();
        return;
    }

    if (evt == GuiBusMessages::AssistantEvent::RECOGNIZER_RECORD_BEGIN && !m_isListening) {
        m_isListening = true;
        emit isListeningChanged();
        return;
    }

    if (evt == GuiBusMessages::AssistantEvent::RECOGNIZER_RECORD_END) {
        m_isListening = false;
        emit isListeningChanged();
        return;
    }

    if (evt == GuiBusMessages::AssistantEvent::SPEECH_RECOGNITION_UNKNOWN) {
        emit notUnderstood();
        return;
    }

    if (evt == GuiBusMessages::AssistantEvent::STOP_HANDLED) {
        emit stopped();
        return;
    }

    if (evt == GuiBusMessages::AssistantEvent::SKILLS_LOADED_RESPONSE) {
        if (doc[QStringLiteral("data")][QStringLiteral("status")].toBool() == true) {
            m_serverReady = true;
            emit serverReadyChanged();
        }
    } else if (evt == GuiBusMessages::AssistantEvent::READY) {
        m_serverReady = true;
        emit serverReadyChanged();
    }

    if (evt == GuiBusMessages::AssistantEvent::SCREEN_CLOSE_IDLE_EVENT) {
        QString skill_idle_event_id = doc[QStringLiteral("data")][QStringLiteral("skill_idle_event_id")].toString();
        emit skillTimeoutReceived(skill_idle_event_id);
    }
}

void OVOSController::sendRequest(const QString &type, const QVariantMap &data, const QVariantMap &context)
{
    if (m_mainWebSocket.state() != QAbstractSocket::ConnectedState) {
        qWarning() << "OVOS connection not open!";
        return;
    }

    QJsonObject root;
    root[QStringLiteral("type")] = type;
    root[QStringLiteral("data")] = QJsonObject::fromVariantMap(data);

    // Ensure context has {"session": {"session_id": "default"}}
    QJsonObject contextJson = QJsonObject::fromVariantMap(context);
    if (!contextJson.contains(QStringLiteral("session"))) {
        QJsonObject session;
        session[QStringLiteral("session_id")] = QStringLiteral("default");
        contextJson[QStringLiteral("session")] = session;
    }

    root[QStringLiteral("context")] = contextJson;

    QJsonDocument doc(root);
    m_mainWebSocket.sendTextMessage(QString::fromUtf8(doc.toJson()));
}

void OVOSController::sendBinary(const QString &type, const QJsonObject &data, const QVariantMap &context)
{
    if (m_mainWebSocket.state() != QAbstractSocket::ConnectedState) {
        qWarning() << "OVOS connection not open!";
        return;
    }
    QJsonObject socketObject;
    socketObject[QStringLiteral("type")] = type;
    socketObject[QStringLiteral("data")] = data;
    socketObject[QStringLiteral("context")] = QJsonObject::fromVariantMap(context);

    QJsonDocument doc;
    doc.setObject(socketObject);
    QByteArray docbin = doc.toJson(QJsonDocument::Compact);
    m_mainWebSocket.sendBinaryMessage(docbin);
}

void OVOSController::sendText(const QString &message)
{
    sendRequest(QStringLiteral("recognizer_loop:utterance"), QVariantMap({{QStringLiteral("utterances"), QStringList({message})}}), QVariantMap({{QStringLiteral("source"), QStringLiteral("ovos-gui")}, {QStringLiteral("destination"), QStringLiteral("skills")}}));
}

void OVOSController::registerView(AbstractSkillView *view)
{
    Q_ASSERT(!view->id().isEmpty());
    Q_ASSERT(!m_views.contains(view->id()));
    m_views[view->id()] = view;
    // Connect view destruction to deregisterView
    connect(view, &QObject::destroyed, this, [this, view]() {
        deregisterView(view);
    });
    if (m_mainWebSocket.state() == QAbstractSocket::ConnectedState) {
        sendRequest(QStringLiteral("mycroft.gui.connected"),
                    QVariantMap({{QStringLiteral("gui_id"), view->id()},
                                 {QStringLiteral("session_id"), m_sessionId},
                                 {QStringLiteral("site_id"), m_siteId}}));
    }
}

void OVOSController::deregisterView(AbstractSkillView *view)
{
    if (!view) {
        return;
    }
    const QString viewId = view->id();
    if (m_views.contains(viewId)) {
        m_views.remove(viewId);
    }
}

OVOSController::Status OVOSController::status() const
{
    if (m_reconnectTimer.isActive()) {
        return Connecting;
    }

    switch(m_mainWebSocket.state())
    {
    case QAbstractSocket::ConnectingState:
    case QAbstractSocket::BoundState:
    case QAbstractSocket::HostLookupState:
        return Connecting;
    case QAbstractSocket::UnconnectedState:
        return Closed;
    case QAbstractSocket::ConnectedState:
        return Open;
    case QAbstractSocket::ClosingState:
        return Closing;
    default:
        return Connecting;
    }
}

bool OVOSController::isSpeaking() const
{
    return m_isSpeaking;
}

bool OVOSController::isListening() const
{
    return m_isListening;
}

bool OVOSController::serverReady() const
{
    return m_serverReady;
}

bool OVOSController::useTls() const
{
    return m_useTls;
}

QString OVOSController::authToken() const
{
    return m_authToken;
}

QString OVOSController::sessionId() const
{
    return m_sessionId;
}

QString OVOSController::siteId() const
{
    return m_siteId;
}

void OVOSController::setSessionId(const QString &sessionId)
{
    if (m_sessionId != sessionId) {
        m_sessionId = sessionId;
        emit sessionIdChanged();
    }
}

void OVOSController::setSiteId(const QString &siteId)
{
    if (m_siteId != siteId) {
        m_siteId = siteId;
        emit siteIdChanged();
    }
}

#include "moc_ovoscontroller.cpp"
