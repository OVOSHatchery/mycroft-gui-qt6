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

#include "abstractskillview.h"
#include "guibusmessages.h"
#include "activeskillsmodel.h"
#include "abstractdelegate.h"
#include "sessiondatamap.h"
#include "sessiondatamodel.h"
#include "delegatesmodel.h"
#include "controllerconfig.h"

#include <QWebSocket>
#include <QUuid>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QQmlContext>
#include <QQmlEngine>
#include <QTranslator>
#include <QFileInfo>

// ---------------------------------------------------------------------------
// SYSTEM: URI scheme
//
// The OVOS server sends "SYSTEM:<TemplateName>.qml" instead of a file:// URI.
// This keeps QML resources client-side: the server never needs the Qt install.
//
// Resolution order:
//   1. $OVOS_SYSTEM_TEMPLATES/<TemplateName>.qml   (runtime override)
//   2. OVOS_SYSTEM_TEMPLATES_DIR/<TemplateName>.qml  (compiled-in default)
// ---------------------------------------------------------------------------
static QUrl resolveSystemTemplate(const QString &templateName)
{
    const QString envDir = qEnvironmentVariable("OVOS_SYSTEM_TEMPLATES");
    if (!envDir.isEmpty()) {
        const QString envPath = envDir + QLatin1Char('/') + templateName;
        if (QFileInfo::exists(envPath)) {
            return QUrl::fromLocalFile(envPath);
        }
    }
    return QUrl::fromLocalFile(
        QLatin1String(OVOS_SYSTEM_TEMPLATES_DIR) + QLatin1Char('/') + templateName
    );
}

static QUrl resolveDelegate(const QString &urlString)
{
    static const QString systemPrefix = QStringLiteral("SYSTEM:");
    if (urlString.startsWith(systemPrefix)) {
        return resolveSystemTemplate(urlString.mid(systemPrefix.length()));
    }
    return QUrl::fromUserInput(urlString);
}

AbstractSkillView::AbstractSkillView(QQuickItem *parent)
    : QQuickItem(parent),
      m_id(QUuid::createUuid().toString()),
      m_controller(OVOSController::instance())
{
    m_activeSkillsModel = new ActiveSkillsModel(this);

    m_guiWebSocket = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);
    m_controller->registerView(this);

    connect(m_guiWebSocket, &QWebSocket::connected, this,
            [this] () {
                m_reconnectTimer.stop();
                m_reconnectTimer.setInterval(1000);  // Reset backoff on successful connect
                emit statusChanged();
            });

    connect(m_guiWebSocket, &QWebSocket::disconnected, this, &AbstractSkillView::closed);

    connect(m_guiWebSocket, &QWebSocket::disconnected, this, [this]() {
        m_activeSkillsModel->removeRows(0, m_activeSkillsModel->rowCount());
        // Clear all session data when socket disconnects
        for (auto it = m_skillData.begin(); it != m_skillData.end(); ++it) {
            it.value()->deleteLater();
        }
        m_skillData.clear();
        // Clean up translators to prevent stale state on reconnect
        for (auto it = m_translatorsForSkill.begin(); it != m_translatorsForSkill.end(); ++it) {
            QCoreApplication::removeTranslator(it.value());
            delete it.value();
        }
        m_translatorsForSkill.clear();
    });

    connect(m_guiWebSocket, &QWebSocket::stateChanged, this,
            [this] (QAbstractSocket::SocketState state) {
                emit statusChanged();
            });

    connect(m_guiWebSocket, &QWebSocket::textMessageReceived, this, &AbstractSkillView::onGuiSocketMessageReceived);

    connect(m_guiWebSocket, &QWebSocket::stateChanged, this,
            [this](QAbstractSocket::SocketState socketState) {
                if (socketState == QAbstractSocket::UnconnectedState && m_url.isValid() && m_controller->status() == OVOSController::Open) {
                    m_reconnectTimer.start();
                }
            });

    connect(m_guiWebSocket, &QWebSocket::errorOccurred, this,
            [this](QAbstractSocket::SocketError error) {
                qWarning() << "Gui socket Connection Error:" << error;
                m_reconnectTimer.start();
            });


    connect(m_controller, &OVOSController::socketStatusChanged, this,
            [this]() {
                if (m_controller->status() != OVOSController::Open) {
                    m_guiWebSocket->close();
                    //don't assume the url will be still valid
                    m_url = QUrl();
                }
            });

    // Reconnect timer with exponential backoff (1s, 2s, 4s, 8s, ... up to 30s)
    m_reconnectTimer.setInterval(1000);
    connect(&m_reconnectTimer, &QTimer::timeout, this, [this]() {
        m_guiWebSocket->close();
        m_guiWebSocket->open(m_url);
        // Exponential backoff: double interval up to 30s max
        int nextInterval = qMin(m_reconnectTimer.interval() * 2, 30000);
        m_reconnectTimer.setInterval(nextInterval);
    });

    // Trim components cache timer
    m_trimComponentsTimer.setInterval(100);
    m_trimComponentsTimer.setSingleShot(true);
    connect(&m_trimComponentsTimer, &QTimer::timeout, this, [this]() {
        QQmlEngine *engine = qmlEngine(this);
        if (engine) {
            engine->clearComponentCache();
        }
    });

    connect(m_controller, &OVOSController::utteranceManagedBySkill, this,
        [this](const QString &skillId) {
            m_activeSkillsModel->checkGuiActivation(skillId);
        });
}

