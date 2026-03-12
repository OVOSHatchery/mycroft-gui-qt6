/*
 * Message Routing Tests
 *
 * These tests validate the enum-based message routing refactoring.
 * They verify that:
 * 1. All 23 OVOS bus messages are recognized
 * 2. Message type strings convert to correct enum values
 * 3. Unknown messages return invalid enum value
 * 4. Enum values can be converted back to strings
 * 5. Message categories are correctly assigned
 */

#include <QtTest>
#include <QString>
#include "../import/guibusmessages.h"

class MessageRoutingTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testEnumConversion_data();
    void testEnumConversion();
    void testToString_data();
    void testToString();
    void testUnknownMessage();
    void testMessageCategories_data();
    void testMessageCategories();
    void testAllMessagesRecognized();
};

/*
 * Test: enum conversion from string
 * Validates that all 23 message types convert to correct enum values
 */
void MessageRoutingTest::testEnumConversion_data()
{
    QTest::addColumn<QString>("messageType");
    QTest::addColumn<GuiBusMessages::GUIBusMessageType>("expectedEnum");

    // Initialization
    QTest::newRow("gui_connected")
        << "mycroft.gui.connected"
        << GuiBusMessages::GUIBusMessageType::GUI_CONNECTED;

    // Page rendering
    QTest::newRow("gui_list_insert")
        << "mycroft.gui.list.insert"
        << GuiBusMessages::GUIBusMessageType::GUI_LIST_INSERT;
    QTest::newRow("gui_list_remove")
        << "mycroft.gui.list.remove"
        << GuiBusMessages::GUIBusMessageType::GUI_LIST_REMOVE;
    QTest::newRow("gui_list_move")
        << "mycroft.gui.list.move"
        << GuiBusMessages::GUIBusMessageType::GUI_LIST_MOVE;

    // Session data
    QTest::newRow("session_set")
        << "mycroft.session.set"
        << GuiBusMessages::GUIBusMessageType::SESSION_SET;
    QTest::newRow("session_delete")
        << "mycroft.session.delete"
        << GuiBusMessages::GUIBusMessageType::SESSION_DELETE;
    QTest::newRow("session_list_insert")
        << "mycroft.session.list.insert"
        << GuiBusMessages::GUIBusMessageType::SESSION_LIST_INSERT;
    QTest::newRow("session_list_remove")
        << "mycroft.session.list.remove"
        << GuiBusMessages::GUIBusMessageType::SESSION_LIST_REMOVE;
    QTest::newRow("session_list_move")
        << "mycroft.session.list.move"
        << GuiBusMessages::GUIBusMessageType::SESSION_LIST_MOVE;
    QTest::newRow("session_list_update")
        << "mycroft.session.list.update"
        << GuiBusMessages::GUIBusMessageType::SESSION_LIST_UPDATE;

    // Namespace lifecycle
    QTest::newRow("clear_namespace")
        << "gui.clear.namespace"
        << GuiBusMessages::GUIBusMessageType::CLEAR_NAMESPACE;

    // Audio/speech states
    QTest::newRow("audio_output_start")
        << "recognizer_loop:audio_output_start"
        << GuiBusMessages::GUIBusMessageType::RECOGNIZER_AUDIO_OUTPUT_START;
    QTest::newRow("audio_output_end")
        << "recognizer_loop:audio_output_end"
        << GuiBusMessages::GUIBusMessageType::RECOGNIZER_AUDIO_OUTPUT_END;
    QTest::newRow("wakeword")
        << "recognizer_loop:wakeword"
        << GuiBusMessages::GUIBusMessageType::RECOGNIZER_WAKEWORD;
    QTest::newRow("record_begin")
        << "recognizer_loop:record_begin"
        << GuiBusMessages::GUIBusMessageType::RECOGNIZER_RECORD_BEGIN;
    QTest::newRow("record_end")
        << "recognizer_loop:record_end"
        << GuiBusMessages::GUIBusMessageType::RECOGNIZER_RECORD_END;
    QTest::newRow("speech_recognition_unknown")
        << "mycroft.speech.recognition.unknown"
        << GuiBusMessages::GUIBusMessageType::SPEECH_RECOGNITION_UNKNOWN;

    // Lifecycle states
    QTest::newRow("stop_handled")
        << "mycroft.stop.handled"
        << GuiBusMessages::GUIBusMessageType::STOP_HANDLED;
    QTest::newRow("intent_failure")
        << "complete_intent_failure"
        << GuiBusMessages::GUIBusMessageType::INTENT_FAILURE;
    QTest::newRow("skills_loaded_response")
        << "mycroft.skills.all_loaded.response"
        << GuiBusMessages::GUIBusMessageType::SKILLS_LOADED_RESPONSE;
    QTest::newRow("ready")
        << "mycroft.ready"
        << GuiBusMessages::GUIBusMessageType::READY;

    // Screen/homescreen
    QTest::newRow("screen_close_idle_event")
        << "screen.close.idle.event"
        << GuiBusMessages::GUIBusMessageType::SCREEN_CLOSE_IDLE_EVENT;

    // User interaction
    QTest::newRow("events_triggered")
        << "mycroft.events.triggered"
        << GuiBusMessages::GUIBusMessageType::EVENTS_TRIGGERED;
    QTest::newRow("recognizer_utterance")
        << "recognizer_loop:utterance"
        << GuiBusMessages::GUIBusMessageType::RECOGNIZER_UTTERANCE;
}

