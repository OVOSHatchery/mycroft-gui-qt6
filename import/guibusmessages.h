/*
 * Copyright 2018 by Marco Martin <mart@kde.org>
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
 */

#pragma once

#include <QString>
#include <QDebug>
#include <QLatin1String>

/**
 * @file guibusmessages.h
 * @brief Central registry of all OVOS bus messages supported by mycroft-gui-qt6
 *
 * This file documents the complete set of OVOS Message Bus events that are forwarded
 * from ovos-core to GUI clients via the WebSocket protocol. This is a SUBSET of all
 * OVOS bus messages — only those relevant to GUI rendering, state changes, and user interaction.
 *
 * Includes all 41 messages forwarded from ovos-gui/namespace.py:_define_messages_to_forward():
 * - Core: skill lifecycle, utterance handling, intent failure
 * - Audio: speak, audio output start/end
 * - Speech Recognition: wakeword, recording, sleep/wake, utterance state
 * - Enclosure Eyes: on/off, blink, look, color, animations (13 commands)
 * - Enclosure Mouth: talk, think, listen, smile, viseme, animations (9 commands)
 * - Display: mouth text/image, weather display
 * - Shell Features: brightness, color schemes, notifications, widgets, configuration (protocol extensions)
 *
 * CRITICAL: mycroft-gui-qt6 does NOT connect to the OVOS Message Bus directly (port 8181).
 * All messages flow through the legacy-plugin adapter (ovos-legacy-mycroft-gui-plugin)
 * which translates between the Qt WebSocket protocol (port 18181) and OVOS Message Bus format.
 *
 * ARCHITECTURE:
 *   OVOS Core Bus (8181) → legacy-plugin (Tornado server) → mycroft-gui-qt6 WebSocket (18181)
 *
 * See: ovos-legacy-mycroft-gui-plugin/docs/TWO_BUS_ARCHITECTURE.md for architecture details
 * See: ovos-legacy-mycroft-gui-plugin/docs/PROTOCOL_EXTENSIONS.md for shell feature protocol
 * See: ovos-gui/ovos_gui/namespace.py:419-471 for forwarded message list
 */

namespace GuiBusMessages {

/**
 * Message types forwarded from OVOS core bus to GUI clients.
 *
 * These messages represent a FILTERED subset of the full OVOS Message Bus.
 * Other messages (skills.shutdown, skill.highlight, etc.) are NOT forwarded to GUI clients.
 */
enum class GUIBusMessageType {
    // ==================== INITIALIZATION ====================
    // Client announces presence; receives port in response
    GUI_CONNECTED,  // "mycroft.gui.connected"

    // ==================== SKILL GUI TEMPLATES ====================
    // Insert/remove/move/update pages (QML templates)
    GUI_LIST_INSERT,   // "mycroft.gui.list.insert"
    GUI_LIST_REMOVE,   // "mycroft.gui.list.remove"
    GUI_LIST_MOVE,     // "mycroft.gui.list.move"

    // ==================== SKILL SESSION DATA ====================
    // Update key-value data available to QML
    SESSION_SET,       // "mycroft.session.set"
    SESSION_DELETE,    // "mycroft.session.delete"

    // ==================== SKILL SESSION LISTS ====================
    // Manage lists within session data (e.g., menu items, search results)
    SESSION_LIST_INSERT,  // "mycroft.session.list.insert"
    SESSION_LIST_REMOVE,  // "mycroft.session.list.remove"
    SESSION_LIST_MOVE,    // "mycroft.session.list.move"
    SESSION_LIST_UPDATE,  // "mycroft.session.list.update"

    // ==================== NAMESPACE LIFECYCLE ====================
    // Clear entire skill namespace (e.g., on skill exit)
    CLEAR_NAMESPACE,   // "gui.clear.namespace" (NEW in A3)

    // ==================== SPEECH & AUDIO STATE ====================
    // Inform client of audio I/O activity (not used for rendering, state only)
    RECOGNIZER_AUDIO_OUTPUT_START,  // "recognizer_loop:audio_output_start"
    RECOGNIZER_AUDIO_OUTPUT_END,    // "recognizer_loop:audio_output_end"
    SPEAK,                          // "speak" (TTS output started)

    // ==================== SPEECH RECOGNITION STATE ====================
    RECOGNIZER_WAKEWORD,       // "recognizer_loop:wakeword"
    RECOGNIZER_RECORD_BEGIN,   // "recognizer_loop:record_begin"
    RECOGNIZER_RECORD_END,     // "recognizer_loop:record_end"
    SPEECH_RECOGNITION_UNKNOWN, // "recognizer_loop:recognition_unknown"
    RECOGNIZER_SLEEP,          // "recognizer_loop:sleep"
    RECOGNIZER_WAKE_UP,        // "recognizer_loop:wake_up"
    MYCROFT_AWOKEN,            // "mycroft.awoken"

