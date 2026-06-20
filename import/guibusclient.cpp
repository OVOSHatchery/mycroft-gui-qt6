/*
 * ============================================================================
 * GuiBusClient - WebSocket Connection Manager for OVOS GUI
 * ============================================================================
 * 
 * WHAT THIS FILE DOES:
 * This is the heart of the GUI client. It manages a WebSocket connection to
 * the OVOS core and routes messages between OVOS and the QML UI.
 * 
 * CONNECTION FLOW:
 * 1. OVOSCore (Python) sends gui messages -> ovos-gui service
 * 2. ovos-gui sends to legacy-plugin (port 18181) 
 * 3. THIS FILE connects to legacy-plugin via WebSocket
 * 4. Messages are parsed and forwarded to GuiNamespace for rendering
 * 
 * MESSAGE TYPES WE HANDLE:
 * - STATE: speaking, listening, ready (update UI indicators)
 * - SESSION: namespace activation, data updates (populate namespace data)
 * - GUI: page show/hide/move (render namespace templates)
 * - EVENTS: namespace events like button clicks (forward to namespaces)
 * 
 * KEY CONCEPTS:
 * - Singleton: Only one instance exists (GuiBusClient::instance())
 * - WebSocket: Qt's QWebSocket for network communication
 * - Signals/Slots: Qt's event system for message passing
 * - Properties: QML-accessible variables with change notifications
 * 
 * FOR NEW DEVELOPERS:
 * - Look at sendRequest() to see how to send messages to OVOS
 * - Look at onMainSocketReceived() to see how we process incoming messages
 * - Look at registerNamespace() to understand namespace lifecycle
 * ============================================================================
 */

#include "guibusclient.h"
#include "mediaservice.h"
#include "guibusmessages.h"
#include "globalsettings.h"
#include "guipage.h"
#include "namespacemodel.h"
#include "guinamespace.h"
#include "controllerconfig.h"

#include <QtGlobal>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QFileInfo>
#include <QJsonDocument>
#include <QDebug>
#include <QProcess>
#include <QQmlPropertyMap>
#include <QStandardItemModel>
#include <QQmlEngine>
#include <QQmlContext>
#include <QUuid>
#include <QWebSocket>


/**
 * Get the singleton instance of GuiBusClient
 */
GuiBusClient *GuiBusClient::instance()
{
    static GuiBusClient* s_self = new GuiBusClient();
    return s_self;
}


/**
 * Constructor - Initializes the WebSocket connection and settings
 */
GuiBusClient::GuiBusClient(QObject *parent)
    : QObject(parent),
      m_appSettingObj(new GlobalSettings)
{
    m_useTls = qgetenv("MYCROFT_GUI_TLS").toInt() == 1 ||
               qgetenv("MYCROFT_GUI_TLS").toLower() == "true";
    
    m_authToken = QString::fromUtf8(qgetenv("MYCROFT_GUI_TOKEN"));
    m_sessionId = QStringLiteral("default");

    QString envSiteId = QString::fromUtf8(qgetenv("MYCROFT_SITE_ID"));
    if (!envSiteId.isEmpty()) {
        m_siteId = envSiteId;
    } else {
        m_siteId = QStringLiteral("default");
    }

    connect(&m_mainWebSocket, &QWebSocket::connected, this,
            [this] () {
                m_reconnectTimer.stop();
                m_reconnectTimer.setInterval(1000);
                emit socketStatusChanged();
            });
    
    connect(&m_mainWebSocket, &QWebSocket::disconnected, this, &GuiBusClient::closed);
    
    connect(&m_mainWebSocket, &QWebSocket::stateChanged, this,
            [this] (QAbstractSocket::SocketState state) {
                emit socketStatusChanged();
                if (state == QAbstractSocket::ConnectedState) {
                    qInfo() << "GUI WebSocket: connected";
                    for (auto namespaceObj : m_namespaces) {
                        sendRequest(QStringLiteral("mycroft.gui.connected"),
                                    QVariantMap({{QStringLiteral("gui_id"), namespaceObj->id()},
                                                 {QStringLiteral("session_id"), m_sessionId},
                                                 {QStringLiteral("site_id"), m_siteId}}));
                    }
                } else if (state == QAbstractSocket::UnconnectedState) {
                    qInfo() << "GUI WebSocket: disconnected";
                    if (m_serverReady) {
                        m_serverReady = false;
                        emit serverReadyChanged();
                    }
                } else if (state == QAbstractSocket::ConnectingState) {
                    qInfo() << "GUI WebSocket: connecting...";
                } else {
                    if (m_serverReady) {
                        m_serverReady = false;
                        emit serverReadyChanged();
                    }
                }
            });

    connect(&m_mainWebSocket, &QWebSocket::textMessageReceived, this, &GuiBusClient::onMainSocketMessageReceived);

    m_reconnectTimer.setInterval(1000);
    connect(&m_reconnectTimer, &QTimer::timeout, this, [this]() {
        m_mainWebSocket.open(QUrl(buildWebSocketUrl()));
        int nextInterval = qMin(m_reconnectTimer.interval() * 2, 30000);
        m_reconnectTimer.setInterval(nextInterval);
    });
}


