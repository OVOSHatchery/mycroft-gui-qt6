/*
 * Copyright 2026 OpenVoiceOS Contributors
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

/*
 * End-to-end test for the GUI session/namespace wire protocol (v2 rework).
 *
 * Unlike servertest.cpp (which models the legacy two-socket flow with a
 * separate gui port), this exercises the reworked single-socket protocol that
 * the current GuiBusClient implements:
 *
 *   - GuiBusClient connects to ONE WebSocket server.
 *   - On connection it emits a "mycroft.gui.connected" handshake carrying the
 *     namespace gui_id plus session_id / site_id.
 *   - All wire-protocol messages arrive on that same socket and are fanned out
 *     by GuiBusClient::onMainSocketMessageReceived() to every registered
 *     GuiNamespace via handleIncomingMessage().
 *
 * The test stands up an in-process QWebSocketServer as the mock OVOS endpoint,
 * points the client at it via the MYCROFT_GUI_HOST/PORT environment overrides
 * honoured by GuiBusClient::buildWebSocketUrl(), and drives the protocol to
 * assert that NamespaceModel / PageModel / NamespaceDataMap update correctly.
 *
 * Covered:
 *   1. connect + handshake               testConnectAndHandshake()
 *   2. namespace created                 testNamespaceInserted()
 *   3. session-data propagated to map    testSessionDataPropagation()
 *   4. SYSTEM_* page populates PageModel testSystemPageShow()
 *   5. namespace teardown clears models  testNamespaceTeardown()
 *
 * The tests run in declaration order (Qt Test guarantees this) and share the
 * single GuiBusClient instance, so the connection established in step 1 is
 * reused by the later steps.
 */

#include <QtTest>
#include <QSignalSpy>
#include <QWebSocket>
#include <QWebSocketServer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QAbstractItemModelTester>

#include "../import/guibusclient.h"
#include "../import/guinamespace.h"
#include "../import/namespacemodel.h"
#include "../import/pagemodel.h"
#include "../import/namespacedatamap.h"

class ProtocolE2ETest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testConnectAndHandshake();
    void testNamespaceInserted();
    void testSessionDataPropagation();
    void testSystemPageShow();
    void testNamespaceTeardown();

private:
    // Helpers ---------------------------------------------------------------
    // Send a wire-protocol JSON message from the mock server to the client.
    void sendToClient(const QJsonObject &msg);
    NamespaceModel *namespaces() const { return m_namespace->activeNamespaces(); }

    // The namespace under test. Real skills use a reverse-dns style id.
    static constexpr const char *kSkill = "ovos.weather";

    // Mock server (the OVOS endpoint the client talks to).
    QWebSocketServer *m_server = nullptr;
    QWebSocket *m_serverConn = nullptr; // accepted client connection
    quint16 m_port = 0;

    // Client side.
    GuiBusClient *m_client = nullptr;
    GuiNamespace *m_namespace = nullptr;
};

void ProtocolE2ETest::sendToClient(const QJsonObject &msg)
{
    QVERIFY2(m_serverConn, "no accepted client connection to send through");
    m_serverConn->sendTextMessage(QString::fromUtf8(QJsonDocument(msg).toJson(QJsonDocument::Compact)));
}

void ProtocolE2ETest::initTestCase()
{
    // Stand up the mock OVOS WebSocket server on an ephemeral port.
    m_server = new QWebSocketServer(QStringLiteral("mock-ovos"),
                                    QWebSocketServer::NonSecureMode, this);
    QVERIFY2(m_server->listen(QHostAddress::LocalHost, 0),
             "mock websocket server failed to listen");
    m_port = m_server->serverPort();

    // Point the client at our mock server. buildWebSocketUrl() honours these
    // env overrides ahead of the persisted GlobalSettings values, so we never
    // touch the real user configuration.
    qputenv("MYCROFT_GUI_HOST", QByteArrayLiteral("127.0.0.1"));
    qputenv("MYCROFT_GUI_PORT", QByteArray::number(m_port));

    m_client = GuiBusClient::instance();
    QVERIFY(m_client);

    // A GuiNamespace registers itself with the singleton client on construction
    // and owns the activeNamespaces() model the protocol drives.
    m_namespace = new GuiNamespace;
    QVERIFY(m_namespace);
    QVERIFY(namespaces());

    // Validate the model against Qt's abstract-item-model contract throughout.
    new QAbstractItemModelTester(namespaces(),
                                 QAbstractItemModelTester::FailureReportingMode::QtTest,
                                 this);
}

