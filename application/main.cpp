#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QCommandLineParser>
#include <QQuickWindow>
#include <QDebug>
#include <QDir>

#include "../import/guibusclient.h"
#include "../import/globalsettings.h"
#include "../import/filereader.h"
#include "../import/mediaservice.h"
#include "../import/guipage.h"
#include "../import/guinamespace.h"
#include "../import/namespacemodel.h"
#include "../import/pagemodel.h"
#include "../import/namespacedatamap.h"
#include "appsettings.h"
#include "version.h"

int main(int argc, char *argv[])
{
    QCommandLineParser parser;
    auto widthOption = QCommandLineOption(QStringLiteral("width"), "Fixed width for the window", "pixels", "800");
    auto heightOption = QCommandLineOption(QStringLiteral("height"), "Fixed height for the window", "pixels", "480");
    auto maximizedOption = QCommandLineOption(QStringLiteral("maximized"), "Start the window maximized");
    auto rotationOption = QCommandLineOption(QStringLiteral("rotation"), "Screen rotation (0, 90, 180, 270)", "degrees", "0");
    auto shellModeOption = QCommandLineOption(QStringLiteral("shell"), "Run in full shell mode (with taskbar/panels)");
    auto singleNamespaceOption = QCommandLineOption(QStringLiteral("namespace"), "Only show a specific namespace", "namespace_id");
    auto singleNamespaceHomeOption = QCommandLineOption(QStringLiteral("namespace-home"), "Automatically navigate to this namespace on connect", "request_type");
    
    parser.addOption(widthOption);
    parser.addOption(heightOption);
    parser.addOption(maximizedOption);
    parser.addOption(rotationOption);
    parser.addOption(shellModeOption);
    parser.addOption(singleNamespaceOption);
    parser.addOption(singleNamespaceHomeOption);
    parser.addHelpOption();
    parser.addVersionOption();

    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGLRhi);
    QApplication app(argc, argv);
    parser.process(app);

    QQmlApplicationEngine engine;

    const char *uri = "OVOS.GUI";
    qmlRegisterSingletonType<GuiBusClient>(uri, 1, 0, "GuiBusClient",
        [](QQmlEngine *engine, QJSEngine *) -> QObject * {
            engine->setObjectOwnership(GuiBusClient::instance(), QQmlEngine::CppOwnership);
            return GuiBusClient::instance();
        });
    qmlRegisterSingletonType<GlobalSettings>(uri, 1, 0, "GlobalSettings",
        [](QQmlEngine *, QJSEngine *) -> QObject * {
            return new GlobalSettings;
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

    engine.rootContext()->setContextProperty(QStringLiteral("deviceWidth"), parser.value(widthOption).toInt());
    engine.rootContext()->setContextProperty(QStringLiteral("deviceHeight"), parser.value(heightOption).toInt());
    engine.rootContext()->setContextProperty(QStringLiteral("deviceMaximized"), parser.isSet(maximizedOption));
    engine.rootContext()->setContextProperty(QStringLiteral("globalScreenRotation"), parser.value(rotationOption).toInt());
    engine.rootContext()->setContextProperty(QStringLiteral("shellMode"), parser.isSet(shellModeOption));
    engine.rootContext()->setContextProperty(QStringLiteral("singleNamespace"), parser.value(singleNamespaceOption));
    engine.rootContext()->setContextProperty(QStringLiteral("singleNamespaceHome"), parser.value(singleNamespaceHomeOption));

    AppSettings *appSettings = new AppSettings();
    engine.rootContext()->setContextProperty(QStringLiteral("applicationSettings"), appSettings);

    if (parser.isSet(shellModeOption)) {
        qDebug() << "Loading in SHELL mode";
        engine.load(QUrl(QStringLiteral("qrc:/shell/main.qml")));
    } else {
        qDebug() << "Loading in SKILL mode";
        engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    }

    return app.exec();
}