/**
 * Build the WebSocket URL for connection
 */
QString GuiBusClient::buildWebSocketUrl() const
{
    QString envHost = QString::fromUtf8(qgetenv("MYCROFT_GUI_HOST"));
    QString host = envHost.isEmpty() ? m_appSettingObj->webSocketAddress() : envHost;

    int port = qgetenv("MYCROFT_GUI_PORT").toInt();
    if (port == 0) {
        port = m_appSettingObj->webSocketPort();
    }

    QString route = m_appSettingObj->webSocketRoute();
    QString scheme = m_useTls ? QStringLiteral("wss") : QStringLiteral("ws");
    QString path = m_authToken.isEmpty() ? route : route + QStringLiteral("?token=") + m_authToken;
    QString url = QStringLiteral("%1://%2:%3%4").arg(scheme).arg(host).arg(port).arg(path);
    qDebug() << "WebSocket URL:" << url;
    return url;
}


/**
 * Start/Connect to OVOS WebSocket server
 */
void GuiBusClient::start()
{
    m_mainWebSocket.open(QUrl(buildWebSocketUrl()));
    connect(&m_mainWebSocket, &QWebSocket::errorOccurred,
            this, [this] (const QAbstractSocket::SocketError &error) {
        qInfo() << "GUI WebSocket: error" << error;
        if (error != QAbstractSocket::HostNotFoundError && error != QAbstractSocket::ConnectionRefusedError) {
            qWarning() << "OVOS is running but the connection failed for some reason.";
            return;
        }

        m_reconnectTimer.start();
        emit socketStatusChanged();
    });
    emit socketStatusChanged();
}


/**
 * Disconnect from OVOS WebSocket server
 */
void GuiBusClient::disconnectSocket()
{
    m_mainWebSocket.close();
    m_reconnectTimer.stop();
    emit socketStatusChanged();
}


/**
 * Disconnect and immediately start reconnecting
 */
void GuiBusClient::reconnect()
{
    m_mainWebSocket.close();
    m_reconnectTimer.start();
    emit socketStatusChanged();
}


/**
 * MESSAGE ROUTING - THE HEART OF THE CLIENT
 */
