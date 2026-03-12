/*
 * QML Framework Components Test
 *
 * Tests for framework components in import/qml/ directory.
 * Validates that updated components:
 * - Load without errors
 * - Have correct Qt/Kirigami versions
 * - Maintain backward compatibility
 */

#include <QtTest>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlContext>
#include <QObject>
#include <QString>
#include <QFile>

class QmlFrameworkComponentsTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    // Component loading tests
    void testDelegateLoads();
    void testScrollableDelegateLoads();
    void testProportionalDelegateLoads();
    void testAutoFitLabelLoads();
    void testBoxLayoutLoads();
    void testCardDelegateLoads();
    void testMarqueeTextLoads();
    void testPaginatedTextLoads();
    void testSlideShowLoads();
    void testSlidingImageLoads();
    void testStatusIndicatorLoads();

    // Version checks
    void testAllComponentsUseQt212OrLater();
    void testDeprecationNotices();

private:
    QQmlEngine engine;

    bool componentExists(const QString &componentPath);
    bool componentLoads(const QString &componentPath, QString &errorMessage);
};

/*
 * Helper: Check if QML file exists
 */
bool QmlFrameworkComponentsTest::componentExists(const QString &componentPath)
{
    QFile file(componentPath);
    return file.exists();
}

/*
 * Helper: Attempt to load a QML component and capture errors
 */
bool QmlFrameworkComponentsTest::componentLoads(const QString &componentPath, QString &errorMessage)
{
    if (!componentExists(componentPath)) {
        errorMessage = "File not found: " + componentPath;
        return false;
    }

    QQmlComponent component(&engine, QUrl::fromLocalFile(componentPath));

    if (component.isError()) {
        QStringList errors;
        for (const auto &error : component.errors()) {
            errors << error.toString();
        }
        errorMessage = errors.join("; ");
        return false;
    }

    return true;
}

/*
 * Test: Delegate component loads
 */
void QmlFrameworkComponentsTest::testDelegateLoads()
{
    QString error;
    const QString path = ":/qml/Delegate.qml";

    QVERIFY2(componentLoads(path, error), qPrintable("Delegate failed to load: " + error));
}

/*
 * Test: ScrollableDelegate component loads
 */
void QmlFrameworkComponentsTest::testScrollableDelegateLoads()
{
    QString error;
    const QString path = ":/qml/ScrollableDelegate.qml";

    QVERIFY2(componentLoads(path, error), qPrintable("ScrollableDelegate failed to load: " + error));
}

/*
 * Test: ProportionalDelegate component loads
 */
void QmlFrameworkComponentsTest::testProportionalDelegateLoads()
{
    QString error;
    const QString path = ":/qml/ProportionalDelegate.qml";

    QVERIFY2(componentLoads(path, error), qPrintable("ProportionalDelegate failed to load: " + error));
}

/*
 * Test: AutoFitLabel component loads
 */
void QmlFrameworkComponentsTest::testAutoFitLabelLoads()
{
    QString error;
    const QString path = ":/qml/AutoFitLabel.qml";

    QVERIFY2(componentLoads(path, error), qPrintable("AutoFitLabel failed to load: " + error));
}

/*
 * Test: BoxLayout component loads
 */
void QmlFrameworkComponentsTest::testBoxLayoutLoads()
{
    QString error;
    const QString path = ":/qml/BoxLayout.qml";

    QVERIFY2(componentLoads(path, error), qPrintable("BoxLayout failed to load: " + error));
}

/*
 * Test: CardDelegate component loads
 */
void QmlFrameworkComponentsTest::testCardDelegateLoads()
{
    QString error;
    const QString path = ":/qml/CardDelegate.qml";

    QVERIFY2(componentLoads(path, error), qPrintable("CardDelegate failed to load: " + error));
}

/*
 * Test: MarqueeText component loads
 */
void QmlFrameworkComponentsTest::testMarqueeTextLoads()
{
    QString error;
    const QString path = ":/qml/MarqueeText.qml";

    QVERIFY2(componentLoads(path, error), qPrintable("MarqueeText failed to load: " + error));
}