void ProtocolE2ETest::cleanupTestCase()
{
    if (m_namespace) {
        delete m_namespace;
        m_namespace = nullptr;
    }
    if (m_client) {
        m_client->disconnectSocket();
    }
    qunsetenv("MYCROFT_GUI_HOST");
    qunsetenv("MYCROFT_GUI_PORT");
}

// ---------------------------------------------------------------------------
// 1. Connect + handshake
//
// start() opens the socket; once connected the client must emit a
// "mycroft.gui.connected" handshake carrying gui_id / session_id / site_id.
// ---------------------------------------------------------------------------
void ProtocolE2ETest::testConnectAndHandshake()
{
    QSignalSpy newConnectionSpy(m_server, &QWebSocketServer::newConnection);
    QSignalSpy statusSpy(m_client, &GuiBusClient::socketStatusChanged);

    m_client->start();

    QVERIFY2(newConnectionSpy.wait(5000), "mock server never accepted a connection");
    m_serverConn = m_server->nextPendingConnection();
    QVERIFY(m_serverConn);

    // The handshake arrives as the first text message on the accepted socket.
    QSignalSpy serverRxSpy(m_serverConn, &QWebSocket::textMessageReceived);

    // Client should reach the Open state.
    QTRY_COMPARE_WITH_TIMEOUT(m_client->status(), GuiBusClient::Open, 5000);

    QVERIFY2(serverRxSpy.count() > 0 || serverRxSpy.wait(5000),
             "no handshake message received from client");

    const QByteArray raw = serverRxSpy.first().first().toString().toUtf8();
    const QJsonDocument doc = QJsonDocument::fromJson(raw);
    QVERIFY2(!doc.isNull(), "handshake was not valid JSON");

    QCOMPARE(doc[QStringLiteral("type")].toString(), QStringLiteral("mycroft.gui.connected"));

    const QJsonObject data = doc[QStringLiteral("data")].toObject();
    QVERIFY2(!data.value(QStringLiteral("gui_id")).toString().isEmpty(),
             "handshake missing gui_id");
    // session_id / site_id default to "default" when not configured.
    QCOMPARE(data.value(QStringLiteral("session_id")).toString(), m_client->sessionId());
    QCOMPARE(data.value(QStringLiteral("site_id")).toString(), m_client->siteId());
}

// ---------------------------------------------------------------------------
// 2. Namespace created
//
// A session.list.insert on the special "mycroft.system.active_skills" list
// registers a new active namespace; the NamespaceModel grows by one row and
// exposes the namespace id via the NamespaceId role.
// ---------------------------------------------------------------------------
void ProtocolE2ETest::testNamespaceInserted()
{
    QCOMPARE(namespaces()->rowCount(), 0);

    QSignalSpy insertedSpy(namespaces(), &NamespaceModel::rowsInserted);

    QJsonObject msg{
        {QStringLiteral("type"), QStringLiteral("mycroft.session.list.insert")},
        {QStringLiteral("namespace"), QStringLiteral("mycroft.system.active_skills")},
        {QStringLiteral("position"), 0},
        {QStringLiteral("data"), QJsonArray{
            QJsonObject{{QStringLiteral("namespace_id"), QString::fromUtf8(kSkill)}}
        }},
    };
    sendToClient(msg);

    QVERIFY2(insertedSpy.wait(5000), "namespace was never inserted into the model");

    QCOMPARE(namespaces()->rowCount(), 1);
    QCOMPARE(namespaces()->data(namespaces()->index(0, 0), NamespaceModel::NamespaceId).toString(),
             QString::fromUtf8(kSkill));

    // A page model must exist for the freshly registered namespace.
    QVERIFY(namespaces()->pageModelForNamespace(QString::fromUtf8(kSkill)));
}