void GuiBusClient::onMainSocketMessageReceived(const QString &message)
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

    qDebug() << "← recv:" << typeStr << "| payload:" << message.left(500);

    auto parsed = GuiBusMessages::parseMessage(typeStr);

    switch (parsed.layer) {

    case GuiBusMessages::MessageLayer::WIRE: {
        auto wire = parsed.wire;

        if (wire == GuiBusMessages::WireMessage::FORWARD_ASSISTANT) {
            const QString eventType = doc[QStringLiteral("event_type")].toString();
            if (!eventType.isEmpty()) {
                auto evt = GuiBusMessages::assistantFromString(eventType);
                handleAssistantEvent(evt, doc);
            }
            return;
        }

        if (wire == GuiBusMessages::WireMessage::FORWARD_SHELL) {
            for (auto namespaceObj : m_namespaces) {
                namespaceObj->handleIncomingMessage(message);
            }
            return;
        }

        if (wire == GuiBusMessages::WireMessage::CLEAR_NAMESPACE) {
            const QString namespace_ = doc[QStringLiteral("data")][QStringLiteral("namespace")].toString();
            if (!namespace_.isEmpty()) {
                for (auto namespaceObj : m_namespaces) {
                    namespaceObj->handleIncomingMessage(message);
                }
            }
            return;
        }

        qDebug() << "  → namespaces (WIRE):" << typeStr;
        for (auto namespaceObj : m_namespaces) {
            namespaceObj->handleIncomingMessage(message);
        }
        return;
    }

    case GuiBusMessages::MessageLayer::SHELL:
        qDebug() << "  → namespaces (SHELL):" << typeStr;

        if (typeStr.startsWith(QStringLiteral("gui.player."))) {
            mediaService()->onMainSocketIntentReceived(typeStr, doc[QStringLiteral("data")].toVariant().toMap());
            return;
        }

        for (auto namespaceObj : m_namespaces) {
            namespaceObj->handleIncomingMessage(message);
        }
        return;

    case GuiBusMessages::MessageLayer::ASSISTANT:
        qWarning() << "Received bare assistant event (expected FORWARD_ASSISTANT wrapper):" << typeStr;
        return;

    case GuiBusMessages::MessageLayer::UNKNOWN:
        if (typeStr.contains(QLatin1Char(':')) && !doc[QStringLiteral("data")][QStringLiteral("utterance")].toString().isEmpty()) {
            const QString namespace_id = typeStr.split(QLatin1Char(':')).first();
            if (namespace_id.contains(QLatin1Char('.'))) {
                qDebug() << "Current namespace:" << namespace_id;
                emit utteranceManagedByNamespace(namespace_id);
            }
        }
        return;
    }
}


/**
 * Handle assistant state events from OVOS
 */
void GuiBusClient::handleAssistantEvent(GuiBusMessages::AssistantEvent evt, const QJsonDocument &doc)
{
    qDebug() << "  assistant:" << GuiBusMessages::toString(evt);
    
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

    if (evt == GuiBusMessages::AssistantEvent::NAMESPACES_LOADED_RESPONSE) {
        if (doc[QStringLiteral("data")][QStringLiteral("status")].toBool() == true) {
            m_serverReady = true;
            emit serverReadyChanged();
        }
    } else if (evt == GuiBusMessages::AssistantEvent::READY) {
        m_serverReady = true;
        emit serverReadyChanged();
    }

    if (evt == GuiBusMessages::AssistantEvent::SCREEN_CLOSE_IDLE_EVENT) {
        QString namespace_idle_event_id = doc[QStringLiteral("data")][QStringLiteral("namespace_idle_event_id")].toString();
        emit namespaceTimeoutReceived(namespace_idle_event_id);
    }
}


/**
 * Send a request to OVOS core
 */
void GuiBusClient::sendRequest(const QString &type, const QVariantMap &data, const QVariantMap &context)
{
    if (m_mainWebSocket.state() != QAbstractSocket::ConnectedState) {
        qWarning() << "OVOS connection not open!";
        return;
    }

    QJsonObject root;
    root[QStringLiteral("type")] = type;
    root[QStringLiteral("data")] = QJsonObject::fromVariantMap(data);

    QJsonObject contextJson = QJsonObject::fromVariantMap(context);
    if (!contextJson.contains(QStringLiteral("session"))) {
        QJsonObject session;
        session[QStringLiteral("session_id")] = QStringLiteral("default");
        contextJson[QStringLiteral("session")] = session;
    }

    root[QStringLiteral("context")] = contextJson;

    QJsonDocument doc(root);
    qDebug() << "→ send:" << type << "| payload:" << QString::fromUtf8(doc.toJson(QJsonDocument::Compact)).left(500);
    m_mainWebSocket.sendTextMessage(QString::fromUtf8(doc.toJson()));
}


/**
 * Send binary message to OVOS
 */
void GuiBusClient::sendBinary(const QString &type, const QJsonObject &data, const QVariantMap &context)
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