/*
 * Test: PaginatedText component loads
 */
void QmlFrameworkComponentsTest::testPaginatedTextLoads()
{
    QString error;
    const QString path = ":/qml/PaginatedText.qml";

    QVERIFY2(componentLoads(path, error), qPrintable("PaginatedText failed to load: " + error));
}

/*
 * Test: SlideShow component loads
 */
void QmlFrameworkComponentsTest::testSlideShowLoads()
{
    QString error;
    const QString path = ":/qml/SlideShow.qml";

    QVERIFY2(componentLoads(path, error), qPrintable("SlideShow failed to load: " + error));
}

/*
 * Test: SlidingImage component loads
 */
void QmlFrameworkComponentsTest::testSlidingImageLoads()
{
    QString error;
    const QString path = ":/qml/SlidingImage.qml";

    QVERIFY2(componentLoads(path, error), qPrintable("SlidingImage failed to load: " + error));
}

/*
 * Test: StatusIndicator component loads
 */
void QmlFrameworkComponentsTest::testStatusIndicatorLoads()
{
    QString error;
    const QString path = ":/qml/StatusIndicator.qml";

    QVERIFY2(componentLoads(path, error), qPrintable("StatusIndicator failed to load: " + error));
}

/*
 * Test: All components use Qt 2.12 or later
 *
 * Verifies that after modernization, no components use outdated Qt versions.
 */
void QmlFrameworkComponentsTest::testAllComponentsUseQt212OrLater()
{
    // List of files that should be updated
    QStringList componentFiles = {
        "Delegate.qml",
        "ScrollableDelegate.qml",
        "ProportionalDelegate.qml",
        "AutoFitLabel.qml",
        "BoxLayout.qml",
        "CardDelegate.qml",
        "MarqueeText.qml",
        "PaginatedText.qml",
        "SlideShow.qml",
        "SlidingImage.qml",
        "SoundEffects.qml",
        "StatusIndicator.qml",
        "Units.qml",
        "SkillView.qml"
    };

    // Check each file for old Qt versions
    QString qmlDir = ":/qml/";
    for (const auto &file : componentFiles) {
        QFile qmlFile(qmlDir + file);
        QVERIFY2(qmlFile.open(QIODevice::ReadOnly), qPrintable("Cannot open " + file));

        QString content = QString::fromUtf8(qmlFile.readAll());
        qmlFile.close();

        // Should use Qt 2.12 or explicitly state it's deprecated
        bool hasModernQt = content.contains("import QtQuick 2.12") ||
                          content.contains("import QtQuick 2.13") ||
                          content.contains("import QtQuick 2.14") ||
                          content.contains("import QtQuick 2.15");

        bool isDeprecated = content.contains("DEPRECATED");

        QVERIFY2(hasModernQt || isDeprecated,
                qPrintable(file + " must use Qt 2.12+ or be marked DEPRECATED"));
    }
}

/*
 * Test: Deprecated components have proper notices
 */
void QmlFrameworkComponentsTest::testDeprecationNotices()
{
    QStringList deprecatedComponents = {
        "Delegate.qml",
        "ScrollableDelegate.qml",
        "ProportionalDelegate.qml",
        "AudioPlayer.qml",
        "VideoPlayer.qml"
    };

    QString qmlDir = ":/qml/";
    for (const auto &file : deprecatedComponents) {
        QFile qmlFile(qmlDir + file);
        QVERIFY2(qmlFile.open(QIODevice::ReadOnly), qPrintable("Cannot open " + file));

        QString content = QString::fromUtf8(qmlFile.readAll());
        qmlFile.close();

        // Deprecated components should have clear notice
        bool hasDeprecationNotice = content.contains("DEPRECATED") ||
                                    content.contains("deprecated") ||
                                    content.contains("template-based system");

        QVERIFY2(hasDeprecationNotice,
                qPrintable(file + " should have deprecation notice"));
    }
}

QTEST_APPLESS_MAIN(QmlFrameworkComponentsTest)
#include "qml_framework_components_test.moc"