AbstractSkillView::~AbstractSkillView()
{
}


QUrl AbstractSkillView::url() const
{
    return m_url;
}

void AbstractSkillView::setUrl(const QUrl &url)
{
    if (m_url == url) {
        return;
    }

    m_url = url;

    //don't connect if the controller is offline
    if (m_controller->status() == OVOSController::Open) {
        m_guiWebSocket->close();
        m_guiWebSocket->open(url);
    }
}

QString AbstractSkillView::id() const
{
    return m_id;
}

void AbstractSkillView::handleIncomingMessage(const QString &message)
{
    onGuiSocketMessageReceived(message);
}

void AbstractSkillView::triggerEvent(const QString &skillId, const QString &eventName, const QVariantMap &parameters)
{
    if (m_guiWebSocket->state() != QAbstractSocket::ConnectedState) {
        qWarning() << "Error: OVOS GUI connection not open!";
        return;
    }
    QJsonObject root;

    root[QStringLiteral("type")] = QStringLiteral("mycroft.events.triggered");
    root[QStringLiteral("namespace")] = skillId;
    root[QStringLiteral("event_name")] = eventName;
    root[QStringLiteral("parameters")] = QJsonObject::fromVariantMap(parameters);

    QJsonDocument doc(root);
    m_guiWebSocket->sendTextMessage(QString::fromUtf8(doc.toJson()));
}

void AbstractSkillView::writeProperties(const QString &skillId, const QVariantMap &data)
{
    if (m_guiWebSocket->state() != QAbstractSocket::ConnectedState) {
        qWarning() << "Error: OVOS GUI connection not open!";
        return;
    }
    QJsonObject root;

    root[QStringLiteral("type")] = QStringLiteral("mycroft.session.set");
    root[QStringLiteral("namespace")] = skillId;
    root[QStringLiteral("data")] = QJsonObject::fromVariantMap(data);

    QJsonDocument doc(root);
    m_guiWebSocket->sendTextMessage(QString::fromUtf8(doc.toJson()));
}

void AbstractSkillView::deleteProperty(const QString &skillId, const QString &property)
{
    if (m_guiWebSocket->state() != QAbstractSocket::ConnectedState) {
        qWarning() << "Error: OVOS GUI connection not open!";
        return;
    }
    QJsonObject root;

    root[QStringLiteral("type")] = QStringLiteral("mycroft.session.delete");
    root[QStringLiteral("namespace")] = skillId;
    root[QStringLiteral("property")] = property;

    QJsonDocument doc(root);
    m_guiWebSocket->sendTextMessage(QString::fromUtf8(doc.toJson()));
}

