/*
 * Message Routing Tests
 *
 * Validates the three-layer message classification:
 * 1. WireMessage — classic mycroft GUI wire protocol
 * 2. ShellEvent — gui.* shell feature extensions
 * 3. AssistantEvent — forwarded OVOS bus events
 */

#include <QtTest>
#include <QString>
#include "../import/guibusmessages.h"

class MessageRoutingTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testWireMessages();
    void testShellEvents();
    void testAssistantEvents();
    void testParseMessage_wire();
    void testParseMessage_shell();
    void testParseMessage_assistant();
    void testParseMessage_unknown();
    void testRoundTrip_wire();
    void testRoundTrip_assistant();
    void testStopHandledAlias();
};

void MessageRoutingTest::testWireMessages()
{
    // All wire protocol messages should parse correctly
    QStringList wireMessages = {
        "mycroft.gui.connected",
        "mycroft.session.set",
        "mycroft.session.delete",
        "mycroft.session.list.insert",
        "mycroft.session.list.remove",
        "mycroft.session.list.move",
        "mycroft.session.list.update",
        "mycroft.gui.list.insert",
        "mycroft.gui.list.remove",
        "mycroft.gui.list.move",
        "mycroft.events.triggered",
        "gui.clear.namespace",
    };

    for (const auto &msg : wireMessages) {
        auto result = GuiBusMessages::wireFromString(msg);
        QVERIFY2(result != static_cast<GuiBusMessages::WireMessage>(-1),
                 qPrintable("Wire message not recognized: " + msg));
    }
    QCOMPARE(wireMessages.count(), 12);
}

void MessageRoutingTest::testShellEvents()
{
    QStringList shellMessages = {
        "gui.brightness.set",
        "gui.brightness.get",
        "gui.brightness.get.response",
        "gui.brightness.auto_dim.set",
        "gui.brightness.night_mode.set",
        "gui.color_scheme.set",
        "gui.color_scheme.get",
        "gui.color_scheme.get.response",
        "gui.notification.set",
        "gui.notification.clear",
        "gui.widget.display",
        "gui.widget.remove",
        "gui.config.list.get",
        "gui.config.list.get.response",
        "gui.config.get",
        "gui.config.get.response",
        "gui.config.set",
        "gui.hints.get",
        "gui.hints.get.response",
    };

    for (const auto &msg : shellMessages) {
        auto result = GuiBusMessages::shellFromString(msg);
        QVERIFY2(result != static_cast<GuiBusMessages::ShellEvent>(-1),
                 qPrintable("Shell event not recognized: " + msg));
    }
    QCOMPARE(shellMessages.count(), 19);
}

void MessageRoutingTest::testAssistantEvents()
{
    QStringList assistantMessages = {
        "recognizer_loop:audio_output_start",
        "recognizer_loop:audio_output_end",
        "speak",
        "recognizer_loop:wakeword",
        "recognizer_loop:record_begin",
        "recognizer_loop:record_end",
        "recognizer_loop:recognition_unknown",
        "recognizer_loop:sleep",
        "recognizer_loop:wake_up",
        "mycroft.awoken",
        "recognizer_loop:utterance",
        "mycroft.stop.handled",
        "mycroft.stop",
        "complete_intent_failure",
        "ovos.utterance.handled",
        "ovos.utterance.cancelled",
        "mycroft.skill.handler.start",
        "mycroft.skill.handler.complete",
        "mycroft.skills.all_loaded.response",
        "mycroft.ready",
        "screen.close.idle.event",
        "enclosure.eyes.on",
        "enclosure.mouth.talk",
        "enclosure.weather.display",
    };

    for (const auto &msg : assistantMessages) {
        auto result = GuiBusMessages::assistantFromString(msg);
        QVERIFY2(result != static_cast<GuiBusMessages::AssistantEvent>(-1),
                 qPrintable("Assistant event not recognized: " + msg));
    }
}

void MessageRoutingTest::testParseMessage_wire()
{
    auto parsed = GuiBusMessages::parseMessage("mycroft.session.set");
    QCOMPARE(parsed.layer, GuiBusMessages::MessageLayer::WIRE);
    QCOMPARE(parsed.wire, GuiBusMessages::WireMessage::SESSION_SET);
}

void MessageRoutingTest::testParseMessage_shell()
{
    auto parsed = GuiBusMessages::parseMessage("gui.brightness.set");
    QCOMPARE(parsed.layer, GuiBusMessages::MessageLayer::SHELL);
    QCOMPARE(parsed.shell, GuiBusMessages::ShellEvent::BRIGHTNESS_SET);
}

void MessageRoutingTest::testParseMessage_assistant()
{
    auto parsed = GuiBusMessages::parseMessage("recognizer_loop:wakeword");
    QCOMPARE(parsed.layer, GuiBusMessages::MessageLayer::ASSISTANT);
    QCOMPARE(parsed.assistant, GuiBusMessages::AssistantEvent::RECOGNIZER_WAKEWORD);
}

void MessageRoutingTest::testParseMessage_unknown()
{
    auto parsed = GuiBusMessages::parseMessage("unknown.message.type");
    QCOMPARE(parsed.layer, GuiBusMessages::MessageLayer::UNKNOWN);
}

void MessageRoutingTest::testRoundTrip_wire()
{
    auto wire = GuiBusMessages::wireFromString("mycroft.gui.list.insert");
    QCOMPARE(QString(GuiBusMessages::toString(wire)), "mycroft.gui.list.insert");
}

void MessageRoutingTest::testRoundTrip_assistant()
{
    auto evt = GuiBusMessages::assistantFromString("recognizer_loop:wakeword");
    QCOMPARE(QString(GuiBusMessages::toString(evt)), "recognizer_loop:wakeword");
}

void MessageRoutingTest::testStopHandledAlias()
{
    // Both "mycroft.stop.handled" and "mycroft.stop" should map to STOP_HANDLED
    auto a = GuiBusMessages::assistantFromString("mycroft.stop.handled");
    auto b = GuiBusMessages::assistantFromString("mycroft.stop");
    QCOMPARE(a, GuiBusMessages::AssistantEvent::STOP_HANDLED);
    QCOMPARE(b, GuiBusMessages::AssistantEvent::STOP_HANDLED);
}

QTEST_APPLESS_MAIN(MessageRoutingTest)
#include "message_routing_test.moc"