void MessageRoutingTest::testEnumConversion()
{
    QFETCH(QString, messageType);
    QFETCH(GuiBusMessages::GUIBusMessageType, expectedEnum);

    auto result = GuiBusMessages::fromString(messageType);
    QCOMPARE(result, expectedEnum);
}

/*
 * Test: enum to string conversion
 * Validates round-trip conversion works correctly
 */
void MessageRoutingTest::testToString_data()
{
    QTest::addColumn<GuiBusMessages::GUIBusMessageType>("enumValue");
    QTest::addColumn<QString>("expectedString");

    QTest::newRow("GUI_CONNECTED")
        << GuiBusMessages::GUIBusMessageType::GUI_CONNECTED
        << "mycroft.gui.connected";
    QTest::newRow("GUI_LIST_INSERT")
        << GuiBusMessages::GUIBusMessageType::GUI_LIST_INSERT
        << "mycroft.gui.list.insert";
    QTest::newRow("SESSION_SET")
        << GuiBusMessages::GUIBusMessageType::SESSION_SET
        << "mycroft.session.set";
    QTest::newRow("RECOGNIZER_WAKEWORD")
        << GuiBusMessages::GUIBusMessageType::RECOGNIZER_WAKEWORD
        << "recognizer_loop:wakeword";
}

void MessageRoutingTest::testToString()
{
    QFETCH(GuiBusMessages::GUIBusMessageType, enumValue);
    QFETCH(QString, expectedString);

    const char* result = GuiBusMessages::toString(enumValue);
    QCOMPARE(QString(result), expectedString);
}

/*
 * Test: unknown message handling
 * Validates that unknown messages return invalid enum value (-1)
 */
void MessageRoutingTest::testUnknownMessage()
{
    auto result = GuiBusMessages::fromString("unknown.message.type");
    QCOMPARE(result, static_cast<GuiBusMessages::GUIBusMessageType>(-1));

    // Verify toString() of invalid enum returns "UNKNOWN"
    const char* str = GuiBusMessages::toString(static_cast<GuiBusMessages::GUIBusMessageType>(-1));
    QCOMPARE(QString(str), "UNKNOWN");
}

/*
 * Test: message categories
 * Validates that each message is correctly categorized
 */
