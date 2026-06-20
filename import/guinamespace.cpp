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

#include "guinamespace.h"
#include "guibusmessages.h"
#include "namespacemodel.h"
#include "guipage.h"
#include "namespacedatamap.h"
#include "namespacedatamodel.h"
#include "pagemodel.h"
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
    // Try built-in resource first (for monolithic app support)
    const QString qrcPath = QStringLiteral("qrc:/system-templates/") + templateName;
    qDebug() << "[view] Resolving SYSTEM:" << templateName << "-> qrcPath:" << qrcPath;

    const QString envDir = qEnvironmentVariable("OVOS_SYSTEM_TEMPLATES");
    if (!envDir.isEmpty()) {
        const QString envPath = envDir + QLatin1Char('/') + templateName;
        if (QFileInfo::exists(envPath)) {
            qDebug() << "[view]   found environment override:" << envPath;
            return QUrl::fromLocalFile(envPath);
        }
    }
    
    // Default to bundled resource
    return QUrl(qrcPath);
}

static QUrl resolvePage(const QString &urlString)
{
    static const QString systemPrefix = QStringLiteral("SYSTEM:");
    if (urlString.startsWith(systemPrefix)) {
        return resolveSystemTemplate(urlString.mid(systemPrefix.length()));
    }
    return QUrl::fromUserInput(urlString);
}