    // ==================== SKILL LIFECYCLE STATE ====================
    // Inform client of skill status (not used for rendering, state only)
    STOP_HANDLED,   // "mycroft.stop.handled" or "mycroft.stop"
    INTENT_FAILURE, // "complete_intent_failure"
    UTTERANCE_HANDLED,  // "ovos.utterance.handled"
    UTTERANCE_CANCELLED,  // "ovos.utterance.cancelled"
    SKILL_HANDLER_START,  // "mycroft.skill.handler.start"
    SKILL_HANDLER_COMPLETE,  // "mycroft.skill.handler.complete"

    // ==================== CORE LIFECYCLE STATE ====================
    SKILLS_LOADED_RESPONSE,  // "mycroft.skills.all_loaded.response"
    READY,                   // "mycroft.ready"

    // ==================== ENCLOSURE EYES COMMANDS ====================
    // Device LED/eye animation control
    ENCLOSURE_EYES_ON,        // "enclosure.eyes.on"
    ENCLOSURE_EYES_OFF,       // "enclosure.eyes.off"
    ENCLOSURE_EYES_BLINK,     // "enclosure.eyes.blink"
    ENCLOSURE_EYES_NARROW,    // "enclosure.eyes.narrow"
    ENCLOSURE_EYES_LOOK,      // "enclosure.eyes.look"
    ENCLOSURE_EYES_COLOR,     // "enclosure.eyes.color"
    ENCLOSURE_EYES_LEVEL,     // "enclosure.eyes.level"
    ENCLOSURE_EYES_VOLUME,    // "enclosure.eyes.volume"
    ENCLOSURE_EYES_SPIN,      // "enclosure.eyes.spin"
    ENCLOSURE_EYES_TIMEDSPIN, // "enclosure.eyes.timedspin"
    ENCLOSURE_EYES_RESET,     // "enclosure.eyes.reset"
    ENCLOSURE_EYES_SETPIXEL,  // "enclosure.eyes.setpixel"
    ENCLOSURE_EYES_FILL,      // "enclosure.eyes.fill"

    // ==================== ENCLOSURE MOUTH COMMANDS ====================
    // Device mouth/mouth animations
    ENCLOSURE_MOUTH_EVENTS_ACTIVATE,    // "enclosure.mouth.events.activate"
    ENCLOSURE_MOUTH_EVENTS_DEACTIVATE,  // "enclosure.mouth.events.deactivate"
    ENCLOSURE_MOUTH_TALK,      // "enclosure.mouth.talk"
    ENCLOSURE_MOUTH_THINK,     // "enclosure.mouth.think"
    ENCLOSURE_MOUTH_LISTEN,    // "enclosure.mouth.listen"
    ENCLOSURE_MOUTH_SMILE,     // "enclosure.mouth.smile"
    ENCLOSURE_MOUTH_VISEME,    // "enclosure.mouth.viseme"
    ENCLOSURE_MOUTH_VISEME_LIST,  // "enclosure.mouth.viseme_list"
    ENCLOSURE_MOUTH_RESET,     // "enclosure.mouth.reset"
    ENCLOSURE_MOUTH_TEXT,      // "enclosure.mouth.text"
    ENCLOSURE_MOUTH_DISPLAY,   // "enclosure.mouth.display"

    // ==================== DISPLAY MESSAGES ====================
    // Display weather and other widgets
    ENCLOSURE_WEATHER_DISPLAY,  // "enclosure.weather.display"

    // ==================== SCREEN/HOMESCREEN ====================
    // Return to idle screen
    SCREEN_CLOSE_IDLE_EVENT,  // "screen.close.idle.event"

    // ==================== USER INTERACTION (CLIENT → CORE) ====================
    // Events sent FROM GUI client TO core (via legacy-plugin)
    EVENTS_TRIGGERED,   // "mycroft.events.triggered" (button clicks, etc.)
    RECOGNIZER_UTTERANCE,  // "recognizer_loop:utterance" (text input)

    // ==================== SHELL FEATURES (PROTOCOL EXTENSIONS) ====================
    // NEW: Brightness and display control
    GUI_BRIGHTNESS_SET,        // "gui.brightness.set" (CLIENT → CORE)
    GUI_BRIGHTNESS_GET,        // "gui.brightness.get" (CLIENT → CORE)
    GUI_BRIGHTNESS_GET_RESPONSE,  // "gui.brightness.get.response" (CORE → CLIENT)
    GUI_BRIGHTNESS_AUTO_DIM_SET,  // "gui.brightness.auto_dim.set" (CLIENT → CORE)
    GUI_BRIGHTNESS_NIGHT_MODE_SET,  // "gui.brightness.night_mode.set" (CLIENT → CORE)

    // NEW: Color scheme management
    GUI_COLOR_SCHEME_SET,      // "gui.color_scheme.set" (CLIENT ↔ CORE)
    GUI_COLOR_SCHEME_GET,      // "gui.color_scheme.get" (CLIENT → CORE)
    GUI_COLOR_SCHEME_GET_RESPONSE,  // "gui.color_scheme.get.response" (CORE → CLIENT)

