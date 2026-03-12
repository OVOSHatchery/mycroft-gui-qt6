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
 * @brief Central registry of all OVOS bus messages supported by mycroft-gui-qt5
 *
 * This file documents the complete set of OVOS Message Bus events that are forwarded
 * from ovos-core to GUI clients via the WebSocket protocol. This is a SUBSET of all
 * OVOS bus messages — only those relevant to GUI rendering and user interaction.
 *
 * CRITICAL: mycroft-gui-qt5 does NOT connect to the OVOS Message Bus directly.
 * All messages flow through the legacy-plugin adapter (ovos-legacy-mycroft-gui-plugin)
 * which translates between the Qt WebSocket protocol and OVOS Message Bus format.
 *
 * ARCHITECTURE:
 *   OVOS Core Bus → legacy-plugin (Tornado server) → mycroft-gui-qt5 (Qt WebSocket client)
 *
 * See: docs/PROTOCOL.md for full message format specifications
 * See: BUS_EVENTS_AUDIT.md for message-to-handler mapping
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

    // ==================== SPEECH RECOGNITION STATE ====================
    RECOGNIZER_WAKEWORD,       // "recognizer_loop:wakeword"
    RECOGNIZER_RECORD_BEGIN,   // "recognizer_loop:record_begin"
    RECOGNIZER_RECORD_END,     // "recognizer_loop:record_end"
    SPEECH_RECOGNITION_UNKNOWN, // "mycroft.speech.recognition.unknown"

    // ==================== SKILL LIFECYCLE STATE ====================
    // Inform client of skill status (not used for rendering, state only)
    STOP_HANDLED,   // "mycroft.stop.handled" or "mycroft.stop"
    INTENT_FAILURE, // "complete_intent_failure"

    // ==================== CORE LIFECYCLE STATE ====================
    SKILLS_LOADED_RESPONSE,  // "mycroft.skills.all_loaded.response"
    READY,                   // "mycroft.ready"

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
    if (typeStr == QLatin1String("recognizer_loop:wakeword"))
        return GUIBusMessageType::RECOGNIZER_WAKEWORD;
    if (typeStr == QLatin1String("recognizer_loop:record_begin"))
        return GUIBusMessageType::RECOGNIZER_RECORD_BEGIN;
    if (typeStr == QLatin1String("recognizer_loop:record_end"))
        return GUIBusMessageType::RECOGNIZER_RECORD_END;
    if (typeStr == QLatin1String("mycroft.speech.recognition.unknown"))
        return GUIBusMessageType::SPEECH_RECOGNITION_UNKNOWN;
    if (typeStr == QLatin1String("mycroft.stop.handled") || typeStr == QLatin1String("mycroft.stop"))
        return GUIBusMessageType::STOP_HANDLED;
    if (typeStr == QLatin1String("complete_intent_failure"))
        return GUIBusMessageType::INTENT_FAILURE;
    if (typeStr == QLatin1String("mycroft.skills.all_loaded.response"))
        return GUIBusMessageType::SKILLS_LOADED_RESPONSE;
    if (typeStr == QLatin1String("mycroft.ready"))
        return GUIBusMessageType::READY;
    if (typeStr == QLatin1String("screen.close.idle.event"))
        return GUIBusMessageType::SCREEN_CLOSE_IDLE_EVENT;

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
        case GUIBusMessageType::RECOGNIZER_WAKEWORD:
            return "recognizer_loop:wakeword";
        case GUIBusMessageType::RECOGNIZER_RECORD_BEGIN:
            return "recognizer_loop:record_begin";
        case GUIBusMessageType::RECOGNIZER_RECORD_END:
            return "recognizer_loop:record_end";
        case GUIBusMessageType::SPEECH_RECOGNITION_UNKNOWN:
            return "mycroft.speech.recognition.unknown";
        case GUIBusMessageType::STOP_HANDLED:
            return "mycroft.stop.handled";
        case GUIBusMessageType::INTENT_FAILURE:
            return "complete_intent_failure";
        case GUIBusMessageType::SKILLS_LOADED_RESPONSE:
            return "mycroft.skills.all_loaded.response";
        case GUIBusMessageType::READY:
            return "mycroft.ready";
        case GUIBusMessageType::SCREEN_CLOSE_IDLE_EVENT:
            return "screen.close.idle.event";
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
        case GUIBusMessageType::RECOGNIZER_WAKEWORD:
        case GUIBusMessageType::RECOGNIZER_RECORD_BEGIN:
        case GUIBusMessageType::RECOGNIZER_RECORD_END:
        case GUIBusMessageType::SPEECH_RECOGNITION_UNKNOWN:
        case GUIBusMessageType::STOP_HANDLED:
        case GUIBusMessageType::INTENT_FAILURE:
        case GUIBusMessageType::SKILLS_LOADED_RESPONSE:
        case GUIBusMessageType::READY:
        case GUIBusMessageType::SCREEN_CLOSE_IDLE_EVENT:
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