OVOSController::Status AbstractSkillView::status() const
{
    if (m_reconnectTimer.isActive()) {
        return OVOSController::Connecting;
    }

    switch(m_guiWebSocket->state())
    {
    case QAbstractSocket::ConnectingState:
    case QAbstractSocket::BoundState:
    case QAbstractSocket::HostLookupState:
        return OVOSController::Connecting;
    case QAbstractSocket::UnconnectedState:
        return OVOSController::Closed;
    case QAbstractSocket::ConnectedState:
        return OVOSController::Open;
    case QAbstractSocket::ClosingState:
        return OVOSController::Closing;
    default:
        return OVOSController::Connecting;
    }
}

ActiveSkillsModel *AbstractSkillView::activeSkills() const
{
    return m_activeSkillsModel;
}

SessionDataMap *AbstractSkillView::sessionDataForSkill(const QString &skillId)
{
    SessionDataMap *map = nullptr;

    if (m_skillData.contains(skillId)) {
        map = m_skillData[skillId];
    } else if (m_activeSkillsModel->skillIndex(skillId).isValid()) {
        map = new SessionDataMap(skillId, this);
        m_skillData[skillId] = map;
    }

    return map;
}

QList<QVariantMap> variantListToOrderedMap(const QVariantList &data)
{
    QList<QVariantMap> ordMap;

    QStringList roleNames;

    for (const auto &item : data) {
        if (!item.canConvert<QVariantMap>()) {
            qWarning() << "Error: Array data structure corrupted: " << data;
            return ordMap;
        }
        const auto &map = item.value<QVariantMap>();
        if (roleNames.isEmpty()) {
            roleNames = map.keys();
        } else if (roleNames != map.keys()) {
            qWarning() << "WARNING: Item with a wrong set of roles encountered, some roles will be inaccessible from QML, expected: " << roleNames << "Encountered: " << map.keys();
        }
        ordMap << map;
    }

    return ordMap;
}

QStringList jsonModelToStringList(const QString &key, const QJsonValue &data)
{
    QStringList items;

    if (!data.isArray()) {
        qWarning() << "Error: Model data is not an Array" << data;
        return items;
    }

    const auto &array = data.toArray();
    for (const auto &item : array) {
        if (!item.isObject()) {
            qWarning() << "Error: Array data structure currupted: " << data;
            items.clear();
            return items;
        }
        const auto &obj = item.toObject();
        const auto &value = obj.value(key);
        if (!value.isString()) {
            qWarning() << "Error: item in model not a string" << value;
        }
        items << value.toString();
    }

    return items;
}