void MessageRoutingTest::testMessageCategories_data()
{
    QTest::addColumn<GuiBusMessages::GUIBusMessageType>("messageType");
    QTest::addColumn<GuiBusMessages::GUIBusMessageCategory>("expectedCategory");

    // Initialization
    QTest::newRow("init")
        << GuiBusMessages::GUIBusMessageType::GUI_CONNECTED
        << GuiBusMessages::GUIBusMessageCategory::INIT;

    // Page rendering
    QTest::newRow("page_render_1")
        << GuiBusMessages::GUIBusMessageType::GUI_LIST_INSERT
        << GuiBusMessages::GUIBusMessageCategory::PAGE_RENDERING;
    QTest::newRow("page_render_2")
        << GuiBusMessages::GUIBusMessageType::GUI_LIST_REMOVE
        << GuiBusMessages::GUIBusMessageCategory::PAGE_RENDERING;

    // Session data
    QTest::newRow("session_data_1")
        << GuiBusMessages::GUIBusMessageType::SESSION_SET
        << GuiBusMessages::GUIBusMessageCategory::SESSION_DATA;
    QTest::newRow("session_data_2")
        << GuiBusMessages::GUIBusMessageType::CLEAR_NAMESPACE
        << GuiBusMessages::GUIBusMessageCategory::SESSION_DATA;

    // State changes
    QTest::newRow("state_change_1")
        << GuiBusMessages::GUIBusMessageType::RECOGNIZER_WAKEWORD
        << GuiBusMessages::GUIBusMessageCategory::STATE_CHANGE;
    QTest::newRow("state_change_2")
        << GuiBusMessages::GUIBusMessageType::READY
        << GuiBusMessages::GUIBusMessageCategory::STATE_CHANGE;

    // User interaction
    QTest::newRow("user_interaction")
        << GuiBusMessages::GUIBusMessageType::EVENTS_TRIGGERED
        << GuiBusMessages::GUIBusMessageCategory::USER_INTERACTION;
}

void MessageRoutingTest::testMessageCategories()
{
    QFETCH(GuiBusMessages::GUIBusMessageType, messageType);
    QFETCH(GuiBusMessages::GUIBusMessageCategory, expectedCategory);

    auto category = GuiBusMessages::getCategory(messageType);
    QCOMPARE(category, expectedCategory);
}

/*
 * Test: all 23 messages are recognized
 * Validates that we handle exactly the expected number of message types
 */
void MessageRoutingTest::testAllMessagesRecognized()
{
    // List of all 23 expected message types
    QStringList allMessages = {
        // Initialization (1)
        "mycroft.gui.connected",
        // Page rendering (3)
        "mycroft.gui.list.insert",
        "mycroft.gui.list.remove",
        "mycroft.gui.list.move",
        // Session data (8)
        "mycroft.session.set",
        "mycroft.session.delete",
        "mycroft.session.list.insert",
        "mycroft.session.list.remove",
        "mycroft.session.list.move",
        "mycroft.session.list.update",
        "gui.clear.namespace",
        // Audio/speech states (6)
        "recognizer_loop:audio_output_start",
        "recognizer_loop:audio_output_end",
        "recognizer_loop:wakeword",
        "recognizer_loop:record_begin",
        "recognizer_loop:record_end",
        "mycroft.speech.recognition.unknown",
        // Lifecycle states (4)
        "mycroft.stop.handled",
        "complete_intent_failure",
        "mycroft.skills.all_loaded.response",
        "mycroft.ready",
        // Screen/homescreen (1)
        "screen.close.idle.event",
        // User interaction (2)
        "mycroft.events.triggered",
        "recognizer_loop:utterance",
    };

    QCOMPARE(allMessages.count(), 23);  // Verify exactly 23 messages

    // Verify each message converts to valid enum (not -1)
    for (const auto& msg : allMessages) {
        auto enumVal = GuiBusMessages::fromString(msg);
        QVERIFY(enumVal != static_cast<GuiBusMessages::GUIBusMessageType>(-1));
    }
}

QTEST_APPLESS_MAIN(MessageRoutingTest)
#include "message_routing_test.moc"
