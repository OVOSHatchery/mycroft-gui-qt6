#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileInfo>

#include "../import/guibusclient.h"
#include "../import/globalsettings.h"
#include "../import/filereader.h"
#include "../import/mediaservice.h"
#include "../import/guipage.h"
#include "../import/guinamespace.h"
#include "../import/namespacemodel.h"
#include "../import/pagemodel.h"
#include "../import/namespacedatamap.h"
#include "../application/appsettings.h"
#include "../application/shell/plugins/EnvironmentSummary.h"
#include "../application/shell/plugins/ResetOperations.h"
#include "demoutils.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Register types for "import OVOS.GUI 1.0"
    const char *uri = "OVOS.GUI";
    qmlRegisterSingletonType<GuiBusClient>(uri, 1, 0, "GuiBusClient",
        [](QQmlEngine *engine, QJSEngine *) -> QObject * {
            engine->setObjectOwnership(GuiBusClient::instance(), QQmlEngine::CppOwnership);
            return GuiBusClient::instance();
        });
    
    GlobalSettings *globalSettings = new GlobalSettings;
    globalSettings->setAutoConnect(false);
    qmlRegisterSingletonType<GlobalSettings>(uri, 1, 0, "GlobalSettings",
        [globalSettings](QQmlEngine *, QJSEngine *) -> QObject * {
            return globalSettings;
        });

    qmlRegisterSingletonType<FileReader>(uri, 1, 0, "FileReader",
        [](QQmlEngine *, QJSEngine *) -> QObject * {
            return new FileReader;
        });
    qmlRegisterSingletonType<MediaService>(uri, 1, 0, "MediaService",
        [](QQmlEngine *, QJSEngine *) -> QObject * {
            return new MediaService;
        });

    qmlRegisterType<GuiNamespace>(uri, 1, 0, "GuiNamespace");
    qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/NamespaceView.qml")), uri, 1, 0, "NamespaceView");
    qmlRegisterType<GuiPage>(uri, 1, 0, "Page");
    
    // Manual registration of QML components
    qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/AudioPlayer.qml")), uri, 1, 0, "AudioPlayer");
    qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/AutoFitLabel.qml")), uri, 1, 0, "AutoFitLabel");
    qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/BoxLayout.qml")), uri, 1, 0, "BoxLayout");
    qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/BusyIndicator.qml")), uri, 1, 0, "BusyIndicator");
    qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/CardDelegate.qml")), uri, 1, 0, "CardDelegate");
    qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/MarqueeText.qml")), uri, 1, 0, "MarqueeText");
    qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/PaginatedText.qml")), uri, 1, 0, "PaginatedText");
    qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/ProportionalDelegate.qml")), uri, 1, 0, "ProportionalDelegate");
    qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/ScrollableDelegate.qml")), uri, 1, 0, "ScrollableDelegate");
    qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/SlideShow.qml")), uri, 1, 0, "SlideShow");
    qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/SlidingImage.qml")), uri, 1, 0, "SlidingImage");
    qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/SoundEffects.qml")), uri, 1, 0, "SoundEffects");
    qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/StatusIndicator.qml")), uri, 1, 0, "StatusIndicator");
    qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/Units.qml")), uri, 1, 0, "Units");
    qmlRegisterType(QUrl(QStringLiteral("qrc:/qml/VideoPlayer.qml")), uri, 1, 0, "VideoPlayer");

    qmlRegisterUncreatableType<NamespaceModel>(uri, 1, 0, "NamespaceModel",
        QStringLiteral("Access via GuiBusClient.activeNamespaces"));
    qmlRegisterUncreatableType<NamespaceDataMap>(uri, 1, 0, "NamespaceDataMap",
        QStringLiteral("Access via namespace pages"));
    qmlRegisterUncreatableType<PageModel>(uri, 1, 0, "PageModel",
        QStringLiteral("Access via GuiNamespace"));

    QQmlApplicationEngine engine;
    
    // Mock context properties
    engine.rootContext()->setContextProperty(QStringLiteral("applicationSettings"), new AppSettings(nullptr));
    engine.rootContext()->setContextProperty(QStringLiteral("environmentSummary"), new EnvironmentSummary(nullptr));
    engine.rootContext()->setContextProperty(QStringLiteral("resetOperations"), new ResetOperations(nullptr));
    engine.rootContext()->setContextProperty(QStringLiteral("deviceWidth"), 800);
    engine.rootContext()->setContextProperty(QStringLiteral("deviceHeight"), 480);
    engine.rootContext()->setContextProperty(QStringLiteral("deviceMaximized"), false);
    engine.rootContext()->setContextProperty(QStringLiteral("globalScreenRotation"), 0);
    engine.rootContext()->setContextProperty(QStringLiteral("singleSkillHome"), QString());
    engine.rootContext()->setContextProperty(QStringLiteral("singleSkill"), QString());
    engine.rootContext()->setContextProperty(QStringLiteral("hideTextInput"), false);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    GuiBusClient *client = GuiBusClient::instance();

    // Utility to inject messages
    auto inject = [&](const QString &type, const QVariantMap &fields) {
        QJsonObject root;
        root[QStringLiteral("type")] = type;
        for (auto it = fields.constBegin(); it != fields.constEnd(); ++it) {
            root[it.key()] = QJsonValue::fromVariant(it.value());
        }
        QJsonObject context;
        QJsonObject session;
        session[QStringLiteral("session_id")] = QStringLiteral("default");
        context[QStringLiteral("session")] = session;
        root[QStringLiteral("context")] = context;
        client->onMainSocketMessageReceived(QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Compact)));
    };

    QStringList templates = DemoUtils::allTemplates();
    int currentIdx = 0;

    QTimer *timer = new QTimer(&app);
    QObject::connect(timer, &QTimer::timeout, [&]() {
        if (currentIdx >= templates.size()) {
            qDebug() << "[TEST] All templates processed. Exiting.";
            app.quit();
            return;
        }

        QString tpl = templates.at(currentIdx);
        qDebug() << "[TEST] Testing template:" << tpl;

        // 1. Insert namespace
        QVariantMap namespaceInfo;
        namespaceInfo[QStringLiteral("namespace_id")] = DemoUtils::kDemoNamespace;
        inject(QStringLiteral("mycroft.session.list.insert"),
                    QVariantMap({{QStringLiteral("namespace"), QStringLiteral("mycroft.system.active_skills")},
                                 {QStringLiteral("position"), 0},
                                 {QStringLiteral("data"), QVariantList({namespaceInfo})}}));

        // 2. Set namespace data
        QJsonObject jsonData = DemoUtils::demoNamespaceData(tpl);
        QVariantMap namespaceData;
        for (auto it = jsonData.constBegin(); it != jsonData.constEnd(); ++it) {
            namespaceData.insert(it.key(), it.value().toVariant());
        }
        inject(QStringLiteral("mycroft.session.set"),
                    QVariantMap({{QStringLiteral("namespace"), DemoUtils::kDemoNamespace},
                                 {QStringLiteral("data"), namespaceData}}));

        // 3. Insert page
        QVariantList pages;
        pages.append(QVariantMap({{QStringLiteral("url"), QStringLiteral("SYSTEM:") + tpl + QStringLiteral(".qml")},
                                  {QStringLiteral("page"), tpl + QStringLiteral(".qml")}}));
        inject(QStringLiteral("mycroft.gui.list.insert"),
                    QVariantMap({{QStringLiteral("namespace"), DemoUtils::kDemoNamespace},
                                 {QStringLiteral("position"), 0},
                                 {QStringLiteral("data"), pages}}));

        // 4. Focus page
        inject(QStringLiteral("mycroft.events.triggered"),
                    QVariantMap({{QStringLiteral("namespace"), DemoUtils::kDemoNamespace},
                                 {QStringLiteral("event_name"), QStringLiteral("page_gained_focus")},
                                 {QStringLiteral("data"), QVariantMap({{QStringLiteral("number"), 0}})}}));

        currentIdx++;
    });

    qDebug() << "[TEST] Starting Automated Demo Test";
    timer->start(2000); // 2 seconds per template

    return app.exec();
}