GuiNamespace::GuiNamespace(QQuickItem *parent)
    : QQuickItem(parent),
      m_id(QUuid::createUuid().toString()),
      m_controller(GuiBusClient::instance())
{
    m_activeNamespacesModel = new NamespaceModel(this);
    m_controller->registerNamespace(this);

    connect(m_controller, &GuiBusClient::socketStatusChanged, this,
            [this]() {
                if (m_controller->status() != GuiBusClient::Open) {
                    m_activeNamespacesModel->removeRows(0, m_activeNamespacesModel->rowCount());
                    // Clear all namespace data when socket disconnects
                    for (auto it = m_namespaceDataMaps.begin(); it != m_namespaceDataMaps.end(); ++it) {
                        it.value()->deleteLater();
                    }
                    m_namespaceDataMaps.clear();
                    // Clean up translators to prevent stale state on reconnect
                    for (auto it = m_translatorsForNamespace.begin(); it != m_translatorsForNamespace.end(); ++it) {
                        QCoreApplication::removeTranslator(it.value());
                        delete it.value();
                    }
                    m_translatorsForNamespace.clear();
                    emit closed();
                }
                emit statusChanged();
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

    connect(m_controller, &GuiBusClient::utteranceManagedByNamespace, this,
        [this](const QString &namespace_id) {
            m_activeNamespacesModel->checkGuiActivation(namespace_id);
        });
}

GuiNamespace::~GuiNamespace()
{
}

QString GuiNamespace::id() const
{
    return m_id;
}

void GuiNamespace::handleIncomingMessage(const QString &message)
{
    onGuiSocketMessageReceived(message);
}

void GuiNamespace::triggerGuiEvent(const QString &eventName, const QVariantMap &parameters)
{
    QVariantMap data;
    data[QStringLiteral("namespace")] = m_id;
    data[QStringLiteral("event_name")] = eventName;
    data[QStringLiteral("parameters")] = parameters;
    m_controller->sendRequest(QStringLiteral("mycroft.events.triggered"), data);
}

GuiBusClient::Status GuiNamespace::status() const
{
    return m_controller->status();
}

NamespaceModel *GuiNamespace::activeNamespaces() const
{
    return m_activeNamespacesModel;
}

NamespaceDataMap *GuiNamespace::namespaceDataForNamespace(const QString &namespace_id)
{
    NamespaceDataMap *map = nullptr;

    if (m_namespaceDataMaps.contains(namespace_id)) {
        map = m_namespaceDataMaps[namespace_id];
    } else if (m_activeNamespacesModel->namespaceIndex(namespace_id).isValid()) {
        map = new NamespaceDataMap(namespace_id, this);
        m_namespaceDataMaps[namespace_id] = map;
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

void GuiNamespace::onGuiSocketMessageReceived(const QString &message)
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

    auto parsed = GuiBusMessages::parseMessage(typeStr);

    // All messages handled here are wire protocol messages (namespace data, GUI pages, events).
    if (parsed.layer != GuiBusMessages::MessageLayer::WIRE) {
        return;
    }
    auto wireMsg = parsed.wire;
    qDebug() << "[view]" << typeStr << "| active namespaces:" << m_activeNamespacesModel->activeNamespaces();

//BEGIN NAMESPACEDATA
    if (wireMsg == GuiBusMessages::WireMessage::SESSION_SET) {
        const QString namespace_id = doc[QStringLiteral("namespace")].toString();
        const QVariantMap data = doc[QStringLiteral("data")].toVariant().toMap();

        if (namespace_id.isEmpty()) {
            qWarning() << "Empty namespace_id in mycroft.session.set";
            return;
        }
        if (!m_activeNamespacesModel->namespaceIndex(namespace_id).isValid()) {
            qWarning() << "Invalid namespace_id in mycroft.session.set:" << namespace_id;
            return;
        }
        if (data.isEmpty()) {
            qWarning() << "Empty data in mycroft.session.set";
            return;
        }

        NamespaceDataMap *map = namespaceDataForNamespace(namespace_id);
        if (!map) {
            return;
        }
        QVariantMap::const_iterator i;
        for (i = data.constBegin(); i != data.constEnd(); ++i) {
            // Only try to convert to a model if the value is actually a list
            if (i.value().typeId() == QMetaType::QVariantList) {
                QList<QVariantMap> list = variantListToOrderedMap(i.value().value<QVariantList>());
                NamespaceDataModel *dm = map->value(i.key()).value<NamespaceDataModel *>();

                if (!list.isEmpty()) {
                    if (!dm) {
                        dm = new NamespaceDataModel(map);
                        qDebug() << "[session] Created new model for key:" << i.key();
                        map->insertAndNotify(i.key(), QVariant::fromValue(dm));
                    } else {
                        dm->clear();
                    }
                    qDebug() << "[session] Inserting" << list.count() << "items into model for key:" << i.key();
                    dm->insertData(0, list);
                    continue;
                }
            }

            // Insert it as a simple value
            qDebug() << "[session] Setting simple value for key:" << i.key() << "| type:" << i.value().typeName();
            NamespaceDataModel *dm = map->value(i.key()).value<NamespaceDataModel *>();
            if (dm) {
                dm->deleteLater();
            }
            map->insertAndNotify(i.key(), i.value());
        }

    } else if (wireMsg == GuiBusMessages::WireMessage::SESSION_DELETE) {
        const QString namespace_id = doc[QStringLiteral("namespace")].toString();
        const QString property = doc[QStringLiteral("property")].toString();
        if (namespace_id.isEmpty()) {
            qWarning() << "No namespace_id provided in mycroft.session.delete";
            return;
        }
        if (!m_activeNamespacesModel->namespaceIndex(namespace_id).isValid()) {
            qWarning() << "Invalid namespace_id in mycroft.session.delete:" << namespace_id;
            return;
        }
        if (property.isEmpty()) {
            qWarning() << "No property provided in mycroft.session.delete";
            return;
        }

        NamespaceDataMap *map = namespaceDataForNamespace(namespace_id);
        NamespaceDataModel *dm = map->value(property).value<NamespaceDataModel *>();
        map->clearAndNotify(property);
        if (dm) {
            dm->deleteLater();
        }
//END NAMESPACEDATA


//BEGIN ACTIVENAMESPACES
    // Insert new active namespace
    } else if (wireMsg == GuiBusMessages::WireMessage::SESSION_LIST_INSERT && doc[QStringLiteral("namespace")].toString() == QLatin1String("mycroft.system.active_skills")) {
        const int position = doc[QStringLiteral("position")].toInt();

        if (position < 0 || position > m_activeNamespacesModel->rowCount()) {
            qWarning() << "Error: Invalid position in mycroft.session.list.insert of mycroft.system.active_skills";
            return;
        }

        const QStringList namespaceList = jsonModelToStringList(QStringLiteral("namespace_id"), doc[QStringLiteral("data")]);

        if (namespaceList.isEmpty()) {
            qWarning() << "Error: no valid namespaces received in mycroft.session.list.insert of mycroft.system.active_skills";
            return;
        }

        qDebug() << "[view] inserting namespaces at pos" << position << ":" << namespaceList;
        m_activeNamespacesModel->insertNamespaces(position, namespaceList);
        qDebug() << "[view] active namespaces after insert:" << m_activeNamespacesModel->activeNamespaces();


    // Active namespace removed
    } else if (wireMsg == GuiBusMessages::WireMessage::SESSION_LIST_REMOVE && doc[QStringLiteral("namespace")].toString() == QLatin1String("mycroft.system.active_skills")) {
        const int position = doc[QStringLiteral("position")].toInt();
        const int itemsNumber = doc[QStringLiteral("items_number")].toInt();

        if (position < 0 || position > m_activeNamespacesModel->rowCount() - 1) {
            qWarning() << "Error: Invalid position in mycroft.session.list.remove of mycroft.system.active_skills";
            return;
        }
        if (itemsNumber < 0 || itemsNumber > m_activeNamespacesModel->rowCount() - position) {
            qWarning() << "Error: Invalid items_number in mycroft.session.list.remove of mycroft.system.active_skills";
            return;
        }

        for (int i = 0; i < itemsNumber; ++i) {
            const QString namespace_id = m_activeNamespacesModel->data(m_activeNamespacesModel->index(position+i, 0)).toString();

            if (m_translatorsForNamespace.contains(namespace_id)) {
                QTranslator *translator = m_translatorsForNamespace[namespace_id];
                QCoreApplication::removeTranslator(translator);
                m_translatorsForNamespace.remove(namespace_id);
                delete translator;
            }
            {
                auto i = m_namespaceDataMaps.find(namespace_id);
                if (i != m_namespaceDataMaps.end()) {
                    i.value()->deleteLater();
                    m_namespaceDataMaps.erase(i);
                }
            }
        }
        m_activeNamespacesModel->removeRows(position, itemsNumber);

    // Active namespace moved
    } else if (wireMsg == GuiBusMessages::WireMessage::SESSION_LIST_MOVE && doc[QStringLiteral("namespace")].toString() == QLatin1String("mycroft.system.active_skills")) {
        const int from = doc[QStringLiteral("from")].toInt();
        const int to = doc[QStringLiteral("to")].toInt();
        const int itemsNumber = doc[QStringLiteral("items_number")].toInt();

        if (from < 0 || from > m_activeNamespacesModel->rowCount() - 1) {
            qWarning() << "Error: Invalid from position in mycroft.session.list.move of mycroft.system.active_skills";
            return;
        }
        if (to < 0 || to > m_activeNamespacesModel->rowCount() - 1) {
            qWarning() << "Error: Invalid to position in mycroft.session.list.move of mycroft.system.active_skills";
            return;
        }
        if (itemsNumber <= 0 || itemsNumber > m_activeNamespacesModel->rowCount() - from) {
            qWarning() << "Error: Invalid items_number in mycroft.session.list.move of mycroft.system.active_skills";
            return;
        }

        m_activeNamespacesModel->moveRows(QModelIndex(), from, itemsNumber, QModelIndex(), to);
//END ACTIVENAMESPACES


//BEGIN PAGE MODEL
    // Insert new pages
    } else if (wireMsg == GuiBusMessages::WireMessage::GUI_LIST_INSERT) {
        const QString namespace_id = doc[QStringLiteral("namespace")].toString();
        if (namespace_id.isEmpty()) {
            qWarning() << "No namespace_id provided in mycroft.gui.list.insert";
            return;
        }

        const int position = doc[QStringLiteral("position")].toInt();

        qDebug() << "[view] GUI_LIST_INSERT for namespace:" << namespace_id
                 << "| in active list:" << m_activeNamespacesModel->activeNamespaces().contains(namespace_id);
        PageModel *pageModel = m_activeNamespacesModel->pageModelForNamespace(namespace_id);

        if (!pageModel) {
            qWarning() << "Error: no page model for namespace" << namespace_id
                       << "| active namespaces:" << m_activeNamespacesModel->activeNamespaces();
            return;
        }
        if (position < 0 || position > pageModel->rowCount()) {
            qWarning() << "Error: Invalid position in mycroft.gui.list.insert";
            return;
        }

        const QStringList pageUrls = jsonModelToStringList(QStringLiteral("url"), doc[QStringLiteral("data")]);

        if (pageUrls.isEmpty()) {
            qWarning() << "Error: no valid pages received in mycroft.gui.list.insert";
            return;
        }

        qWarning() << "Arrived mycroft.gui.list.insert, pageUrls are" << pageUrls;

        QList <PageLoader *> pageLoaders;
        for (const auto &urlString : pageUrls) {
            const QUrl pageUrl = resolvePage(urlString);

            if (!pageUrl.isValid()) {
                continue;
            }

            PageLoader *loader = new PageLoader(this);
            loader->init(namespace_id, pageUrl);

            qWarning() << "Created a new PageLoader" << loader << "which will load" << pageUrl << "for the namespace" << namespace_id;

            if (!m_translatorsForNamespace.contains(namespace_id)) {
                QTranslator *translator = new QTranslator(this);
                if (translator->load(QLocale(), namespace_id, QLatin1String("_"), loader->translationsUrl().path())) {
                    QCoreApplication::installTranslator(translator);
                    m_translatorsForNamespace[namespace_id] = translator;
                } else {
                    translator->deleteLater();
                }
            }

            connect(loader, &QObject::destroyed, &m_trimComponentsTimer, QOverload<>::of(&QTimer::start));

            pageLoaders << loader;
        }

        if (pageLoaders.count() > 0) {
            pageModel->insertPageLoaders(position, pageLoaders);
            //give the focus to the first
            pageLoaders.first()->setFocus(true);
        }


    // Pages removed
    } else if (wireMsg == GuiBusMessages::WireMessage::GUI_LIST_REMOVE) {
        const QString namespace_id = doc[QStringLiteral("namespace")].toString();
        if (namespace_id.isEmpty()) {
            qWarning() << "No namespace_id provided in mycroft.gui.list.remove";
            return;
        }

        const int position = doc[QStringLiteral("position")].toInt();
        const int itemsNumber = doc[QStringLiteral("items_number")].toInt();

        PageModel *pageModel = m_activeNamespacesModel->pageModelForNamespace(namespace_id);
        if (!pageModel) {
            qWarning() << "Error: no page model for namespace" << namespace_id;
            return;
        }

        if (position < 0 || position > pageModel->rowCount() - 1) {
            qWarning() << "Error: Invalid position in mycroft.gui.list.remove";
            return;
        }

        if (itemsNumber < 0 || itemsNumber > pageModel->rowCount()) {
            qWarning() << "Error: Invalid items_number in mycroft.gui.list.remove";
            return;
        }

        pageModel->removeRows(position, itemsNumber);

    // Pages moved
    } else if (wireMsg == GuiBusMessages::WireMessage::GUI_LIST_MOVE) {

        const QString namespace_id = doc[QStringLiteral("namespace")].toString();
        if (namespace_id.isEmpty()) {
            qWarning() << "No namespace_id provided in mycroft.gui.list.move";
            return;
        }

        const int from = doc[QStringLiteral("from")].toInt();
        const int to = doc[QStringLiteral("to")].toInt();
        const int itemsNumber = doc[QStringLiteral("items_number")].toInt();

        PageModel *pageModel = m_activeNamespacesModel->pageModelForNamespace(namespace_id);

        if (!pageModel) {
            qWarning() << "Error: no page model for namespace" << namespace_id;
            return;
        }

        if (from < 0 || from > pageModel->rowCount() - 1) {
            qWarning() << "Error: Invalid from position in mycroft.gui.list.move";
            return;
        }
        if (to < 0 || to > pageModel->rowCount() - 1) {
            qWarning() << "Error: Invalid to position in mycroft.gui.list.move";
            return;
        }
        if (itemsNumber <= 0 || itemsNumber > pageModel->rowCount() - from) {
            qWarning() << "Error: Invalid items_number in mycroft.gui.list.move";
            return;
        }
        pageModel->moveRows(QModelIndex(), from, itemsNumber, QModelIndex(), to);
//END PAGE MODELS


//BEGIN DATA MODELS
    // Insert new items in an existing list, or creates one under "property"
    } else if (wireMsg == GuiBusMessages::WireMessage::SESSION_LIST_INSERT && doc[QStringLiteral("namespace")].toString() != QLatin1String("mycroft.system.active_skills")) {
        const QString namespace_id = doc[QStringLiteral("namespace")].toString();
        if (namespace_id.isEmpty()) {
            qWarning() << "No namespace_id provided in mycroft.session.list.insert";
            return;
        }
        const QString &property = doc[QStringLiteral("property")].toString();
        if (property.isEmpty()) {
            qWarning() << "Error: Invalid or empty \"property\" in mycroft.session.list.insert";
            return;
        }

        NamespaceDataMap *map = namespaceDataForNamespace(namespace_id);
        NamespaceDataModel *dm = map->value(property).value<NamespaceDataModel *>();

        if (!dm) {
            dm = new NamespaceDataModel(map);
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

    // Updates the value of items in an existing list
    } else if (wireMsg == GuiBusMessages::WireMessage::SESSION_LIST_UPDATE) {
        const QString namespace_id = doc[QStringLiteral("namespace")].toString();
        if (namespace_id.isEmpty()) {
            qWarning() << "No namespace_id provided in mycroft.session.list.update";
            return;
        }
        const QString &property = doc[QStringLiteral("property")].toString();
        if (property.isEmpty()) {
            qWarning() << "Error: Invalid or empty \"property\" in mycroft.session.list.update";
            return;
        }

        NamespaceDataMap *map = namespaceDataForNamespace(namespace_id);
        NamespaceDataModel *dm = map->value(property).value<NamespaceDataModel *>();

        if (!dm) {
            qWarning() << "Error: no list model existing under property" << property << "in mycroft.session.list.update";
            return;
        }

        const int position = doc[QStringLiteral("position")].toInt();

        if (position < 0 || position > m_activeNamespacesModel->rowCount()) {
            qWarning() << "Error: Invalid position in mycroft.session.list.update";
            return;
        }

        QList<QVariantMap> list = variantListToOrderedMap(doc[QStringLiteral("data")].toVariant().value<QVariantList>());

        if (list.isEmpty()) {
            qWarning() << "Error: invalid data in mycroft.session.list.update:" << doc[QStringLiteral("data")];
            return;
        }

        dm->updateData(position, list);

    // Moves items within an existing list
    } else if (wireMsg == GuiBusMessages::WireMessage::SESSION_LIST_MOVE && doc[QStringLiteral("namespace")].toString() != QLatin1String("mycroft.system.active_skills")) {
        const QString namespace_id = doc[QStringLiteral("namespace")].toString();
        if (namespace_id.isEmpty()) {
            qWarning() << "No namespace_id provided in mycroft.session.list.move";
            return;
        }
        const QString &property = doc[QStringLiteral("property")].toString();
        if (property.isEmpty()) {
            qWarning() << "Error: Invalid or empty \"property\" in mycroft.session.list.move";
            return;
        }

        NamespaceDataMap *map = namespaceDataForNamespace(namespace_id);
        NamespaceDataModel *dm = map->value(property).value<NamespaceDataModel *>();

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

    // Removes items from an existing list
    } else if (wireMsg == GuiBusMessages::WireMessage::SESSION_LIST_REMOVE && doc[QStringLiteral("namespace")].toString() != QLatin1String("mycroft.system.active_skills")) {
        const QString namespace_id = doc[QStringLiteral("namespace")].toString();
        if (namespace_id.isEmpty()) {
            qWarning() << "No namespace_id provided in mycroft.session.list.remove";
            return;
        }
        const QString &property = doc[QStringLiteral("property")].toString();
        if (property.isEmpty()) {
            qWarning() << "Error: Invalid or empty \"property\" in mycroft.session.list.remove";
            return;
        }

        NamespaceDataMap *map = namespaceDataForNamespace(namespace_id);
        NamespaceDataModel *dm = map->value(property).value<NamespaceDataModel *>();

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
        const QString namespaceOrSystem = doc[QStringLiteral("namespace")].toString();

        if (namespaceOrSystem.isEmpty()) {
            qWarning() << "No namespace provided for mycroft.events.triggered";
            return;
        }

        const QString eventName = doc[QStringLiteral("event_name")].toString();
        if (eventName.isEmpty()) {
            qWarning() << "No event_name provided for mycroft.events.triggered";
            return;
        }

        const QVariantMap data = doc[QStringLiteral("data")].toVariant().toMap();

        QList<GuiPage *> pages;

        if (namespaceOrSystem == QLatin1String("system")) {
            for (auto *pageModel : activeNamespaces()->pageModels()) {
                pages << pageModel->pages();
            }
        } else {
            PageModel *pageModel = activeNamespaces()->pageModelForNamespace(namespaceOrSystem);
            if (pageModel) {
                pages << pageModel->pages();
            }
        }

        // page_gained_focus is special
        if (eventName == QStringLiteral("page_gained_focus")) {
            int pos = data.value(QStringLiteral("number")).toInt();
            PageModel *pageModel = activeNamespaces()->pageModelForNamespace(namespaceOrSystem);
            if (pageModel) {
                pageModel->setCurrentIndex(pos);
            }
        } else if (eventName == QStringLiteral("mycroft.gui.close.screen")) {
            emit closed();
        } else {
            for (auto *page : pages) {
                emit page->guiEvent(eventName, data);
            }
        }
    } else {
        qWarning() << "Unrecognized operation" << typeStr;
    }
//END EVENTS
}

#include "moc_guinamespace.cpp"