/**
 * Send user text/utterance to OVOS for processing
 */
void GuiBusClient::sendText(const QString &message)
{
    sendRequest(QStringLiteral("gui.client.utterance"), QVariantMap({{QStringLiteral("utterances"), QStringList({message})}}), QVariantMap({{QStringLiteral("source"), QStringLiteral("ovos-gui")}, {QStringLiteral("destination"), QStringLiteral("skills")}}));
}


/**
 * Send confirmation response (yes/no dialog)
 */
void GuiBusClient::sendConfirmResponse(bool confirmed)
{
    sendRequest(QStringLiteral("gui.client.confirm.response"), QVariantMap({{QStringLiteral("confirmed"), confirmed}}));
}


/**
 * Send selection response (choice dialog)
 */
void GuiBusClient::sendSelectResponse(const QVariant &selection)
{
    sendRequest(QStringLiteral("gui.client.select.response"), QVariantMap({{QStringLiteral("selection"), selection}}));
}


/**
 * Register a namespace with the GuiBusClient
 */
void GuiBusClient::registerNamespace(GuiNamespace *namespaceObj)
{
    Q_ASSERT(!namespaceObj->id().isEmpty());
    Q_ASSERT(!m_namespaces.contains(namespaceObj->id()));
    m_namespaces[namespaceObj->id()] = namespaceObj;
    connect(namespaceObj, &QObject::destroyed, this, [this, namespaceObj]() {
        deregisterNamespace(namespaceObj);
    });
    if (m_mainWebSocket.state() == QAbstractSocket::ConnectedState) {
        sendRequest(QStringLiteral("mycroft.gui.connected"),
                    QVariantMap({{QStringLiteral("gui_id"), namespaceObj->id()},
                                 {QStringLiteral("session_id"), m_sessionId},
                                 {QStringLiteral("site_id"), m_siteId}}));
    }
}


/**
 * Deregister a namespace
 */
void GuiBusClient::deregisterNamespace(GuiNamespace *namespaceObj)
{
    if (!namespaceObj) {
        return;
    }
    const QString namespaceId = namespaceObj->id();
    if (m_namespaces.contains(namespaceId)) {
        m_namespaces.remove(namespaceId);
    }
}


/**
 * Get current connection status
 */
GuiBusClient::Status GuiBusClient::status() const
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


/**
 * Is OVOS currently speaking?
 */
bool GuiBusClient::isSpeaking() const
{
    return m_isSpeaking;
}


/**
 * Is OVOS currently listening?
 */
bool GuiBusClient::isListening() const
{
    return m_isListening;
}


/**
 * Is OVOS server ready (namespaces loaded)?
 */
bool GuiBusClient::serverReady() const
{
    return m_serverReady;
}


/**
 * Is TLS enabled for WebSocket?
 */
bool GuiBusClient::useTls() const
{
    return m_useTls;
}


/**
 * Get the authentication token
 */
QString GuiBusClient::authToken() const
{
    return m_authToken;
}


/**
 * Get the session ID
 */
QString GuiBusClient::sessionId() const
{
    return m_sessionId;
}


/**
 * Get the site ID (which screen)
 */
QString GuiBusClient::siteId() const
{
    return m_siteId;
}


/**
 * Set the session ID
 */
void GuiBusClient::setSessionId(const QString &sessionId)
{
    if (m_sessionId != sessionId) {
        m_sessionId = sessionId;
        emit sessionIdChanged();
    }
}


/**
 * Set the site ID (which screen)
 */
void GuiBusClient::setSiteId(const QString &siteId)
{
    if (m_siteId != siteId) {
        m_siteId = siteId;
        emit siteIdChanged();
    }
}

void GuiBusClient::setMediaService(MediaService *service)
{
    m_mediaService = service;
}

MediaService *GuiBusClient::mediaService() const
{
    return m_mediaService;
}

NamespaceModel *GuiBusClient::activeNamespaces() const
{
    if (!m_namespaces.isEmpty()) {
        return m_namespaces.values().first()->activeNamespaces();
    }
    return nullptr;
}

#include "moc_guibusclient.cpp"
