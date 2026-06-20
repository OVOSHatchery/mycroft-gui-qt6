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
    void create100Skills();
    void move100Skills();
    void delete100Skills();
    void repeat100();


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

void ServerTest::create100Skills()
{
    QSignalSpy skillInsertedSpy(m_view->activeNamespaces(), &NamespaceModel::rowsInserted);

    QCOMPARE(m_view->activeNamespaces()->rowCount(), 0);

    const QUrl url(QStringLiteral("file://") + QFINDTESTDATA("delegatewithloader.qml"));

    for (int i = 0; i < 10; ++i) {
        const QString id = QString::number(i);

        m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.list.insert\", \"namespace\": \"mycroft.system.active_skills\", \"position\": 0, \"data\": [{\"skill_id\": \"") + id + QStringLiteral("\"}]}"));

        skillInsertedSpy.wait();
        NamespaceDataMap *map = m_view->namespaceDataForNamespace(id);
        QVERIFY(map);

        //set data
        m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.set\", \"namespace\": \"") + id + QStringLiteral("\", \"data\": {\"temperature\": \"24°C\", \"otherproperty\": \"value\"}}"));

        m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.gui.list.insert\", \"namespace\": \"") + id + QStringLiteral("\", \"position\": 0, \"data\": [{\"url\": \"") + url.toString() + QStringLiteral("\"}]}"));

        m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.set\", \"namespace\": \"") + id + QStringLiteral("\", \"data\": {\"temperature\": \"24°C\", \"otherproperty\": \"value\", \"state\": \"subdelegate1\"}}"));
    }

    for (int i = 0; i < 10; ++i) {
        const QString id = QString::number(i);
        m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.set\", \"namespace\": \"") + id + QStringLiteral("\", \"data\": {\"temperature\": \"24°C\", \"otherproperty\": \"value\", \"state\": \"subdelegate2\"}}"));
    }

    QCOMPARE(m_view->activeNamespaces()->rowCount(), 10);
    for (int i = 0; i < 10; ++i) {
        QCOMPARE(m_view->activeNamespaces()->data(m_view->activeNamespaces()->index(9-i,0), NamespaceModel::NamespaceId).toString(), QString::number(i));
    }
}

void ServerTest::move100Skills()
{
    QSignalSpy skillMovedSpy(m_view->activeNamespaces(), &NamespaceModel::rowsMoved);

   // QTest::qWait(3000);
    const QUrl url(QStringLiteral("file://") + QFINDTESTDATA("delegatewithloader.qml"));

    for (int i = 0; i < 10; ++i) {

        m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.list.move\", \"namespace\": \"mycroft.system.active_skills\", \"from\": 9, \"to\": 0, \"items_number\": 1}"));

        skillMovedSpy.wait();
    }
    
}

void ServerTest::delete100Skills()
{
    QSignalSpy skillRemovedSpy(m_view->activeNamespaces(), &NamespaceModel::rowsRemoved);

   // QTest::qWait(3000);
    const QUrl url(QStringLiteral("file://") + QFINDTESTDATA("delegatewithloader.qml"));

    for (int i = 0; i < 10; ++i) {
        const QString id = QString::number(i);
        // Try to do a race condition between setting value and deleting
        m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.set\", \"namespace\": \"") + id + QStringLiteral("\", \"data\": {\"temperature\": \"25°C\", \"otherproperty\": \"value\", \"state\": \"subdelegate2\"}}"));
        
        m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.list.remove\", \"namespace\": \"mycroft.system.active_skills\", \"position\": 0, \"items_number\": 1}"));

        m_guiWebSocket->sendTextMessage(QStringLiteral("{\"type\": \"mycroft.session.set\", \"namespace\": \"") + id + QStringLiteral("\", \"data\": {\"temperature\": \"11°C\", \"otherproperty\": \"value\"}}"));

        skillRemovedSpy.wait();
    }
    
}

void ServerTest::repeat100()
{
    for (int i = 0; i < 10; ++i) {
        create100Skills();
        delete100Skills();
    }
}

QTEST_MAIN(ServerTest);

#include "stresstest.moc"