    // NEW: Notifications
    GUI_NOTIFICATION_SET,      // "gui.notification.set" (CORE → CLIENT)
    GUI_NOTIFICATION_CLEAR,    // "gui.notification.clear" (CLIENT → CORE)

    // NEW: Custom widgets
    GUI_WIDGET_DISPLAY,        // "gui.widget.display" (CORE → CLIENT)
    GUI_WIDGET_REMOVE,         // "gui.widget.remove" (CLIENT ↔ CORE)

    // NEW: Configuration UI
    GUI_CONFIG_LIST_GET,       // "gui.config.list.get" (CLIENT → CORE)
    GUI_CONFIG_LIST_GET_RESPONSE,  // "gui.config.list.get.response" (CORE → CLIENT)
    GUI_CONFIG_GET,            // "gui.config.get" (CLIENT → CORE)
    GUI_CONFIG_GET_RESPONSE,   // "gui.config.get.response" (CORE → CLIENT)
    GUI_CONFIG_SET,            // "gui.config.set" (CLIENT → CORE)
};

/**
 * Convert string message type to enum value.
 *
 * This function is used in message handlers to convert the JSON "type" field
 * (which is a string) into the corresponding enum value for safe, type-checked
 * message routing.
 *
 * Example:
 *   QString msgType = doc["type"].toString();
 *   auto enumType = fromString(msgType);
 *   if (enumType == GUIBusMessageType::SESSION_SET) { ... }
 *
 * Returns an empty/unknown value if the string doesn't match any known message type.
 * This allows the protocol to be extended without breaking older clients.
 */
inline GUIBusMessageType fromString(const QString &typeStr) {
    // Initialization
    if (typeStr == QLatin1String("mycroft.gui.connected"))
        return GUIBusMessageType::GUI_CONNECTED;

    // Page rendering
    if (typeStr == QLatin1String("mycroft.gui.list.insert"))
        return GUIBusMessageType::GUI_LIST_INSERT;
    if (typeStr == QLatin1String("mycroft.gui.list.remove"))
        return GUIBusMessageType::GUI_LIST_REMOVE;
    if (typeStr == QLatin1String("mycroft.gui.list.move"))
        return GUIBusMessageType::GUI_LIST_MOVE;

    // Session data
    if (typeStr == QLatin1String("mycroft.session.set"))
        return GUIBusMessageType::SESSION_SET;
    if (typeStr == QLatin1String("mycroft.session.delete"))
        return GUIBusMessageType::SESSION_DELETE;
    if (typeStr == QLatin1String("mycroft.session.list.insert"))
        return GUIBusMessageType::SESSION_LIST_INSERT;
    if (typeStr == QLatin1String("mycroft.session.list.remove"))
        return GUIBusMessageType::SESSION_LIST_REMOVE;
    if (typeStr == QLatin1String("mycroft.session.list.move"))
        return GUIBusMessageType::SESSION_LIST_MOVE;
    if (typeStr == QLatin1String("mycroft.session.list.update"))
        return GUIBusMessageType::SESSION_LIST_UPDATE;
    if (typeStr == QLatin1String("gui.clear.namespace"))
        return GUIBusMessageType::CLEAR_NAMESPACE;

    // State changes
    if (typeStr == QLatin1String("recognizer_loop:audio_output_start"))
        return GUIBusMessageType::RECOGNIZER_AUDIO_OUTPUT_START;
    if (typeStr == QLatin1String("recognizer_loop:audio_output_end"))
        return GUIBusMessageType::RECOGNIZER_AUDIO_OUTPUT_END;
    if (typeStr == QLatin1String("speak"))
        return GUIBusMessageType::SPEAK;
    if (typeStr == QLatin1String("recognizer_loop:wakeword"))
        return GUIBusMessageType::RECOGNIZER_WAKEWORD;
    if (typeStr == QLatin1String("recognizer_loop:record_begin"))
        return GUIBusMessageType::RECOGNIZER_RECORD_BEGIN;
    if (typeStr == QLatin1String("recognizer_loop:record_end"))
        return GUIBusMessageType::RECOGNIZER_RECORD_END;
    if (typeStr == QLatin1String("recognizer_loop:recognition_unknown"))
        return GUIBusMessageType::SPEECH_RECOGNITION_UNKNOWN;
    if (typeStr == QLatin1String("recognizer_loop:sleep"))
        return GUIBusMessageType::RECOGNIZER_SLEEP;
    if (typeStr == QLatin1String("recognizer_loop:wake_up"))
        return GUIBusMessageType::RECOGNIZER_WAKE_UP;
    if (typeStr == QLatin1String("mycroft.awoken"))
        return GUIBusMessageType::MYCROFT_AWOKEN;
    if (typeStr == QLatin1String("mycroft.stop.handled") || typeStr == QLatin1String("mycroft.stop"))
        return GUIBusMessageType::STOP_HANDLED;
    if (typeStr == QLatin1String("complete_intent_failure"))
        return GUIBusMessageType::INTENT_FAILURE;
    if (typeStr == QLatin1String("ovos.utterance.handled"))
        return GUIBusMessageType::UTTERANCE_HANDLED;
    if (typeStr == QLatin1String("ovos.utterance.cancelled"))
        return GUIBusMessageType::UTTERANCE_CANCELLED;
    if (typeStr == QLatin1String("mycroft.skill.handler.start"))
        return GUIBusMessageType::SKILL_HANDLER_START;
    if (typeStr == QLatin1String("mycroft.skill.handler.complete"))
        return GUIBusMessageType::SKILL_HANDLER_COMPLETE;
    if (typeStr == QLatin1String("mycroft.skills.all_loaded.response"))
        return GUIBusMessageType::SKILLS_LOADED_RESPONSE;
    if (typeStr == QLatin1String("mycroft.ready"))
        return GUIBusMessageType::READY;
    if (typeStr == QLatin1String("screen.close.idle.event"))
        return GUIBusMessageType::SCREEN_CLOSE_IDLE_EVENT;

    // Enclosure eyes
    if (typeStr == QLatin1String("enclosure.eyes.on"))
        return GUIBusMessageType::ENCLOSURE_EYES_ON;
    if (typeStr == QLatin1String("enclosure.eyes.off"))
        return GUIBusMessageType::ENCLOSURE_EYES_OFF;
    if (typeStr == QLatin1String("enclosure.eyes.blink"))
        return GUIBusMessageType::ENCLOSURE_EYES_BLINK;
    if (typeStr == QLatin1String("enclosure.eyes.narrow"))
        return GUIBusMessageType::ENCLOSURE_EYES_NARROW;
    if (typeStr == QLatin1String("enclosure.eyes.look"))
        return GUIBusMessageType::ENCLOSURE_EYES_LOOK;
    if (typeStr == QLatin1String("enclosure.eyes.color"))
        return GUIBusMessageType::ENCLOSURE_EYES_COLOR;
    if (typeStr == QLatin1String("enclosure.eyes.level"))
        return GUIBusMessageType::ENCLOSURE_EYES_LEVEL;
    if (typeStr == QLatin1String("enclosure.eyes.volume"))
        return GUIBusMessageType::ENCLOSURE_EYES_VOLUME;
    if (typeStr == QLatin1String("enclosure.eyes.spin"))
        return GUIBusMessageType::ENCLOSURE_EYES_SPIN;
    if (typeStr == QLatin1String("enclosure.eyes.timedspin"))
        return GUIBusMessageType::ENCLOSURE_EYES_TIMEDSPIN;
    if (typeStr == QLatin1String("enclosure.eyes.reset"))
        return GUIBusMessageType::ENCLOSURE_EYES_RESET;
    if (typeStr == QLatin1String("enclosure.eyes.setpixel"))
        return GUIBusMessageType::ENCLOSURE_EYES_SETPIXEL;
    if (typeStr == QLatin1String("enclosure.eyes.fill"))
        return GUIBusMessageType::ENCLOSURE_EYES_FILL;

    // Enclosure mouth
    if (typeStr == QLatin1String("enclosure.mouth.events.activate"))
        return GUIBusMessageType::ENCLOSURE_MOUTH_EVENTS_ACTIVATE;
    if (typeStr == QLatin1String("enclosure.mouth.events.deactivate"))
        return GUIBusMessageType::ENCLOSURE_MOUTH_EVENTS_DEACTIVATE;
    if (typeStr == QLatin1String("enclosure.mouth.talk"))
        return GUIBusMessageType::ENCLOSURE_MOUTH_TALK;
    if (typeStr == QLatin1String("enclosure.mouth.think"))
        return GUIBusMessageType::ENCLOSURE_MOUTH_THINK;
    if (typeStr == QLatin1String("enclosure.mouth.listen"))
        return GUIBusMessageType::ENCLOSURE_MOUTH_LISTEN;
    if (typeStr == QLatin1String("enclosure.mouth.smile"))
        return GUIBusMessageType::ENCLOSURE_MOUTH_SMILE;
    if (typeStr == QLatin1String("enclosure.mouth.viseme"))
        return GUIBusMessageType::ENCLOSURE_MOUTH_VISEME;
    if (typeStr == QLatin1String("enclosure.mouth.viseme_list"))
        return GUIBusMessageType::ENCLOSURE_MOUTH_VISEME_LIST;
    if (typeStr == QLatin1String("enclosure.mouth.reset"))
        return GUIBusMessageType::ENCLOSURE_MOUTH_RESET;
    if (typeStr == QLatin1String("enclosure.mouth.text"))
        return GUIBusMessageType::ENCLOSURE_MOUTH_TEXT;
    if (typeStr == QLatin1String("enclosure.mouth.display"))
        return GUIBusMessageType::ENCLOSURE_MOUTH_DISPLAY;

    // Display
    if (typeStr == QLatin1String("enclosure.weather.display"))
        return GUIBusMessageType::ENCLOSURE_WEATHER_DISPLAY;

    // User interaction
    if (typeStr == QLatin1String("mycroft.events.triggered"))
        return GUIBusMessageType::EVENTS_TRIGGERED;
    if (typeStr == QLatin1String("recognizer_loop:utterance"))
        return GUIBusMessageType::RECOGNIZER_UTTERANCE;

    // Shell feature protocol extensions
    // Brightness control
    if (typeStr == QLatin1String("gui.brightness.set"))
        return GUIBusMessageType::GUI_BRIGHTNESS_SET;
    if (typeStr == QLatin1String("gui.brightness.get"))
        return GUIBusMessageType::GUI_BRIGHTNESS_GET;
    if (typeStr == QLatin1String("gui.brightness.get.response"))
        return GUIBusMessageType::GUI_BRIGHTNESS_GET_RESPONSE;
    if (typeStr == QLatin1String("gui.brightness.auto_dim.set"))
        return GUIBusMessageType::GUI_BRIGHTNESS_AUTO_DIM_SET;
    if (typeStr == QLatin1String("gui.brightness.night_mode.set"))
        return GUIBusMessageType::GUI_BRIGHTNESS_NIGHT_MODE_SET;

    // Color scheme management
    if (typeStr == QLatin1String("gui.color_scheme.set"))
        return GUIBusMessageType::GUI_COLOR_SCHEME_SET;
    if (typeStr == QLatin1String("gui.color_scheme.get"))
        return GUIBusMessageType::GUI_COLOR_SCHEME_GET;
    if (typeStr == QLatin1String("gui.color_scheme.get.response"))
        return GUIBusMessageType::GUI_COLOR_SCHEME_GET_RESPONSE;

    // Notifications
    if (typeStr == QLatin1String("gui.notification.set"))
        return GUIBusMessageType::GUI_NOTIFICATION_SET;
    if (typeStr == QLatin1String("gui.notification.clear"))
        return GUIBusMessageType::GUI_NOTIFICATION_CLEAR;

    // Widgets
    if (typeStr == QLatin1String("gui.widget.display"))
        return GUIBusMessageType::GUI_WIDGET_DISPLAY;
    if (typeStr == QLatin1String("gui.widget.remove"))
        return GUIBusMessageType::GUI_WIDGET_REMOVE;

    // Configuration UI
    if (typeStr == QLatin1String("gui.config.list.get"))
        return GUIBusMessageType::GUI_CONFIG_LIST_GET;
    if (typeStr == QLatin1String("gui.config.list.get.response"))
        return GUIBusMessageType::GUI_CONFIG_LIST_GET_RESPONSE;
    if (typeStr == QLatin1String("gui.config.get"))
        return GUIBusMessageType::GUI_CONFIG_GET;
    if (typeStr == QLatin1String("gui.config.get.response"))
        return GUIBusMessageType::GUI_CONFIG_GET_RESPONSE;
    if (typeStr == QLatin1String("gui.config.set"))
        return GUIBusMessageType::GUI_CONFIG_SET;

    // Unknown message type (not in whitelist)
    // This is not an error - allows protocol extensions
    qDebug() << "Unknown GUI message type:" << typeStr << "(not in whitelist, ignored)";
    return static_cast<GUIBusMessageType>(-1);  // Invalid enum value
}

/**
 * Convert enum value to string message type.
 * Useful for debug logging and error messages.
 */
inline const char* toString(GUIBusMessageType type) {
    switch (type) {
        case GUIBusMessageType::GUI_CONNECTED:
            return "mycroft.gui.connected";
        case GUIBusMessageType::GUI_LIST_INSERT:
            return "mycroft.gui.list.insert";
        case GUIBusMessageType::GUI_LIST_REMOVE:
            return "mycroft.gui.list.remove";
        case GUIBusMessageType::GUI_LIST_MOVE:
            return "mycroft.gui.list.move";
        case GUIBusMessageType::SESSION_SET:
            return "mycroft.session.set";
        case GUIBusMessageType::SESSION_DELETE:
            return "mycroft.session.delete";
        case GUIBusMessageType::SESSION_LIST_INSERT:
            return "mycroft.session.list.insert";
        case GUIBusMessageType::SESSION_LIST_REMOVE:
            return "mycroft.session.list.remove";
        case GUIBusMessageType::SESSION_LIST_MOVE:
            return "mycroft.session.list.move";
        case GUIBusMessageType::SESSION_LIST_UPDATE:
            return "mycroft.session.list.update";
        case GUIBusMessageType::CLEAR_NAMESPACE:
            return "gui.clear.namespace";
        case GUIBusMessageType::RECOGNIZER_AUDIO_OUTPUT_START:
            return "recognizer_loop:audio_output_start";
        case GUIBusMessageType::RECOGNIZER_AUDIO_OUTPUT_END:
            return "recognizer_loop:audio_output_end";
        case GUIBusMessageType::SPEAK:
            return "speak";
        case GUIBusMessageType::RECOGNIZER_WAKEWORD:
            return "recognizer_loop:wakeword";
        case GUIBusMessageType::RECOGNIZER_RECORD_BEGIN:
            return "recognizer_loop:record_begin";
        case GUIBusMessageType::RECOGNIZER_RECORD_END:
            return "recognizer_loop:record_end";
        case GUIBusMessageType::SPEECH_RECOGNITION_UNKNOWN:
            return "recognizer_loop:recognition_unknown";
        case GUIBusMessageType::RECOGNIZER_SLEEP:
            return "recognizer_loop:sleep";
        case GUIBusMessageType::RECOGNIZER_WAKE_UP:
            return "recognizer_loop:wake_up";
        case GUIBusMessageType::MYCROFT_AWOKEN:
            return "mycroft.awoken";
        case GUIBusMessageType::STOP_HANDLED:
            return "mycroft.stop.handled";
        case GUIBusMessageType::INTENT_FAILURE:
            return "complete_intent_failure";
        case GUIBusMessageType::UTTERANCE_HANDLED:
            return "ovos.utterance.handled";
        case GUIBusMessageType::UTTERANCE_CANCELLED:
            return "ovos.utterance.cancelled";
        case GUIBusMessageType::SKILL_HANDLER_START:
            return "mycroft.skill.handler.start";
        case GUIBusMessageType::SKILL_HANDLER_COMPLETE:
            return "mycroft.skill.handler.complete";
        case GUIBusMessageType::SKILLS_LOADED_RESPONSE:
            return "mycroft.skills.all_loaded.response";
        case GUIBusMessageType::READY:
            return "mycroft.ready";
        case GUIBusMessageType::SCREEN_CLOSE_IDLE_EVENT:
            return "screen.close.idle.event";
        case GUIBusMessageType::ENCLOSURE_EYES_ON:
            return "enclosure.eyes.on";
        case GUIBusMessageType::ENCLOSURE_EYES_OFF:
            return "enclosure.eyes.off";
        case GUIBusMessageType::ENCLOSURE_EYES_BLINK:
            return "enclosure.eyes.blink";
        case GUIBusMessageType::ENCLOSURE_EYES_NARROW:
            return "enclosure.eyes.narrow";
        case GUIBusMessageType::ENCLOSURE_EYES_LOOK:
            return "enclosure.eyes.look";
        case GUIBusMessageType::ENCLOSURE_EYES_COLOR:
            return "enclosure.eyes.color";
        case GUIBusMessageType::ENCLOSURE_EYES_LEVEL:
            return "enclosure.eyes.level";
        case GUIBusMessageType::ENCLOSURE_EYES_VOLUME:
            return "enclosure.eyes.volume";
        case GUIBusMessageType::ENCLOSURE_EYES_SPIN:
            return "enclosure.eyes.spin";
        case GUIBusMessageType::ENCLOSURE_EYES_TIMEDSPIN:
            return "enclosure.eyes.timedspin";
        case GUIBusMessageType::ENCLOSURE_EYES_RESET:
            return "enclosure.eyes.reset";
        case GUIBusMessageType::ENCLOSURE_EYES_SETPIXEL:
            return "enclosure.eyes.setpixel";
        case GUIBusMessageType::ENCLOSURE_EYES_FILL:
            return "enclosure.eyes.fill";
        case GUIBusMessageType::ENCLOSURE_MOUTH_EVENTS_ACTIVATE:
            return "enclosure.mouth.events.activate";
        case GUIBusMessageType::ENCLOSURE_MOUTH_EVENTS_DEACTIVATE:
            return "enclosure.mouth.events.deactivate";
        case GUIBusMessageType::ENCLOSURE_MOUTH_TALK:
            return "enclosure.mouth.talk";
        case GUIBusMessageType::ENCLOSURE_MOUTH_THINK:
            return "enclosure.mouth.think";
        case GUIBusMessageType::ENCLOSURE_MOUTH_LISTEN:
            return "enclosure.mouth.listen";
        case GUIBusMessageType::ENCLOSURE_MOUTH_SMILE:
            return "enclosure.mouth.smile";
        case GUIBusMessageType::ENCLOSURE_MOUTH_VISEME:
            return "enclosure.mouth.viseme";
        case GUIBusMessageType::ENCLOSURE_MOUTH_VISEME_LIST:
            return "enclosure.mouth.viseme_list";
        case GUIBusMessageType::ENCLOSURE_MOUTH_RESET:
            return "enclosure.mouth.reset";
        case GUIBusMessageType::ENCLOSURE_MOUTH_TEXT:
            return "enclosure.mouth.text";
        case GUIBusMessageType::ENCLOSURE_MOUTH_DISPLAY:
            return "enclosure.mouth.display";
        case GUIBusMessageType::ENCLOSURE_WEATHER_DISPLAY:
            return "enclosure.weather.display";
        case GUIBusMessageType::EVENTS_TRIGGERED:
            return "mycroft.events.triggered";
        case GUIBusMessageType::RECOGNIZER_UTTERANCE:
            return "recognizer_loop:utterance";
        // Shell feature extensions
        case GUIBusMessageType::GUI_BRIGHTNESS_SET:
            return "gui.brightness.set";
        case GUIBusMessageType::GUI_BRIGHTNESS_GET:
            return "gui.brightness.get";
        case GUIBusMessageType::GUI_BRIGHTNESS_GET_RESPONSE:
            return "gui.brightness.get.response";
        case GUIBusMessageType::GUI_BRIGHTNESS_AUTO_DIM_SET:
            return "gui.brightness.auto_dim.set";
        case GUIBusMessageType::GUI_BRIGHTNESS_NIGHT_MODE_SET:
            return "gui.brightness.night_mode.set";
        case GUIBusMessageType::GUI_COLOR_SCHEME_SET:
            return "gui.color_scheme.set";
        case GUIBusMessageType::GUI_COLOR_SCHEME_GET:
            return "gui.color_scheme.get";
        case GUIBusMessageType::GUI_COLOR_SCHEME_GET_RESPONSE:
            return "gui.color_scheme.get.response";
        case GUIBusMessageType::GUI_NOTIFICATION_SET:
            return "gui.notification.set";
        case GUIBusMessageType::GUI_NOTIFICATION_CLEAR:
            return "gui.notification.clear";
        case GUIBusMessageType::GUI_WIDGET_DISPLAY:
            return "gui.widget.display";
        case GUIBusMessageType::GUI_WIDGET_REMOVE:
            return "gui.widget.remove";
        case GUIBusMessageType::GUI_CONFIG_LIST_GET:
            return "gui.config.list.get";
        case GUIBusMessageType::GUI_CONFIG_LIST_GET_RESPONSE:
            return "gui.config.list.get.response";
        case GUIBusMessageType::GUI_CONFIG_GET:
            return "gui.config.get";
        case GUIBusMessageType::GUI_CONFIG_GET_RESPONSE:
            return "gui.config.get.response";
        case GUIBusMessageType::GUI_CONFIG_SET:
            return "gui.config.set";
        default:
            return "UNKNOWN";
    }
}

/**
 * Categorize message types by their role in the GUI communication protocol.
 */
enum class GUIBusMessageCategory {
    INIT,              // Connection negotiation
    PAGE_RENDERING,    // QML template display
    SESSION_DATA,      // Skill data updates
    STATE_CHANGE,      // Status updates (listening, speaking, etc.)
    USER_INTERACTION,  // Events from GUI client back to core
};

inline GUIBusMessageCategory getCategory(GUIBusMessageType type) {
    switch (type) {
        case GUIBusMessageType::GUI_CONNECTED:
            return GUIBusMessageCategory::INIT;
        case GUIBusMessageType::GUI_LIST_INSERT:
        case GUIBusMessageType::GUI_LIST_REMOVE:
        case GUIBusMessageType::GUI_LIST_MOVE:
            return GUIBusMessageCategory::PAGE_RENDERING;
        case GUIBusMessageType::SESSION_SET:
        case GUIBusMessageType::SESSION_DELETE:
        case GUIBusMessageType::SESSION_LIST_INSERT:
        case GUIBusMessageType::SESSION_LIST_REMOVE:
        case GUIBusMessageType::SESSION_LIST_MOVE:
        case GUIBusMessageType::SESSION_LIST_UPDATE:
        case GUIBusMessageType::CLEAR_NAMESPACE:
            return GUIBusMessageCategory::SESSION_DATA;
        case GUIBusMessageType::RECOGNIZER_AUDIO_OUTPUT_START:
        case GUIBusMessageType::RECOGNIZER_AUDIO_OUTPUT_END:
        case GUIBusMessageType::SPEAK:
        case GUIBusMessageType::RECOGNIZER_WAKEWORD:
        case GUIBusMessageType::RECOGNIZER_RECORD_BEGIN:
        case GUIBusMessageType::RECOGNIZER_RECORD_END:
        case GUIBusMessageType::RECOGNIZER_SLEEP:
        case GUIBusMessageType::RECOGNIZER_WAKE_UP:
        case GUIBusMessageType::MYCROFT_AWOKEN:
        case GUIBusMessageType::SPEECH_RECOGNITION_UNKNOWN:
        case GUIBusMessageType::STOP_HANDLED:
        case GUIBusMessageType::INTENT_FAILURE:
        case GUIBusMessageType::UTTERANCE_HANDLED:
        case GUIBusMessageType::UTTERANCE_CANCELLED:
        case GUIBusMessageType::SKILL_HANDLER_START:
        case GUIBusMessageType::SKILL_HANDLER_COMPLETE:
        case GUIBusMessageType::SKILLS_LOADED_RESPONSE:
        case GUIBusMessageType::READY:
        case GUIBusMessageType::SCREEN_CLOSE_IDLE_EVENT:
        case GUIBusMessageType::ENCLOSURE_EYES_ON:
        case GUIBusMessageType::ENCLOSURE_EYES_OFF:
        case GUIBusMessageType::ENCLOSURE_EYES_BLINK:
        case GUIBusMessageType::ENCLOSURE_EYES_NARROW:
        case GUIBusMessageType::ENCLOSURE_EYES_LOOK:
        case GUIBusMessageType::ENCLOSURE_EYES_COLOR:
        case GUIBusMessageType::ENCLOSURE_EYES_LEVEL:
        case GUIBusMessageType::ENCLOSURE_EYES_VOLUME:
        case GUIBusMessageType::ENCLOSURE_EYES_SPIN:
        case GUIBusMessageType::ENCLOSURE_EYES_TIMEDSPIN:
        case GUIBusMessageType::ENCLOSURE_EYES_RESET:
        case GUIBusMessageType::ENCLOSURE_EYES_SETPIXEL:
        case GUIBusMessageType::ENCLOSURE_EYES_FILL:
        case GUIBusMessageType::ENCLOSURE_MOUTH_EVENTS_ACTIVATE:
        case GUIBusMessageType::ENCLOSURE_MOUTH_EVENTS_DEACTIVATE:
        case GUIBusMessageType::ENCLOSURE_MOUTH_TALK:
        case GUIBusMessageType::ENCLOSURE_MOUTH_THINK:
        case GUIBusMessageType::ENCLOSURE_MOUTH_LISTEN:
        case GUIBusMessageType::ENCLOSURE_MOUTH_SMILE:
        case GUIBusMessageType::ENCLOSURE_MOUTH_VISEME:
        case GUIBusMessageType::ENCLOSURE_MOUTH_VISEME_LIST:
        case GUIBusMessageType::ENCLOSURE_MOUTH_RESET:
        case GUIBusMessageType::ENCLOSURE_MOUTH_TEXT:
        case GUIBusMessageType::ENCLOSURE_MOUTH_DISPLAY:
        case GUIBusMessageType::ENCLOSURE_WEATHER_DISPLAY:
            return GUIBusMessageCategory::STATE_CHANGE;
        case GUIBusMessageType::EVENTS_TRIGGERED:
        case GUIBusMessageType::RECOGNIZER_UTTERANCE:
            return GUIBusMessageCategory::USER_INTERACTION;
        // Shell feature extensions (protocol extensions)
        // Brightness - bidirectional (client sets, core can push updates)
        case GUIBusMessageType::GUI_BRIGHTNESS_SET:
        case GUIBusMessageType::GUI_BRIGHTNESS_GET:
        case GUIBusMessageType::GUI_BRIGHTNESS_AUTO_DIM_SET:
        case GUIBusMessageType::GUI_BRIGHTNESS_NIGHT_MODE_SET:
            return GUIBusMessageCategory::USER_INTERACTION;  // Client initiates
        case GUIBusMessageType::GUI_BRIGHTNESS_GET_RESPONSE:
            return GUIBusMessageCategory::SESSION_DATA;  // Server responds
        // Color scheme - bidirectional
        case GUIBusMessageType::GUI_COLOR_SCHEME_SET:
        case GUIBusMessageType::GUI_COLOR_SCHEME_GET:
            return GUIBusMessageCategory::USER_INTERACTION;  // Client initiates
        case GUIBusMessageType::GUI_COLOR_SCHEME_GET_RESPONSE:
            return GUIBusMessageCategory::SESSION_DATA;  // Server responds
        // Notifications - core to client
        case GUIBusMessageType::GUI_NOTIFICATION_SET:
            return GUIBusMessageCategory::SESSION_DATA;  // Server pushes
        case GUIBusMessageType::GUI_NOTIFICATION_CLEAR:
            return GUIBusMessageCategory::USER_INTERACTION;  // Client requests
        // Widgets - bidirectional
        case GUIBusMessageType::GUI_WIDGET_DISPLAY:
            return GUIBusMessageCategory::SESSION_DATA;  // Server pushes
        case GUIBusMessageType::GUI_WIDGET_REMOVE:
            return GUIBusMessageCategory::USER_INTERACTION;  // Client requests
        // Configuration - bidirectional
        case GUIBusMessageType::GUI_CONFIG_LIST_GET:
        case GUIBusMessageType::GUI_CONFIG_GET:
        case GUIBusMessageType::GUI_CONFIG_SET:
            return GUIBusMessageCategory::USER_INTERACTION;  // Client initiates
        case GUIBusMessageType::GUI_CONFIG_LIST_GET_RESPONSE:
        case GUIBusMessageType::GUI_CONFIG_GET_RESPONSE:
            return GUIBusMessageCategory::SESSION_DATA;  // Server responds
        default:
            return GUIBusMessageCategory::STATE_CHANGE;
    }
}

} // namespace GuiBusMessages