void AbstractSkillView::onGuiSocketMessageReceived(const QString &message)
{
    QJsonParseError parseError;
    auto doc = QJsonDocument::fromJson(message.toUtf8(), &parseError);

    if (doc.isEmpty()) {
        qWarning() << "Empty or invalid JSON message arrived on the gui socket:" << message << "Error:" << parseError.errorString();
        return;
    }

    auto typeStr = doc[QStringLiteral("type")].toString();

    if (typeStr.isEmpty()) {
        qWarning() << "Empty type in the JSON message on the gui socket";
        return;
    }

    //qDebug() << "gui message type" << typeStr;

    auto parsed = GuiBusMessages::parseMessage(typeStr);

    // All messages handled here are wire protocol messages (session data, GUI pages, events).
    // Assistant events and shell events are handled by OVOSController, not forwarded here.
    if (parsed.layer != GuiBusMessages::MessageLayer::WIRE) {
        return;
    }
    auto wireMsg = parsed.wire;

//BEGIN SKILLDATA
    if (wireMsg == GuiBusMessages::WireMessage::SESSION_SET) {
        const QString skillId = doc[QStringLiteral("namespace")].toString();
        const QVariantMap data = doc[QStringLiteral("data")].toVariant().toMap();

        if (skillId.isEmpty()) {
            qWarning() << "Empty skill_id in mycroft.session.set";
            return;
        }
        if (!m_activeSkillsModel->skillIndex(skillId).isValid()) {
            qWarning() << "Invalid skill_id in mycroft.session.set:" << skillId;
            return;
        }
        if (data.isEmpty()) {
            qWarning() << "Empty data in mycroft.session.set";
            return;
        }

        //we already checked, assume *map is valid
        SessionDataMap *map = sessionDataForSkill(skillId);
        if (!map) {
            return;
        }
        QVariantMap::const_iterator i;
        for (i = data.constBegin(); i != data.constEnd(); ++i) {
            //insert it as a model
            QList<QVariantMap> list = variantListToOrderedMap(i.value().value<QVariantList>());
            SessionDataModel *dm = map->value(i.key()).value<SessionDataModel *>();

            if (!list.isEmpty()) {
                if (!dm) {
                    dm = new SessionDataModel(map);
                    map->insertAndNotify(i.key(), QVariant::fromValue(dm));
                } else {
                    dm->clear();
                }
                dm->insertData(0, list);

            //insert it as is.
            } else {
                if (dm) {
                    dm->deleteLater();
                }
                map->insertAndNotify(i.key(), i.value());
            }
        }

    // The SkillData was removed by the server
    } else if (wireMsg == GuiBusMessages::WireMessage::SESSION_DELETE) {
        const QString skillId = doc[QStringLiteral("namespace")].toString();
        const QString property = doc[QStringLiteral("property")].toString();
        if (skillId.isEmpty()) {
            qWarning() << "No skill_id provided in mycroft.session.delete";
            return;
        }
        if (!m_activeSkillsModel->skillIndex(skillId).isValid()) {
            qWarning() << "Invalid skill_id in mycroft.session.delete:" << skillId;
            return;
        }
        if (property.isEmpty()) {
            qWarning() << "No property provided in mycroft.session.delete";
            return;
        }

        SessionDataMap *map = sessionDataForSkill(skillId);
        SessionDataModel *dm = map->value(property).value<SessionDataModel *>();
        map->clearAndNotify(property);
        //a model will need to be manually deleted
        if (dm) {
            dm->deleteLater();
        }
//END SKILLDATA


//BEGIN ACTIVESKILLS
    // Insert new active skill
    } else if (wireMsg == GuiBusMessages::WireMessage::SESSION_LIST_INSERT && doc[QStringLiteral("namespace")].toString() == QLatin1String("mycroft.system.active_skills")) {
        const int position = doc[QStringLiteral("position")].toInt();

        if (position < 0 || position > m_activeSkillsModel->rowCount()) {
            qWarning() << "Error: Invalid position in mycroft.session.list.insert of mycroft.system.active_skills";
            return;
        }

        const QStringList skillList = jsonModelToStringList(QStringLiteral("skill_id"), doc[QStringLiteral("data")]);

        if (skillList.isEmpty()) {
            qWarning() << "Error: no valid skills received in mycroft.session.list.insert of mycroft.system.active_skills";
            return;
        }

        m_activeSkillsModel->insertSkills(position, skillList);


    // Active skill removed
    } else if (wireMsg == GuiBusMessages::WireMessage::SESSION_LIST_REMOVE && doc[QStringLiteral("namespace")].toString() == QLatin1String("mycroft.system.active_skills")) {
        const int position = doc[QStringLiteral("position")].toInt();
        const int itemsNumber = doc[QStringLiteral("items_number")].toInt();

        if (position < 0 || position > m_activeSkillsModel->rowCount() - 1) {
            qWarning() << "Error: Invalid position in mycroft.session.list.remove of mycroft.system.active_skills";
            return;
        }
        if (itemsNumber < 0 || itemsNumber > m_activeSkillsModel->rowCount() - position) {
            qWarning() << "Error: Invalid items_number in mycroft.session.list.remove of mycroft.system.active_skills";
            return;
        }

        for (int i = 0; i < itemsNumber; ++i) {

            const QString skillId = m_activeSkillsModel->data(m_activeSkillsModel->index(position+i, 0)).toString();

            if (m_translatorsForSkill.contains(skillId)) {
                QTranslator *translator = m_translatorsForSkill[skillId];
                QCoreApplication::removeTranslator(translator);
                m_translatorsForSkill.remove(skillId);
                delete translator;
            }
            //TODO: do this after an animation
            {
                auto i = m_skillData.find(skillId);
                if (i != m_skillData.end()) {
                    i.value()->deleteLater();
                    m_skillData.erase(i);
                }
            }
        }
        m_activeSkillsModel->removeRows(position, itemsNumber);

    // Active skill moved
    } else if (wireMsg == GuiBusMessages::WireMessage::SESSION_LIST_MOVE && doc[QStringLiteral("namespace")].toString() == QLatin1String("mycroft.system.active_skills")) {
        const int from = doc[QStringLiteral("from")].toInt();
        const int to = doc[QStringLiteral("to")].toInt();
        const int itemsNumber = doc[QStringLiteral("items_number")].toInt();

        if (from < 0 || from > m_activeSkillsModel->rowCount() - 1) {
            qWarning() << "Error: Invalid from position in mycroft.session.list.move of mycroft.system.active_skills";
            return;
        }
        if (to < 0 || to > m_activeSkillsModel->rowCount() - 1) {
            qWarning() << "Error: Invalid to position in mycroft.session.list.move of mycroft.system.active_skills";
            return;
        }
        if (itemsNumber <= 0 || itemsNumber > m_activeSkillsModel->rowCount() - from) {
            qWarning() << "Error: Invalid items_number in mycroft.session.list.move of mycroft.system.active_skills";
            return;
        }

        m_activeSkillsModel->moveRows(QModelIndex(), from, itemsNumber, QModelIndex(), to);
//END ACTIVESKILLS


//BEGIN GUI MODEL
    // Insert new new gui delegates
    } else if (wireMsg == GuiBusMessages::WireMessage::GUI_LIST_INSERT) {
        const QString skillId = doc[QStringLiteral("namespace")].toString();
        if (skillId.isEmpty()) {
            qWarning() << "No skill_id provided in mycroft.gui.list.insert";
            return;
        }

        const int position = doc[QStringLiteral("position")].toInt();

        DelegatesModel *delegatesModel = m_activeSkillsModel->delegatesModelForSkill(skillId);

        if (!delegatesModel) {
            qWarning() << "Error: no delegates model for skill" << skillId;
            return;
        }
        if (position < 0 || position > delegatesModel->rowCount()) {
            qWarning() << "Error: Invalid position in mycroft.gui.list.insert";
            return;
        }

        const QStringList delegateUrls = jsonModelToStringList(QStringLiteral("url"), doc[QStringLiteral("data")]);

        if (delegateUrls.isEmpty()) {
            qWarning() << "Error: no valid skills received in mycroft.gui.list.insert";
            return;
        }

        qWarning() << "Arrived mycroft.gui.list.insert, delegateUrls are" << delegateUrls;

        QList <DelegateLoader *> delegateLoaders;
        for (const auto &urlString : delegateUrls) {
            const QUrl delegateUrl = resolveDelegate(urlString);

            if (!delegateUrl.isValid()) {
                continue;
            }

            DelegateLoader *loader = new DelegateLoader(this);
            loader->init(skillId, delegateUrl);

            qWarning() << "Created a new DelegateLoader" << loader << "which will load" << delegateUrl << "for the skill" << skillId;

            if (!m_translatorsForSkill.contains(skillId)) {
                QTranslator *translator = new QTranslator(this);
                if (translator->load(QLocale(), skillId, QLatin1String("_"), loader->translationsUrl().path())) {
                    QCoreApplication::installTranslator(translator);
                    m_translatorsForSkill[skillId] = translator;
                } else {
                    translator->deleteLater();
                }
            }

            connect(loader, &QObject::destroyed, &m_trimComponentsTimer, QOverload<>::of(&QTimer::start));

            delegateLoaders << loader;
        }

        if (delegateLoaders.count() > 0) {
            delegatesModel->insertDelegateLoaders(position, delegateLoaders);
            //give the focus to the first
            delegateLoaders.first()->setFocus(true);
        }


    // Gui delegates removed
    } else if (wireMsg == GuiBusMessages::WireMessage::GUI_LIST_REMOVE) {
        const QString skillId = doc[QStringLiteral("namespace")].toString();
        if (skillId.isEmpty()) {
            qWarning() << "No skill_id provided in mycroft.gui.list.remove";
            return;
        }

        const int position = doc[QStringLiteral("position")].toInt();
        const int itemsNumber = doc[QStringLiteral("items_number")].toInt();

        //TODO: try with lifecycle managed by the view?
        DelegatesModel *delegatesModel = m_activeSkillsModel->delegatesModelForSkill(skillId);
        if (!delegatesModel) {
            qWarning() << "Error: no delegates model for skill" << skillId;
            return;
        }

        if (position < 0 || position > delegatesModel->rowCount() - 1) {
            qWarning() << "Error: Invalid position in mycroft.gui.list.remove";
            return;
        }

        if (itemsNumber < 0 || itemsNumber > delegatesModel->rowCount()) {
            qWarning() << "Error: Invalid items_number in mycroft.gui.list.remove";
            return;
        }

        delegatesModel->removeRows(position, itemsNumber);

    // Gui delegates moved
    } else if (wireMsg == GuiBusMessages::WireMessage::GUI_LIST_MOVE) {

        const QString skillId = doc[QStringLiteral("namespace")].toString();
        if (skillId.isEmpty()) {
            qWarning() << "No skill_id provided in mycroft.gui.list.move";
            return;
        }

        const int from = doc[QStringLiteral("from")].toInt();
        const int to = doc[QStringLiteral("to")].toInt();
        const int itemsNumber = doc[QStringLiteral("items_number")].toInt();

        DelegatesModel *delegatesModel = m_activeSkillsModel->delegatesModelForSkill(skillId);

        if (!delegatesModel) {
            qWarning() << "Error: no delegates model for skill" << skillId;
            return;
        }

        if (from < 0 || from > delegatesModel->rowCount() - 1) {
            qWarning() << "Error: Invalid from position in mycroft.gui.list.move";
            return;
        }
        if (to < 0 || to > delegatesModel->rowCount() - 1) {
            qWarning() << "Error: Invalid to position in mycroft.gui.list.move";
            return;
        }
        if (itemsNumber <= 0 || itemsNumber > delegatesModel->rowCount() - from) {
            qWarning() << "Error: Invalid items_number in mycroft.gui.list.move";
            return;
        }
        delegatesModel->moveRows(QModelIndex(), from, itemsNumber, QModelIndex(), to);
//END GUI MODELS


//TODO: manage nested models?
//BEGIN DATA MODELS
    // Insert new items in an existing list, or creates one under "property"
    // NOTE: Generic data lists (skill custom lists) - different from active_skills list
    } else if (wireMsg == GuiBusMessages::WireMessage::SESSION_LIST_INSERT && doc[QStringLiteral("namespace")].toString() != QLatin1String("mycroft.system.active_skills")) {
        const QString skillId = doc[QStringLiteral("namespace")].toString();
        if (skillId.isEmpty()) {
            qWarning() << "No skill_id provided in mycroft.session.list.insert";
            return;
        }
        const QString &property = doc[QStringLiteral("property")].toString();
        if (property.isEmpty()) {
            qWarning() << "Error: Invalid or empty \"property\" in mycroft.session.list.insert";
            return;
        }

        SessionDataMap *map = sessionDataForSkill(skillId);
        SessionDataModel *dm = map->value(property).value<SessionDataModel *>();

        if (!dm) {
            dm = new SessionDataModel(map);
            map->insertAndNotify(property, QVariant::fromValue(dm));
        }

        const int position = doc[QStringLiteral("position")].toInt();

        if (position < 0 || position > dm->rowCount()) {
            qWarning() << "Error: Invalid position in mycroft.session.list.insert";
            return;
        }

        QList<QVariantMap> list = variantListToOrderedMap(doc[QStringLiteral("data")].toVariant().value<QVariantList>());

        if (list.isEmpty()) {
            qWarning() << "Error: invalid data in mycroft.session.list.insert:" << doc[QStringLiteral("data")];
            return;
        }

        dm->insertData(position, list);

    // Updates the value of items in an existing list, Error if under "property" no list exists
    } else if (wireMsg == GuiBusMessages::WireMessage::SESSION_LIST_UPDATE) {
        const QString skillId = doc[QStringLiteral("namespace")].toString();
        if (skillId.isEmpty()) {
            qWarning() << "No skill_id provided in mycroft.session.list.update";
            return;
        }
        const QString &property = doc[QStringLiteral("property")].toString();
        if (property.isEmpty()) {
            qWarning() << "Error: Invalid or empty \"property\" in mycroft.session.list.update";
            return;
        }

        SessionDataMap *map = sessionDataForSkill(skillId);
        SessionDataModel *dm = map->value(property).value<SessionDataModel *>();

        if (!dm) {
            qWarning() << "Error: no list model existing under property" << property << "in mycroft.session.list.update";
            return;
        }

        const int position = doc[QStringLiteral("position")].toInt();

        if (position < 0 || position > m_activeSkillsModel->rowCount()) {
            qWarning() << "Error: Invalid position in mycroft.session.list.update";
            return;
        }

        QList<QVariantMap> list = variantListToOrderedMap(doc[QStringLiteral("data")].toVariant().value<QVariantList>());

        if (list.isEmpty()) {
            qWarning() << "Error: invalid data in mycroft.session.list.update:" << doc[QStringLiteral("data")];
            return;
        }

        dm->updateData(position, list);

    // Moves items within an existing list, Error if under "property" no list exists
    // NOTE: Generic data lists (skill custom lists) - different from active_skills list
    } else if (wireMsg == GuiBusMessages::WireMessage::SESSION_LIST_MOVE && doc[QStringLiteral("namespace")].toString() != QLatin1String("mycroft.system.active_skills")) {
        const QString skillId = doc[QStringLiteral("namespace")].toString();
        if (skillId.isEmpty()) {
            qWarning() << "No skill_id provided in mycroft.session.list.move";
            return;
        }
        const QString &property = doc[QStringLiteral("property")].toString();
        if (property.isEmpty()) {
            qWarning() << "Error: Invalid or empty \"property\" in mycroft.session.list.move";
            return;
        }

        SessionDataMap *map = sessionDataForSkill(skillId);
        SessionDataModel *dm = map->value(property).value<SessionDataModel *>();

        if (!dm) {
            qWarning() << "Error: no list model existing under property" << property << "in mycroft.session.list.move";
            return;
        }

        const int from = doc[QStringLiteral("from")].toInt();
        const int to = doc[QStringLiteral("to")].toInt();
        const int itemsNumber = doc[QStringLiteral("items_number")].toInt();

        if (from < 0 || from > dm->rowCount() - 1) {
            qWarning() << "Error: Invalid from position in mycroft.session.list.move";
            return;
        }
        if (to < 0 || to > dm->rowCount()) {
            qWarning() << "Error: Invalid to position in mycroft.session.list.move";
            return;
        }
        if (itemsNumber <= 0 || itemsNumber > dm->rowCount() - from) {
            qWarning() << "Error: Invalid items_number in mycroft.session.list.move";
            return;
        }
        dm->moveRows(QModelIndex(), from, itemsNumber, QModelIndex(), to);

    // Removes items from an existing list, Error if under "property" no list exists
    // NOTE: Generic data lists (skill custom lists) - different from active_skills list
    } else if (wireMsg == GuiBusMessages::WireMessage::SESSION_LIST_REMOVE && doc[QStringLiteral("namespace")].toString() != QLatin1String("mycroft.system.active_skills")) {
        const QString skillId = doc[QStringLiteral("namespace")].toString();
        if (skillId.isEmpty()) {
            qWarning() << "No skill_id provided in mycroft.session.list.remove";
            return;
        }
        const QString &property = doc[QStringLiteral("property")].toString();
        if (property.isEmpty()) {
            qWarning() << "Error: Invalid or empty \"property\" in mycroft.session.list.remove";
            return;
        }

        SessionDataMap *map = sessionDataForSkill(skillId);
        SessionDataModel *dm = map->value(property).value<SessionDataModel *>();

        if (!dm) {
            qWarning() << "Error: no list model existing under property" << property << "in mycroft.session.list.remove";
            return;
        }

        const int position = doc[QStringLiteral("position")].toInt();
        const int itemsNumber = doc[QStringLiteral("items_number")].toInt();

        if (position < 0 || position > dm->rowCount() - 1) {
            qWarning() << "Error: Invalid position in mycroft.session.list.remove";
            return;
        }
        if (itemsNumber < 0 || itemsNumber > dm->rowCount() - position) {
            qWarning() << "Error: Invalid items_number in mycroft.session.list.remove";
            return;
        }

        dm->removeRows(position, itemsNumber);
//END DATA MODELS


//BEGIN EVENTS
    // Action triggered from the server
    } else if (wireMsg == GuiBusMessages::WireMessage::EVENTS_TRIGGERED) {
        const QString skillOrSystem = doc[QStringLiteral("namespace")].toString();

        if (skillOrSystem.isEmpty()) {
            qWarning() << "No namespace provided for mycroft.events.triggered";
            return;
        }

        const QString eventName = doc[QStringLiteral("event_name")].toString();
        if (eventName.isEmpty()) {
            qWarning() << "No event_name provided for mycroft.events.triggered";
            return;
        }

        // data can also be empty
        const QVariantMap data = doc[QStringLiteral("data")].toVariant().toMap();

        QList<AbstractDelegate *> delegates;

        if (skillOrSystem == QLatin1String("system")) {
            for (auto *delegatesModel : activeSkills()->delegatesModels()) {
                delegates << delegatesModel->delegates();
            }
        } else {
            DelegatesModel *delegatesModel = activeSkills()->delegatesModelForSkill(skillOrSystem);
            if (delegatesModel) {
                delegates << delegatesModel->delegates();
            }
        }

        // page_gained_focus is special: interests only one single delegate
        if (eventName == QStringLiteral("page_gained_focus")) {
            int pos = data.value(QStringLiteral("number")).toInt();
            if (pos >= 0 && pos < delegates.count()) {
                AbstractDelegate *delegate = delegates[pos];
                delegate->forceActiveFocus((Qt::FocusReason)ServerEventFocusReason);
                emit delegate->guiEvent(eventName, data);
            }
        } else if (eventName == QStringLiteral("mycroft.gui.close.screen")) {
            emit activeSkillClosed();
        } else {
            for (auto *delegate : delegates) {
                emit delegate->guiEvent(eventName, data);
            }
        }
    } else {
        qWarning() << "Unrecognized operation" << typeStr;
    }
//END EVENTS
}

#include "moc_abstractskillview.cpp"
