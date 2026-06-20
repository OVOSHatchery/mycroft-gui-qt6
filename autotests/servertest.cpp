/*
 * Copyright 2018 by Marco Martin <mart@kde.org>
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

#include <QtTest>
#include <QWebSocket>
#include <QWebSocketServer>
#include <QAbstractItemModel>
#include <QQuickView>
#include <QQmlEngine>
#include "../import/guibusclient.h"
#include "../import/guipage.h"
#include "../import/filereader.h"
#include "../import/globalsettings.h"
#include "../import/namespacemodel.h"
#include "../import/pagemodel.h"
#include "../import/guinamespace.h"
#include "../import/namespacedatamap.h"
#include "../import/namespacedatamodel.h"

class ServerTest : public QObject
{
    Q_OBJECT

public Q_SLOTS:
    void initTestCase();

private Q_SLOTS:
    void testGuiConnection();
    void testActiveSkills();
    void testSessionData();
    void testChangeSessionData();
    void testShowGui();
    void testClientToServerData();
    void testShowSecondGuiPage();
    void testEventsFromServer();
    void testEventsFromClient();
    void testMoveGuiPage();
    void testRemoveGuiPage();
    void testSwitchSkill();

private:
    GuiPage *delegateForSkill(const QString &skill, const QUrl &url);
    QList <GuiPage *>delegatesForSkill(const QString &skill);

    //Client
    GuiBusClient *m_controller;
    GuiNamespace *m_view;

    QQuickView *m_window;

    //Server
    QWebSocketServer *m_mainServerSocket;
    QWebSocketServer *m_guiServerSocket;

    QWebSocket *m_mainWebSocket;
    QWebSocket *m_guiWebSocket;
};


static QObject *fileReaderSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new FileReader;
}

static QObject *globalSettingsSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new GlobalSettings;
}

static QObject *mycroftControllerSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return GuiBusClient::instance();
}



GuiPage *ServerTest::delegateForSkill(const QString &skill, const QUrl &url)
{
    PageModel *delegatesModel = m_view->activeNamespaces()->pageModelForNamespace(skill);
    if (!delegatesModel) {
        return nullptr;
    }

    for (auto d : delegatesModel->delegates()) {
        if (d->qmlUrl() == url) {
            return d;
        }
    }
    return nullptr;
}

QList <GuiPage *>ServerTest::delegatesForSkill(const QString &skill)
{
    PageModel *delegatesModel = m_view->activeNamespaces()->pageModelForNamespace(skill);

    if (!delegatesModel) {
        return {};
    }
    return delegatesModel->delegates();
}

void ServerTest::initTestCase()
{
    m_mainServerSocket = new QWebSocketServer(QStringLiteral("core"),
                                            QWebSocketServer::NonSecureMode, this);
    m_mainServerSocket->listen(QHostAddress::Any, 8181);
    m_guiServerSocket = new QWebSocketServer(QStringLiteral("gui"),
                                            QWebSocketServer::NonSecureMode, this);
    m_guiServerSocket->listen(QHostAddress::Any, 1818);
    m_controller = GuiBusClient::instance();
    //TODO: delete
    //m_view = new GuiNamespace;
    m_window = new QQuickView;
    bool pluginFound = false;
    for (const auto &path : m_window->engine()->importPathList()) {
        QDir importDir(path);
        if (importDir.entryList().contains(QStringLiteral("Mycroft"))) {
            pluginFound = true;
            break;
        }
    }

    if (!pluginFound) {
        qmlRegisterSingletonType<GuiBusClient>("Mycroft", 1, 0, "MycroftController", mycroftControllerSingletonProvider);
        qmlRegisterSingletonType<GlobalSettings>("Mycroft", 1, 0, "GlobalSettings", globalSettingsSingletonProvider);
        qmlRegisterSingletonType<FileReader>("Mycroft", 1, 0, "FileReader", fileReaderSingletonProvider);
        qmlRegisterType<GuiNamespace>("Mycroft", 1, 0, "GuiNamespace");
        qmlRegisterType<GuiPage>("Mycroft", 1, 0, "GuiPage");

        qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/AudioPlayer.qml")), "Mycroft", 1, 0, "AudioPlayer");
        qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/AutoFitLabel.qml")), "Mycroft", 1, 0, "AutoFitLabel");
        qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/Delegate.qml")), "Mycroft", 1, 0, "Delegate");
        qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/PaginatedText.qml")), "Mycroft", 1, 0, "PaginatedText");
        qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/ProportionalDelegate.qml")), "Mycroft", 1, 0, "ProportionalDelegate");
        qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/ScrollableDelegate.qml")), "Mycroft", 1, 0, "ScrollableDelegate");
        qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/SkillView.qml")), "Mycroft", 1, 0, "SkillView");
        qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/SlideShow.qml")), "Mycroft", 1, 0, "SlideShow");
        qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/SlidingImage.qml")), "Mycroft", 1, 0, "SlidingImage");
        qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/StatusIndicator.qml")), "Mycroft", 1, 0, "StatusIndicator");
        qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/VideoPlayer.qml")), "Mycroft", 1, 0, "VideoPlayer");

        qmlRegisterUncreatableType<NamespaceModel>("Mycroft", 1, 0, "NamespaceModel", QStringLiteral("You cannot instantiate items of type NamespaceModel"));
        qmlRegisterUncreatableType<PageModel>("Mycroft", 1, 0, "PageModel", QStringLiteral("You cannot instantiate items of type PageModel"));
        qmlRegisterUncreatableType<NamespaceDataMap>("Mycroft", 1, 0, "NamespaceDataMap", QStringLiteral("You cannot instantiate items of type NamespaceDataMap"));

        qmlRegisterType(QUrl::fromLocalFile(QFINDTESTDATA(QStringLiteral("../import/qml/Delegate.qml"))), "Mycroft", 1, 0, "Delegate");

        qmlProtectModule("Mycroft", 1);
    }

    m_window->setResizeMode(QQuickView::SizeRootObjectToView);
    m_window->resize(400, 800);

    //Load the AbstractSkillview from our specialization in QML
    m_window->setSource(QUrl::fromLocalFile(QFINDTESTDATA("../import/qml/SkillView.qml")));
    if (m_window->errors().length() > 0) {
        qWarning() << m_window->errors();
    }
    m_window->show();
    m_view = qobject_cast<GuiNamespace *>(m_window->rootObject());
    QVERIFY(m_view);

    new QAbstractItemModelTester(m_view->activeNamespaces(), QAbstractItemModelTester::FailureReportingMode::QtTest, this);
}

//TODO: test a spotty connection
void ServerTest::testGuiConnection()
{
    QSignalSpy newConnectionSpy(m_mainServerSocket, &QWebSocketServer::newConnection);
    QSignalSpy controllerSocketStatusChangedSpy(m_controller, &GuiBusClient::socketStatusChanged);
    m_controller->start();

    //wait the server received a connection and the client got connected state
    newConnectionSpy.wait();

    m_mainWebSocket = m_mainServerSocket->nextPendingConnection();
    QSignalSpy textFromMainSpy(m_mainWebSocket, &QWebSocket::textMessageReceived);
    QVERIFY(m_mainWebSocket);

    controllerSocketStatusChangedSpy.wait();
    QCOMPARE(m_controller->status(), GuiBusClient::Open);

    textFromMainSpy.wait();
    auto doc = QJsonDocument::fromJson(textFromMainSpy.first().first().toString().toLatin1());
    auto type = doc[QStringLiteral("type")].toString();
    auto guiId = doc[QStringLiteral("data")][QStringLiteral("gui_id")].toString();

    QCOMPARE(type, QStringLiteral("mycroft.gui.connected"));
    QVERIFY(guiId.length() > 0);

    //Now, connect the gui
    QSignalSpy newGuiConnectionSpy(m_guiServerSocket, &QWebSocketServer::newConnection);
    m_mainWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.gui.port\", \"data\": {\"gui_id\": \"%1\", \"port\": 1818}}").arg(guiId));
    newGuiConnectionSpy.wait();
    m_guiWebSocket = m_guiServerSocket->nextPendingConnection();
    QVERIFY(m_guiWebSocket);
}

void ServerTest::testActiveSkills()
{
    QSignalSpy skillInsertedSpy(m_view->activeNamespaces(), &NamespaceModel::rowsInserted);
    QSignalSpy skillMovedSpy(m_view->activeNamespaces(), &NamespaceModel::rowsMoved);
    QSignalSpy skillRemovedSpy(m_view->activeNamespaces(), &NamespaceModel::rowsRemoved);

    QCOMPARE(m_view->activeNamespaces()->rowCount(), 0);

    //Add weather skill
    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.list.insert\", \"namespace\": \"mycroft.system.active_skills\", \"position\": 0, \"data\": [{\"skill_id\": \"mycroft.weather\"}]}"));

    skillInsertedSpy.wait();

    QCOMPARE(m_view->activeNamespaces()->rowCount(), 1);
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(0,0), NamespaceModel::NamespaceId), QStringLiteral("mycroft.weather"));

    //Add food-wizard skill, before weather
    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.list.insert\", \"namespace\": \"mycroft.system.active_skills\", \"position\": 0, \"data\": [{\"skill_id\": \"aiix.food-wizard\"}]}"));

    skillInsertedSpy.wait();

    QCOMPARE(m_view->activeNamespaces()->rowCount(), 2);
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(0,0), NamespaceModel::NamespaceId), QStringLiteral("aiix.food-wizard"));
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(1,0), NamespaceModel::NamespaceId), QStringLiteral("mycroft.weather"));

    //Add timer skill, between food-wizard and weather
    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.list.insert\", \"namespace\": \"mycroft.system.active_skills\", \"position\": 1, \"data\": [{\"skill_id\": \"mycroft.timer\"}]}"));

    skillInsertedSpy.wait();

    QCOMPARE(m_view->activeNamespaces()->rowCount(), 3);
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(0,0), NamespaceModel::NamespaceId), QStringLiteral("aiix.food-wizard"));
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(1,0), NamespaceModel::NamespaceId), QStringLiteral("mycroft.timer"));
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(2,0), NamespaceModel::NamespaceId), QStringLiteral("mycroft.weather"));

    //Add shopping skill, wiki and weather in the end: weather will be ignored as is already present
    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.list.insert\", \"namespace\": \"mycroft.system.active_skills\", \"position\": 3, \"data\": [{\"skill_id\": \"aiix.shopping-demo\"}, {\"skill_id\": \"mycroft.wiki\"}, {\"skill_id\": \"mycroft.weather\"}]}"));

    skillInsertedSpy.wait();

    //5 because weather was ignored
    QCOMPARE(m_view->activeNamespaces()->rowCount(), 5);
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(0,0), NamespaceModel::NamespaceId), QStringLiteral("aiix.food-wizard"));
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(1,0), NamespaceModel::NamespaceId), QStringLiteral("mycroft.timer"));
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(2,0), NamespaceModel::NamespaceId), QStringLiteral("mycroft.weather"));
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(3,0), NamespaceModel::NamespaceId), QStringLiteral("aiix.shopping-demo"));
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(4,0), NamespaceModel::NamespaceId), QStringLiteral("mycroft.wiki"));

    //Move timer in first position
    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.list.move\", \"namespace\": \"mycroft.system.active_skills\", \"from\": 2, \"to\": 1, \"items_number\": 1}"));

    skillMovedSpy.wait();

    QCOMPARE(m_view->activeNamespaces()->rowCount(), 5);
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(0,0), NamespaceModel::NamespaceId), QStringLiteral("aiix.food-wizard"));
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(1,0), NamespaceModel::NamespaceId), QStringLiteral("mycroft.weather"));
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(2,0), NamespaceModel::NamespaceId), QStringLiteral("mycroft.timer"));
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(3,0), NamespaceModel::NamespaceId), QStringLiteral("aiix.shopping-demo"));
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(4,0), NamespaceModel::NamespaceId), QStringLiteral("mycroft.wiki"));

    //Move weather and food-wizard in front
    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.list.move\", \"namespace\": \"mycroft.system.active_skills\", \"from\": 1, \"to\": 0, \"items_number\": 2}"));

    skillMovedSpy.wait();

    QCOMPARE(m_view->activeNamespaces()->rowCount(), 5);
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(0,0), NamespaceModel::NamespaceId), QStringLiteral("mycroft.weather"));
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(1,0), NamespaceModel::NamespaceId), QStringLiteral("mycroft.timer"));
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(2,0), NamespaceModel::NamespaceId), QStringLiteral("aiix.food-wizard"));
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(3,0), NamespaceModel::NamespaceId), QStringLiteral("aiix.shopping-demo"));
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(4,0), NamespaceModel::NamespaceId), QStringLiteral("mycroft.wiki"));

    //Move timer and food-wizard in the back
    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.list.move\", \"namespace\": \"mycroft.system.active_skills\", \"from\": 1, \"to\": 4, \"items_number\": 2}"));

    skillMovedSpy.wait();

    QCOMPARE(m_view->activeNamespaces()->rowCount(), 5);
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(0,0), NamespaceModel::NamespaceId), QStringLiteral("mycroft.weather"));
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(1,0), NamespaceModel::NamespaceId), QStringLiteral("aiix.shopping-demo"));
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(2,0), NamespaceModel::NamespaceId), QStringLiteral("mycroft.timer"));
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(3,0), NamespaceModel::NamespaceId), QStringLiteral("aiix.food-wizard"));
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(4,0), NamespaceModel::NamespaceId), QStringLiteral("mycroft.wiki"));

    //Remove shopping-demo and timer
    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.list.remove\", \"namespace\": \"mycroft.system.active_skills\", \"position\": 1, \"items_number\": 2}"));

    skillRemovedSpy.wait();

    QCOMPARE(m_view->activeNamespaces()->rowCount(), 3);
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(0,0), NamespaceModel::NamespaceId), QStringLiteral("mycroft.weather"));
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(1,0), NamespaceModel::NamespaceId), QStringLiteral("aiix.food-wizard"));
    QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(2,0), NamespaceModel::NamespaceId), QStringLiteral("mycroft.wiki"));
}

void ServerTest::testSessionData()
{
    NamespaceDataMap *map = m_view->namespaceDataForNamespace(QStringLiteral("mycroft.weather"));
    QVERIFY(map);
    //No sessiondata for invalid skills
    QVERIFY(!m_view->namespaceDataForNamespace(QStringLiteral("invalidskillid")));

    QSignalSpy dataChangedSpy(map, &NamespaceDataMap::valueChanged);

    //set data for weather skill
    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.set\", \"namespace\": \"mycroft.weather\", \"data\": {\"temperature\": \"28°C\", \"icon\": \"weather-clear\", \"forecast\":[{\"when\": \"Monday\", \"temperature\": \"13°C\", \"icon\": \"weather-clouds\"}, {\"when\": \"Tuesday\", \"temperature\": \"24°C\", \"icon\": \"overcast\"}, {\"when\": \"Wednesday\", \"temperature\": \"22°C\", \"icon\": \"weather-showers-day\"}]}}"));

    dataChangedSpy.wait();
    QCOMPARE(dataChangedSpy.count(), 3);

    QCOMPARE(map->keys().count(), 3);
    QCOMPARE(map->value(QStringLiteral("temperature")), QStringLiteral("28°C"));
    QCOMPARE(map->value(QStringLiteral("icon")), QStringLiteral("weather-clear"));

    //Verify the model contents, setting the whole list means reset of the model
    NamespaceDataModel *dm = map->value(QStringLiteral("forecast")).value<NamespaceDataModel *>();
    QVERIFY(dm);
    new QAbstractItemModelTester(dm, QAbstractItemModelTester::FailureReportingMode::QtTest, this);
    QCOMPARE(dm->rowCount(), 3);

    QCOMPARE(dm->data(dm->index(0, 0), dm->roleNames().key("when")).toString(), QStringLiteral("Monday"));
    QCOMPARE(dm->data(dm->index(0, 0), dm->roleNames().key("temperature")).toString(), QStringLiteral("13°C"));
    QCOMPARE(dm->data(dm->index(0, 0), dm->roleNames().key("icon")).toString(), QStringLiteral("weather-clouds"));

    QCOMPARE(dm->data(dm->index(1, 0), dm->roleNames().key("when")).toString(), QStringLiteral("Tuesday"));
    QCOMPARE(dm->data(dm->index(1, 0), dm->roleNames().key("temperature")).toString(), QStringLiteral("24°C"));
    QCOMPARE(dm->data(dm->index(1, 0), dm->roleNames().key("icon")).toString(), QStringLiteral("overcast"));

    QCOMPARE(dm->data(dm->index(2, 0), dm->roleNames().key("when")).toString(), QStringLiteral("Wednesday"));
    QCOMPARE(dm->data(dm->index(2, 0), dm->roleNames().key("temperature")).toString(), QStringLiteral("22°C"));
    QCOMPARE(dm->data(dm->index(2, 0), dm->roleNames().key("icon")).toString(), QStringLiteral("weather-showers-day"));
}

void ServerTest::testChangeSessionData()
{
    NamespaceDataMap *map = m_view->namespaceDataForNamespace(QStringLiteral("mycroft.weather"));
    QVERIFY(map);
    QVERIFY(!m_view->namespaceDataForNamespace(QStringLiteral("invalidskillid")));

    QSignalSpy dataChangedSpy(map, &NamespaceDataMap::valueChanged);
    QSignalSpy dataClearedSpy(map, &NamespaceDataMap::dataCleared);

    //set data for weather skill
    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.set\", \"namespace\": \"mycroft.weather\", \"data\": {\"temperature\": \"24°C\", \"otherproperty\": \"value\"}}"));

    dataChangedSpy.wait();
    QCOMPARE(dataChangedSpy.count(), 2);

    //keys are alphabetically ordered
    QCOMPARE(map->keys().count(), 4);
    QCOMPARE(map->value(QStringLiteral("temperature")), QStringLiteral("24°C"));
    QCOMPARE(map->value(QStringLiteral("icon")), QStringLiteral("weather-clear"));
    QCOMPARE(map->value(QStringLiteral("otherproperty")), QStringLiteral("value"));

    //remove otherproperty
    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.delete\", \"namespace\": \"mycroft.weather\", \"property\": \"otherproperty\"}"));

    dataClearedSpy.wait();
    QCOMPARE(dataClearedSpy.first().first(), QStringLiteral("otherproperty"));
    //is not possible to actually remove a key
    QCOMPARE(map->keys().count(), 4);
    QCOMPARE(map->value(QStringLiteral("temperature")), QStringLiteral("24°C"));
    QCOMPARE(map->value(QStringLiteral("icon")), QStringLiteral("weather-clear"));
    QCOMPARE(map->value(QStringLiteral("otherproperty")), QVariant());

    //Change a value in the model of forecasts
    NamespaceDataModel *dm = map->value(QStringLiteral("forecast")).value<NamespaceDataModel *>();
    QVERIFY(dm);
    QSignalSpy modelDataChangedSpy(dm, &NamespaceDataModel::dataChanged);
    QSignalSpy modelDataInsertedSpy(dm, &NamespaceDataModel::rowsInserted);
    QSignalSpy modelDataMovedSpy(dm, &NamespaceDataModel::rowsMoved);
    QSignalSpy modelDataRemovedSpy(dm, &NamespaceDataModel::rowsRemoved);
    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.list.update\", \"namespace\": \"mycroft.weather\", \"property\": \"forecast\", \"position\": 1, \"data\": [{\"temperature\": \"30°C\", \"icon\": \"weather-clear\", \"to_delete\": \"value to delete\"}]}"));
    modelDataChangedSpy.wait();

    QCOMPARE(dm->data(dm->index(1, 0), dm->roleNames().key("temperature")).toString(), QStringLiteral("30°C"));
    QCOMPARE(dm->data(dm->index(1, 0), dm->roleNames().key("icon")).toString(), QStringLiteral("weather-clear"));

    //Insert a new value in the forecasts model
    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.list.insert\", \"namespace\": \"mycroft.weather\", \"property\": \"forecast\", \"position\": 1, \"data\": [{\"when\": \"Thursday\", \"temperature\": \"2°C\", \"icon\": \"weather-snow\"}, {\"when\": \"Friday\", \"temperature\": \"12°C\", \"icon\": \"weather-few-clouds\"}]}"));
    modelDataInsertedSpy.wait();

    QCOMPARE(dm->rowCount(), 5);

    QCOMPARE(dm->data(dm->index(0, 0), dm->roleNames().key("when")).toString(), QStringLiteral("Monday"));
    QCOMPARE(dm->data(dm->index(0, 0), dm->roleNames().key("temperature")).toString(), QStringLiteral("13°C"));
    QCOMPARE(dm->data(dm->index(0, 0), dm->roleNames().key("icon")).toString(), QStringLiteral("weather-clouds"));

    QCOMPARE(dm->data(dm->index(1, 0), dm->roleNames().key("when")).toString(), QStringLiteral("Thursday"));
    QCOMPARE(dm->data(dm->index(1, 0), dm->roleNames().key("temperature")).toString(), QStringLiteral("2°C"));
    QCOMPARE(dm->data(dm->index(1, 0), dm->roleNames().key("icon")).toString(), QStringLiteral("weather-snow"));

    QCOMPARE(dm->data(dm->index(2, 0), dm->roleNames().key("when")).toString(), QStringLiteral("Friday"));
    QCOMPARE(dm->data(dm->index(2, 0), dm->roleNames().key("temperature")).toString(), QStringLiteral("12°C"));
    QCOMPARE(dm->data(dm->index(2, 0), dm->roleNames().key("icon")).toString(), QStringLiteral("weather-few-clouds"));

    QCOMPARE(dm->data(dm->index(3, 0), dm->roleNames().key("when")).toString(), QStringLiteral("Tuesday"));
    QCOMPARE(dm->data(dm->index(3, 0), dm->roleNames().key("temperature")).toString(), QStringLiteral("30°C"));
    QCOMPARE(dm->data(dm->index(3, 0), dm->roleNames().key("icon")).toString(), QStringLiteral("weather-clear"));

    QCOMPARE(dm->data(dm->index(4, 0), dm->roleNames().key("when")).toString(), QStringLiteral("Wednesday"));
    QCOMPARE(dm->data(dm->index(4, 0), dm->roleNames().key("temperature")).toString(), QStringLiteral("22°C"));
    QCOMPARE(dm->data(dm->index(4, 0), dm->roleNames().key("icon")).toString(), QStringLiteral("weather-showers-day"));

    //Move Thursday and Friday at the bottom of the list
    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.list.move\", \"namespace\": \"mycroft.weather\", \"property\": \"forecast\", \"from\": 1, \"to\": 5, \"items_number\": 2}"));

    modelDataMovedSpy.wait();
    QCOMPARE(dm->rowCount(), 5);
    QCOMPARE(dm->data(dm->index(0, 0), dm->roleNames().key("when")).toString(), QStringLiteral("Monday"));
    QCOMPARE(dm->data(dm->index(0, 0), dm->roleNames().key("temperature")).toString(), QStringLiteral("13°C"));
    QCOMPARE(dm->data(dm->index(0, 0), dm->roleNames().key("icon")).toString(), QStringLiteral("weather-clouds"));

    QCOMPARE(dm->data(dm->index(1, 0), dm->roleNames().key("when")).toString(), QStringLiteral("Tuesday"));
    QCOMPARE(dm->data(dm->index(1, 0), dm->roleNames().key("temperature")).toString(), QStringLiteral("30°C"));
    QCOMPARE(dm->data(dm->index(1, 0), dm->roleNames().key("icon")).toString(), QStringLiteral("weather-clear"));

    QCOMPARE(dm->data(dm->index(2, 0), dm->roleNames().key("when")).toString(), QStringLiteral("Wednesday"));
    QCOMPARE(dm->data(dm->index(2, 0), dm->roleNames().key("temperature")).toString(), QStringLiteral("22°C"));
    QCOMPARE(dm->data(dm->index(2, 0), dm->roleNames().key("icon")).toString(), QStringLiteral("weather-showers-day"));

    QCOMPARE(dm->data(dm->index(3, 0), dm->roleNames().key("when")).toString(), QStringLiteral("Thursday"));
    QCOMPARE(dm->data(dm->index(3, 0), dm->roleNames().key("temperature")).toString(), QStringLiteral("2°C"));
    QCOMPARE(dm->data(dm->index(3, 0), dm->roleNames().key("icon")).toString(), QStringLiteral("weather-snow"));

    QCOMPARE(dm->data(dm->index(4, 0), dm->roleNames().key("when")).toString(), QStringLiteral("Friday"));
    QCOMPARE(dm->data(dm->index(4, 0), dm->roleNames().key("temperature")).toString(), QStringLiteral("12°C"));
    QCOMPARE(dm->data(dm->index(4, 0), dm->roleNames().key("icon")).toString(), QStringLiteral("weather-few-clouds"));

    //Remove Thursday and Friday from the
    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.list.remove\", \"namespace\": \"mycroft.weather\", \"property\": \"forecast\", \"position\": 3, \"items_number\": 2}"));
    modelDataRemovedSpy.wait();

    QCOMPARE(dm->rowCount(), 3);
    QCOMPARE(dm->data(dm->index(0, 0), dm->roleNames().key("when")).toString(), QStringLiteral("Monday"));
    QCOMPARE(dm->data(dm->index(0, 0), dm->roleNames().key("temperature")).toString(), QStringLiteral("13°C"));
    QCOMPARE(dm->data(dm->index(0, 0), dm->roleNames().key("icon")).toString(), QStringLiteral("weather-clouds"));

    QCOMPARE(dm->data(dm->index(1, 0), dm->roleNames().key("when")).toString(), QStringLiteral("Tuesday"));
    QCOMPARE(dm->data(dm->index(1, 0), dm->roleNames().key("temperature")).toString(), QStringLiteral("30°C"));
    QCOMPARE(dm->data(dm->index(1, 0), dm->roleNames().key("icon")).toString(), QStringLiteral("weather-clear"));

    QCOMPARE(dm->data(dm->index(2, 0), dm->roleNames().key("when")).toString(), QStringLiteral("Wednesday"));
    QCOMPARE(dm->data(dm->index(2, 0), dm->roleNames().key("temperature")).toString(), QStringLiteral("22°C"));
    QCOMPARE(dm->data(dm->index(2, 0), dm->roleNames().key("icon")).toString(), QStringLiteral("weather-showers-day"));
}

void ServerTest::testShowGui()
{
    QSignalSpy skillModelDataChangedSpy(m_view->activeNamespaces(), &NamespaceModel::dataChanged);

    const QUrl url(QStringLiteral("file://") + QFINDTESTDATA("currentweather.qml"));

    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.gui.list.insert\", \"namespace\": \"mycroft.weather\", \"position\": 0, \"data\": [{\"url\": \"") + url.toString() + QStringLiteral("\"}]}"));

    skillModelDataChangedSpy.wait();

    GuiPage *delegate = delegateForSkill(QStringLiteral("mycroft.weather"), url);
    QVERIFY(delegate);
    QCOMPARE(delegate->skillId(), QStringLiteral("mycroft.weather"));
    QCOMPARE(delegate->qmlUrl(), url);

    //check the delegate has the proper data associated
    NamespaceDataMap *map = qobject_cast<NamespaceDataMap *>(delegate->namespaceData());
    QVERIFY(map);
    QCOMPARE(map->keys().count(), 4);
    QCOMPARE(map->value(QStringLiteral("temperature")), QStringLiteral("24°C"));
    QCOMPARE(map->value(QStringLiteral("icon")), QStringLiteral("weather-clear"));
    QCOMPARE(map->value(QStringLiteral("otherproperty")), QVariant());

    //try to get the delegate via the model, like qml will do and check they're the same
    PageModel *dm = m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(0, 0), NamespaceModel::Pages).value<PageModel *>();
    QVERIFY(dm);
    new QAbstractItemModelTester(dm, QAbstractItemModelTester::FailureReportingMode::QtTest, this);
    GuiPage *delegate2 = dm->data(dm->index(0, 0), PageModel::PageUi).value<GuiPage *>();
    QCOMPARE(delegate, delegate2);
}

void ServerTest::testClientToServerData()
{
    const QUrl url(QStringLiteral("file://") + QFINDTESTDATA("currentweather.qml"));
    GuiPage *delegate = delegateForSkill(QStringLiteral("mycroft.weather"), url);
    QVERIFY(delegate);

    QSignalSpy propertySpy(m_guiWebSocket, &QWebSocket::textMessageReceived);

    QMetaObject::invokeMethod(delegate, "updateTemperature", Qt::DirectConnection, Q_ARG(QVariant, QStringLiteral("21 °C")));
    propertySpy.wait();

    QJsonDocument doc = QJsonDocument::fromJson(propertySpy.first().first().toString().toUtf8());

    QVERIFY(!doc.isEmpty());
    QCOMPARE(doc[QStringLiteral("type")], QStringLiteral("mycroft.session.set"));
    QCOMPARE(doc[QStringLiteral("namespace")], QStringLiteral("mycroft.weather"));
    QCOMPARE(doc[QStringLiteral("data")][QStringLiteral("temperature")], QStringLiteral("21 °C"));

    QMetaObject::invokeMethod(delegate, "deleteProperty");
    propertySpy.wait();
    doc = QJsonDocument::fromJson(propertySpy[1].first().toString().toUtf8());

    QCOMPARE(doc[QStringLiteral("type")], QStringLiteral("mycroft.session.delete"));
    QCOMPARE(doc[QStringLiteral("namespace")], QStringLiteral("mycroft.weather"));
    QCOMPARE(doc[QStringLiteral("property")], QStringLiteral("to_delete"));
}

void ServerTest::testShowSecondGuiPage()
{
    QSignalSpy skillModelDataChangedSpy(m_view->activeNamespaces(), &NamespaceModel::dataChanged);

    const QUrl url(QStringLiteral("file://") + QFINDTESTDATA("forecast.qml"));
    //wait a moment before showing it there
    QTest::qWait(2000);
    
    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.gui.list.insert\", \"namespace\": \"mycroft.weather\", \"position\": 1, \"data\": [{\"url\": \"") + url.toString() + QStringLiteral("\"}]}"));

    skillModelDataChangedSpy.wait();

    GuiPage *delegate = delegateForSkill(QStringLiteral("mycroft.weather"), url);
    QVERIFY(delegate);
    QCOMPARE(delegate->skillId(), QStringLiteral("mycroft.weather"));
    QCOMPARE(delegate->qmlUrl(), url);
}

void ServerTest::testEventsFromServer()
{
    GuiPage *delegate = delegatesForSkill(QStringLiteral("mycroft.weather")).first();
    QVERIFY(delegate);
    QCOMPARE(delegate->skillId(), QStringLiteral("mycroft.weather"));

    QSignalSpy eventSpy(delegate, &GuiPage::guiEvent);

    //An event of the weather skill
    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.events.triggered\", \"namespace\": \"mycroft.weather\", \"event_name\": \"show_alert\", \"data\": {\"alert_name\": \"blizzard\", \"condition\": \"severe\"}}"));

    eventSpy.wait();
    QCOMPARE(eventSpy.count(), 1);
    QCOMPARE(eventSpy.first().first(), QStringLiteral("show_alert"));
    QCOMPARE(eventSpy.first()[1].value<QVariantMap>()[QStringLiteral("alert_name")], QStringLiteral("blizzard"));
    QCOMPARE(eventSpy.first()[1].value<QVariantMap>()[QStringLiteral("condition")], QStringLiteral("severe"));

    //A system event
    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.events.triggered\", \"namespace\": \"system\", \"event_name\": \"system.next\", \"data\": {}}"));

    eventSpy.wait();
    QCOMPARE(eventSpy.count(), 2);
    QCOMPARE(eventSpy[1].first(), QStringLiteral("system.next"));

    //An event for the wiki skill: the weather one will never receive it
    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.events.triggered\", \"namespace\": \"mycroft.wiki\", \"event_name\": \"new_data\", \"data\": {}}"));

    eventSpy.wait(1000);
    QCOMPARE(eventSpy.count(), 2);

    //view switches again to current
    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.events.triggered\", \"namespace\": \"mycroft.weather\", \"event_name\": \"page_gained_focus\", \"data\": {\"number\": 0}}"));

    //Wait a moment before messing with the gui
    QTest::qWait(1000);
}

void ServerTest::testEventsFromClient()
{
    GuiPage *delegate = delegatesForSkill(QStringLiteral("mycroft.weather")).first();
    QVERIFY(delegate);
    QCOMPARE(delegate->skillId(), QStringLiteral("mycroft.weather"));

    QSignalSpy eventSpy(m_guiWebSocket, &QWebSocket::textMessageReceived);

    //skill own event
    delegate->triggerGuiEvent(QStringLiteral("mycroft.weather.refresh_forecast"), QVariantMap({{QStringLiteral("when"), QStringLiteral("Monday")}}));
    eventSpy.wait();

    QJsonDocument doc = QJsonDocument::fromJson(eventSpy.first().first().toString().toUtf8());
    QVERIFY(!doc.isEmpty());
    QCOMPARE(doc[QStringLiteral("type")], QStringLiteral("mycroft.events.triggered"));
    QCOMPARE(doc[QStringLiteral("namespace")], QStringLiteral("mycroft.weather"));
    QCOMPARE(doc[QStringLiteral("event_name")], QStringLiteral("mycroft.weather.refresh_forecast"));
    QCOMPARE(doc[QStringLiteral("parameters")][QStringLiteral("when")], QStringLiteral("Monday"));

    //system event
    delegate->triggerGuiEvent(QStringLiteral("system.next"), QVariantMap());
    eventSpy.wait();

    doc = QJsonDocument::fromJson(eventSpy[1].first().toString().toUtf8());
    QVERIFY(!doc.isEmpty());
    QCOMPARE(doc[QStringLiteral("type")], QStringLiteral("mycroft.events.triggered"));
    QCOMPARE(doc[QStringLiteral("event_name")], QStringLiteral("system.next"));
    QCOMPARE(doc[QStringLiteral("namespace")], QStringLiteral("system"));

    QCOMPARE(eventSpy.count(), 2);
    //Wait a moment before messing with the gui
    QTest::qWait(1000);
}


void ServerTest::testMoveGuiPage()
{
    QUrl currentUrl = QUrl::fromLocalFile(QFINDTESTDATA("currentweather.qml"));
    QUrl forecastUrl = QUrl::fromLocalFile(QFINDTESTDATA("forecast.qml"));

    PageModel *delegatesModel = m_view->activeNamespaces()->pageModelForNamespace(QStringLiteral("mycroft.weather"));
    QVERIFY(delegatesModel);
    
    QSignalSpy rowsMovedSpy(delegatesModel, &PageModel::rowsMoved);

    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.gui.list.move\", \"namespace\": \"mycroft.weather\", \"items_number\": 1, \"from\": 1, \"to\": 0}"));
    QTest::qWait(1000);

    rowsMovedSpy.wait();

    GuiPage *delegate = delegatesModel->data(delegatesModel->index(0,0), PageModel::PageUi).value<GuiPage *>();
    QVERIFY(delegate);
    QCOMPARE(delegate->qmlUrl(), forecastUrl);

    GuiPage *delegate2 = delegatesModel->data(delegatesModel->index(1,0), PageModel::PageUi).value<GuiPage *>();
    QVERIFY(delegate2);
    QCOMPARE(delegate2->qmlUrl(), currentUrl);
}

void ServerTest::testRemoveGuiPage()
{
    QUrl currentUrl = QUrl::fromLocalFile(QFINDTESTDATA("currentweather.qml"));
    QUrl forecastUrl = QUrl::fromLocalFile(QFINDTESTDATA("forecast.qml"));

    GuiPage *delegate = delegateForSkill(QStringLiteral("mycroft.weather"), forecastUrl);

    PageModel *delegatesModel = m_view->activeNamespaces()->pageModelForNamespace(QStringLiteral("mycroft.weather"));
    QVERIFY(delegatesModel);
    QCOMPARE(delegatesModel->rowCount(), 2);
    
    QSignalSpy rowsRemovedSpy(delegatesModel, &PageModel::rowsRemoved);
    QSignalSpy destroyedSpy(delegate, &QObject::destroyed);

    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.gui.list.remove\", \"namespace\": \"mycroft.weather\", \"items_number\": 1, \"position\": 0}"));
    QTest::qWait(1000);

    rowsRemovedSpy.wait();

    QCOMPARE(delegatesModel->rowCount(), 1);
    delegate = delegatesModel->data(delegatesModel->index(0,0), PageModel::PageUi).value<GuiPage *>();
    QVERIFY(delegate);
    QCOMPARE(delegate->qmlUrl(), currentUrl);

    destroyedSpy.wait();
    QCOMPARE(destroyedSpy.count(), 1);
}

void ServerTest::testSwitchSkill()
{
    NamespaceDataMap *map = m_view->namespaceDataForNamespace(QStringLiteral("mycroft.wiki"));
    QVERIFY(map);

    PageModel *delegatesModel = m_view->activeNamespaces()->pageModelForNamespace(QStringLiteral("mycroft.wiki"));
    QVERIFY(delegatesModel);

    QSignalSpy dataChangedSpy(map, &NamespaceDataMap::valueChanged);
    QSignalSpy skillMovedSpy(m_view->activeNamespaces(), &NamespaceModel::rowsMoved);
    QSignalSpy delegateInsertedSpy(delegatesModel, &PageModel::rowsInserted);

    QUrl url = QUrl::fromLocalFile(QFINDTESTDATA("wiki.qml"));

    //set wiki skill as active
     m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.list.move\", \"namespace\": \"mycroft.system.active_skills\", \"from\": 2, \"to\": 0, \"items_number\": 1}"));
    skillMovedSpy.wait();

    //set data for wiki skill
    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.set\", \"namespace\": \"mycroft.wiki\", \"data\": {\"title\": \"Mycroft\", \"text\": \"Mycroft is a free and open-source voice assistant for Linux-based operating systems that uses a natural language user interface.\", \"image\":\"https://upload.wikimedia.org/wikipedia/en/f/f1/Mycroft_logo.png\"}}"));
    dataChangedSpy.wait();

    //show wiki gui
    m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.gui.list.insert\", \"namespace\": \"mycroft.wiki\", \"position\": 0, \"data\": [{\"url\": \"") + url.toString() + QStringLiteral("\"}]}"));
    delegateInsertedSpy.wait();
    

    //wait a moment before quitting
    QTest::qWait(3000);
}

QTEST_MAIN(ServerTest);

#include "servertest.moc"