// ---------------------------------------------------------------------------
// 3. Session-data propagated to the data map
//
// session.set on an active namespace populates its NamespaceDataMap; each
// scalar key emits valueChanged and is readable back via value().
// ---------------------------------------------------------------------------
void ProtocolE2ETest::testSessionDataPropagation()
{
    NamespaceDataMap *map = m_namespace->namespaceDataForNamespace(QString::fromUtf8(kSkill));
    QVERIFY2(map, "no data map for the active namespace");

    // No data map should exist for an unknown namespace.
    QVERIFY(!m_namespace->namespaceDataForNamespace(QStringLiteral("does.not.exist")));

    QSignalSpy valueChangedSpy(map, &NamespaceDataMap::valueChanged);

    QJsonObject msg{
        {QStringLiteral("type"), QStringLiteral("mycroft.session.set")},
        {QStringLiteral("namespace"), QString::fromUtf8(kSkill)},
        {QStringLiteral("data"), QJsonObject{
            {QStringLiteral("temperature"), QStringLiteral("21C")},
            {QStringLiteral("icon"), QStringLiteral("weather-clear")},
        }},
    };
    sendToClient(msg);

    QVERIFY2(valueChangedSpy.wait(5000), "session data never propagated to the map");
    // Two scalar keys -> two valueChanged emissions.
    QTRY_COMPARE_WITH_TIMEOUT(valueChangedSpy.count(), 2, 5000);

    QCOMPARE(map->value(QStringLiteral("temperature")).toString(), QStringLiteral("21C"));
    QCOMPARE(map->value(QStringLiteral("icon")).toString(), QStringLiteral("weather-clear"));
}

// ---------------------------------------------------------------------------
// 4. SYSTEM_* page show populates the page model
//
// gui.list.insert with a "SYSTEM:<Template>.qml" url adds a page to the
// namespace's PageModel. The PageUrl role resolves the SYSTEM: scheme to a
// bundled qrc resource path (no QML engine load required to assert the row).
// ---------------------------------------------------------------------------
void ProtocolE2ETest::testSystemPageShow()
{
    PageModel *pages = namespaces()->pageModelForNamespace(QString::fromUtf8(kSkill));
    QVERIFY(pages);
    new QAbstractItemModelTester(pages,
                                 QAbstractItemModelTester::FailureReportingMode::QtTest,
                                 this);
    QCOMPARE(pages->rowCount(), 0);

    QSignalSpy pageInsertedSpy(pages, &PageModel::rowsInserted);

    QJsonObject msg{
        {QStringLiteral("type"), QStringLiteral("mycroft.gui.list.insert")},
        {QStringLiteral("namespace"), QString::fromUtf8(kSkill)},
        {QStringLiteral("position"), 0},
        {QStringLiteral("data"), QJsonArray{
            QJsonObject{{QStringLiteral("url"), QStringLiteral("SYSTEM:Status.qml")}}
        }},
    };
    sendToClient(msg);

    QVERIFY2(pageInsertedSpy.wait(5000), "SYSTEM page was never inserted into the page model");
    QCOMPARE(pages->rowCount(), 1);

    // SYSTEM: resolves to the bundled qrc template path. Assert via the PageUrl
    // role, which returns the stored loader url without forcing a QML load.
    const QUrl url = pages->data(pages->index(0, 0), PageModel::PageUrl).toUrl();
    QCOMPARE(url, QUrl(QStringLiteral("qrc:/system-templates/Status.qml")));
    QCOMPARE(pages->data(pages->index(0, 0), PageModel::NamespaceId).toString(),
             QString::fromUtf8(kSkill));
}

// ---------------------------------------------------------------------------
// 5. Namespace teardown clears models
//
// session.list.remove on the active_skills list removes the namespace, which
// drops the row from the NamespaceModel and disposes the associated page model
// and data map.
// ---------------------------------------------------------------------------
void ProtocolE2ETest::testNamespaceTeardown()
{
    QCOMPARE(namespaces()->rowCount(), 1);

    QSignalSpy removedSpy(namespaces(), &NamespaceModel::rowsRemoved);

    QJsonObject msg{
        {QStringLiteral("type"), QStringLiteral("mycroft.session.list.remove")},
        {QStringLiteral("namespace"), QStringLiteral("mycroft.system.active_skills")},
        {QStringLiteral("position"), 0},
        {QStringLiteral("items_number"), 1},
    };
    sendToClient(msg);

    QVERIFY2(removedSpy.wait(5000), "namespace was never removed from the model");

    QCOMPARE(namespaces()->rowCount(), 0);
    // The page model for the torn-down namespace must no longer be reachable.
    QVERIFY(!namespaces()->pageModelForNamespace(QString::fromUtf8(kSkill)));
}

QTEST_MAIN(ProtocolE2ETest)
#include "protocol_e2e_test.moc"
