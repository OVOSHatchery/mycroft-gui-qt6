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
 * @brief Central registry of all messages on the mycroft GUI wire protocol.
 *
 * Messages arriving on the GUI WebSocket (port 18181) belong to three distinct layers:
 *
 * 1. **WireMessage** — The classic mycroft GUI wire protocol. These are the protocol's
 *    own messages for connection setup, session data, page rendering, event triggering,
 *    and namespace lifecycle. They use `mycroft.*` prefixes as part of the wire format.
 *
 * 2. **ShellEvent** — Shell feature protocol extensions (`gui.*`). Brightness, color
 *    schemes, notifications, widgets, configuration, hints. These are bidirectional
 *    between client and server, extending the base protocol for smart display features.
 *
 * 3. **AssistantEvent** — Forwarded OVOS Message Bus events. These are real bus messages
 *    (recognizer_loop:*, speak, enclosure.*, mycroft.stop.*, etc.) that the server wraps
 *    in the wire protocol JSON format and forwards to the GUI client for state display.
 *    The GUI does NOT connect to the bus directly — these arrive via the legacy-plugin adapter.
 *
 * ARCHITECTURE:
 *   OVOS Core Bus (8181) -> ovos-gui service -> legacy-plugin (18181) -> this client
 *
 * All three message types arrive on the SAME WebSocket. The type string in the JSON
 * determines which layer the message belongs to.
 */

namespace GuiBusMessages {

// ============================================================================
// Layer 1: Wire Protocol Messages
// ============================================================================

/**
 * The classic mycroft GUI wire protocol messages.
 *
 * These are the protocol's own messages — NOT forwarded bus events.
 * They handle connection negotiation, session data CRUD, QML page management,
 * event triggering between skill and GUI, and namespace lifecycle.
 *
 * Wire strings use `mycroft.*` prefix — this is the PROTOCOL FORMAT, not branding.
 */
enum class WireMessage {
    // Connection
    GUI_CONNECTED,          // "mycroft.gui.connected"

    // Session data (key-value pairs for QML bindings)
    SESSION_SET,            // "mycroft.session.set"
    SESSION_DELETE,         // "mycroft.session.delete"

    // Session lists (ordered collections within session data)
    SESSION_LIST_INSERT,    // "mycroft.session.list.insert"
    SESSION_LIST_REMOVE,    // "mycroft.session.list.remove"
    SESSION_LIST_MOVE,      // "mycroft.session.list.move"
    SESSION_LIST_UPDATE,    // "mycroft.session.list.update"

    // QML page management
    GUI_LIST_INSERT,        // "mycroft.gui.list.insert"
    GUI_LIST_REMOVE,        // "mycroft.gui.list.remove"
    GUI_LIST_MOVE,          // "mycroft.gui.list.move"

    // Skill-GUI events (bidirectional)
    EVENTS_TRIGGERED,       // "mycroft.events.triggered"

    // Namespace lifecycle
    CLEAR_NAMESPACE,        // "gui.clear.namespace"

    // Explicit forwarding wrappers (new protocol v2)
    // These wrap messages from other layers in an explicit wire envelope.
    // The actual event type is in the "event_type" field of the JSON payload.
    FORWARD_ASSISTANT,      // "mycroft.gui.forward.assistant"
    FORWARD_SHELL,          // "mycroft.gui.forward.shell"
};

inline WireMessage wireFromString(const QString &s) {
    if (s == QLatin1String("mycroft.gui.connected"))       return WireMessage::GUI_CONNECTED;
    if (s == QLatin1String("mycroft.session.set"))          return WireMessage::SESSION_SET;
    if (s == QLatin1String("mycroft.session.delete"))       return WireMessage::SESSION_DELETE;
    if (s == QLatin1String("mycroft.session.list.insert"))  return WireMessage::SESSION_LIST_INSERT;
    if (s == QLatin1String("mycroft.session.list.remove"))  return WireMessage::SESSION_LIST_REMOVE;
    if (s == QLatin1String("mycroft.session.list.move"))    return WireMessage::SESSION_LIST_MOVE;
    if (s == QLatin1String("mycroft.session.list.update"))  return WireMessage::SESSION_LIST_UPDATE;
    if (s == QLatin1String("mycroft.gui.list.insert"))      return WireMessage::GUI_LIST_INSERT;
    if (s == QLatin1String("mycroft.gui.list.remove"))      return WireMessage::GUI_LIST_REMOVE;
    if (s == QLatin1String("mycroft.gui.list.move"))        return WireMessage::GUI_LIST_MOVE;
    if (s == QLatin1String("mycroft.events.triggered"))     return WireMessage::EVENTS_TRIGGERED;
    if (s == QLatin1String("gui.clear.namespace"))          return WireMessage::CLEAR_NAMESPACE;
    if (s == QLatin1String("mycroft.gui.forward.assistant")) return WireMessage::FORWARD_ASSISTANT;
    if (s == QLatin1String("mycroft.gui.forward.shell"))    return WireMessage::FORWARD_SHELL;
    return static_cast<WireMessage>(-1);
}

inline const char* toString(WireMessage m) {
    switch (m) {
        case WireMessage::GUI_CONNECTED:       return "mycroft.gui.connected";
        case WireMessage::SESSION_SET:         return "mycroft.session.set";
        case WireMessage::SESSION_DELETE:      return "mycroft.session.delete";
        case WireMessage::SESSION_LIST_INSERT: return "mycroft.session.list.insert";
        case WireMessage::SESSION_LIST_REMOVE: return "mycroft.session.list.remove";
        case WireMessage::SESSION_LIST_MOVE:   return "mycroft.session.list.move";
        case WireMessage::SESSION_LIST_UPDATE: return "mycroft.session.list.update";
        case WireMessage::GUI_LIST_INSERT:     return "mycroft.gui.list.insert";
        case WireMessage::GUI_LIST_REMOVE:     return "mycroft.gui.list.remove";
        case WireMessage::GUI_LIST_MOVE:       return "mycroft.gui.list.move";
        case WireMessage::EVENTS_TRIGGERED:    return "mycroft.events.triggered";
        case WireMessage::CLEAR_NAMESPACE:     return "gui.clear.namespace";
        case WireMessage::FORWARD_ASSISTANT:  return "mycroft.gui.forward.assistant";
        case WireMessage::FORWARD_SHELL:     return "mycroft.gui.forward.shell";
        default: return "UNKNOWN_WIRE";
    }
}

// ============================================================================
// Layer 2: Shell Feature Events (protocol extensions)
// ============================================================================

/**
 * Shell feature protocol extensions.
 *
 * These extend the base wire protocol with smart display features:
 * brightness control, color schemes, notifications, widgets, configuration, hints.
 *
 * All use `gui.*` prefix. Bidirectional — some client-initiated, some server-pushed.
 */
enum class ShellEvent {
    // Brightness
    BRIGHTNESS_SET,             // "gui.brightness.set"
    BRIGHTNESS_GET,             // "gui.brightness.get"
    BRIGHTNESS_GET_RESPONSE,    // "gui.brightness.get.response"
    BRIGHTNESS_AUTO_DIM_SET,    // "gui.brightness.auto_dim.set"
    BRIGHTNESS_NIGHT_MODE_SET,  // "gui.brightness.night_mode.set"

    // Color scheme
    COLOR_SCHEME_SET,           // "gui.color_scheme.set"
    COLOR_SCHEME_GET,           // "gui.color_scheme.get"
    COLOR_SCHEME_GET_RESPONSE,  // "gui.color_scheme.get.response"

    // Notifications
    NOTIFICATION_SET,           // "gui.notification.set"
    NOTIFICATION_CLEAR,         // "gui.notification.clear"

    // Widgets
    WIDGET_DISPLAY,             // "gui.widget.display"
    WIDGET_REMOVE,              // "gui.widget.remove"

    // Configuration
    CONFIG_LIST_GET,            // "gui.config.list.get"
    CONFIG_LIST_GET_RESPONSE,   // "gui.config.list.get.response"
    CONFIG_GET,                 // "gui.config.get"
    CONFIG_GET_RESPONSE,        // "gui.config.get.response"
    CONFIG_SET,                 // "gui.config.set"

    // Hints
    HINTS_GET,                  // "gui.hints.get"
    HINTS_GET_RESPONSE,         // "gui.hints.get.response"
};

inline ShellEvent shellFromString(const QString &s) {
    if (s == QLatin1String("gui.brightness.set"))              return ShellEvent::BRIGHTNESS_SET;
    if (s == QLatin1String("gui.brightness.get"))              return ShellEvent::BRIGHTNESS_GET;
    if (s == QLatin1String("gui.brightness.get.response"))     return ShellEvent::BRIGHTNESS_GET_RESPONSE;
    if (s == QLatin1String("gui.brightness.auto_dim.set"))     return ShellEvent::BRIGHTNESS_AUTO_DIM_SET;
    if (s == QLatin1String("gui.brightness.night_mode.set"))   return ShellEvent::BRIGHTNESS_NIGHT_MODE_SET;
    if (s == QLatin1String("gui.color_scheme.set"))            return ShellEvent::COLOR_SCHEME_SET;
    if (s == QLatin1String("gui.color_scheme.get"))            return ShellEvent::COLOR_SCHEME_GET;
    if (s == QLatin1String("gui.color_scheme.get.response"))   return ShellEvent::COLOR_SCHEME_GET_RESPONSE;
    if (s == QLatin1String("gui.notification.set"))            return ShellEvent::NOTIFICATION_SET;
    if (s == QLatin1String("gui.notification.clear"))          return ShellEvent::NOTIFICATION_CLEAR;
    if (s == QLatin1String("gui.widget.display"))              return ShellEvent::WIDGET_DISPLAY;
    if (s == QLatin1String("gui.widget.remove"))               return ShellEvent::WIDGET_REMOVE;
    if (s == QLatin1String("gui.config.list.get"))             return ShellEvent::CONFIG_LIST_GET;
    if (s == QLatin1String("gui.config.list.get.response"))    return ShellEvent::CONFIG_LIST_GET_RESPONSE;
    if (s == QLatin1String("gui.config.get"))                  return ShellEvent::CONFIG_GET;
    if (s == QLatin1String("gui.config.get.response"))         return ShellEvent::CONFIG_GET_RESPONSE;
    if (s == QLatin1String("gui.config.set"))                  return ShellEvent::CONFIG_SET;
    if (s == QLatin1String("gui.hints.get"))                   return ShellEvent::HINTS_GET;
    if (s == QLatin1String("gui.hints.get.response"))          return ShellEvent::HINTS_GET_RESPONSE;
    return static_cast<ShellEvent>(-1);
}

inline const char* toString(ShellEvent e) {
    switch (e) {
        case ShellEvent::BRIGHTNESS_SET:            return "gui.brightness.set";
        case ShellEvent::BRIGHTNESS_GET:            return "gui.brightness.get";
        case ShellEvent::BRIGHTNESS_GET_RESPONSE:   return "gui.brightness.get.response";
        case ShellEvent::BRIGHTNESS_AUTO_DIM_SET:   return "gui.brightness.auto_dim.set";
        case ShellEvent::BRIGHTNESS_NIGHT_MODE_SET: return "gui.brightness.night_mode.set";
        case ShellEvent::COLOR_SCHEME_SET:          return "gui.color_scheme.set";
        case ShellEvent::COLOR_SCHEME_GET:          return "gui.color_scheme.get";
        case ShellEvent::COLOR_SCHEME_GET_RESPONSE: return "gui.color_scheme.get.response";
        case ShellEvent::NOTIFICATION_SET:          return "gui.notification.set";
        case ShellEvent::NOTIFICATION_CLEAR:        return "gui.notification.clear";
        case ShellEvent::WIDGET_DISPLAY:            return "gui.widget.display";
        case ShellEvent::WIDGET_REMOVE:             return "gui.widget.remove";
        case ShellEvent::CONFIG_LIST_GET:           return "gui.config.list.get";
        case ShellEvent::CONFIG_LIST_GET_RESPONSE:  return "gui.config.list.get.response";
        case ShellEvent::CONFIG_GET:                return "gui.config.get";
        case ShellEvent::CONFIG_GET_RESPONSE:       return "gui.config.get.response";
        case ShellEvent::CONFIG_SET:                return "gui.config.set";
        case ShellEvent::HINTS_GET:                 return "gui.hints.get";
        case ShellEvent::HINTS_GET_RESPONSE:        return "gui.hints.get.response";
        default: return "UNKNOWN_SHELL";
    }
}

// ============================================================================
// Layer 3: Forwarded Assistant Events (OVOS bus messages wrapped in wire protocol)
// ============================================================================

/**
 * OVOS Message Bus events forwarded to GUI clients.
 *
 * These are NOT wire protocol messages — they are real bus events that the
 * ovos-gui service forwards through the legacy-plugin adapter. The GUI client
 * uses them for state display (listening indicator, speaking animation, etc.)
 * but does NOT originate them.
 *
 * Exception: RECOGNIZER_UTTERANCE is sent CLIENT -> SERVER (text input).
 *
 * See: ovos-gui/ovos_gui/namespace.py:_define_messages_to_forward()
 */
enum class AssistantEvent {
    // Speech & audio state
    RECOGNIZER_AUDIO_OUTPUT_START,  // "recognizer_loop:audio_output_start"
    RECOGNIZER_AUDIO_OUTPUT_END,    // "recognizer_loop:audio_output_end"
    SPEAK,                          // "speak"

    // Speech recognition state
    RECOGNIZER_WAKEWORD,            // "recognizer_loop:wakeword"
    RECOGNIZER_RECORD_BEGIN,        // "recognizer_loop:record_begin"
    RECOGNIZER_RECORD_END,          // "recognizer_loop:record_end"
    SPEECH_RECOGNITION_UNKNOWN,     // "recognizer_loop:recognition_unknown"
    RECOGNIZER_SLEEP,               // "recognizer_loop:sleep"
    RECOGNIZER_WAKE_UP,             // "recognizer_loop:wake_up"
    OVOS_AWOKEN,                    // "mycroft.awoken"

    // User input (CLIENT -> SERVER)
    RECOGNIZER_UTTERANCE,           // "recognizer_loop:utterance"

    // Skill lifecycle
    STOP_HANDLED,                   // "mycroft.stop.handled" / "mycroft.stop"
    INTENT_FAILURE,                 // "complete_intent_failure"
    UTTERANCE_HANDLED,              // "ovos.utterance.handled"
    UTTERANCE_CANCELLED,            // "ovos.utterance.cancelled"
    SKILL_HANDLER_START,            // "mycroft.skill.handler.start"
    SKILL_HANDLER_COMPLETE,         // "mycroft.skill.handler.complete"

    // Core lifecycle
    SKILLS_LOADED_RESPONSE,         // "mycroft.skills.all_loaded.response"
    READY,                          // "mycroft.ready"

    // Screen
    SCREEN_CLOSE_IDLE_EVENT,        // "screen.close.idle.event"

    // Enclosure eyes
    ENCLOSURE_EYES_ON,              // "enclosure.eyes.on"
    ENCLOSURE_EYES_OFF,             // "enclosure.eyes.off"
    ENCLOSURE_EYES_BLINK,           // "enclosure.eyes.blink"
    ENCLOSURE_EYES_NARROW,          // "enclosure.eyes.narrow"
    ENCLOSURE_EYES_LOOK,            // "enclosure.eyes.look"
    ENCLOSURE_EYES_COLOR,           // "enclosure.eyes.color"
    ENCLOSURE_EYES_LEVEL,           // "enclosure.eyes.level"
    ENCLOSURE_EYES_VOLUME,          // "enclosure.eyes.volume"
    ENCLOSURE_EYES_SPIN,            // "enclosure.eyes.spin"
    ENCLOSURE_EYES_TIMEDSPIN,       // "enclosure.eyes.timedspin"
    ENCLOSURE_EYES_RESET,           // "enclosure.eyes.reset"
    ENCLOSURE_EYES_SETPIXEL,        // "enclosure.eyes.setpixel"
    ENCLOSURE_EYES_FILL,            // "enclosure.eyes.fill"

    // Enclosure mouth
    ENCLOSURE_MOUTH_EVENTS_ACTIVATE,    // "enclosure.mouth.events.activate"
    ENCLOSURE_MOUTH_EVENTS_DEACTIVATE,  // "enclosure.mouth.events.deactivate"
    ENCLOSURE_MOUTH_TALK,               // "enclosure.mouth.talk"
    ENCLOSURE_MOUTH_THINK,              // "enclosure.mouth.think"
    ENCLOSURE_MOUTH_LISTEN,             // "enclosure.mouth.listen"
    ENCLOSURE_MOUTH_SMILE,              // "enclosure.mouth.smile"
    ENCLOSURE_MOUTH_VISEME,             // "enclosure.mouth.viseme"
    ENCLOSURE_MOUTH_VISEME_LIST,        // "enclosure.mouth.viseme_list"
    ENCLOSURE_MOUTH_RESET,              // "enclosure.mouth.reset"
    ENCLOSURE_MOUTH_TEXT,               // "enclosure.mouth.text"
    ENCLOSURE_MOUTH_DISPLAY,            // "enclosure.mouth.display"

    // Enclosure display
    ENCLOSURE_WEATHER_DISPLAY,          // "enclosure.weather.display"
};

inline AssistantEvent assistantFromString(const QString &s) {
    if (s == QLatin1String("recognizer_loop:audio_output_start")) return AssistantEvent::RECOGNIZER_AUDIO_OUTPUT_START;
    if (s == QLatin1String("recognizer_loop:audio_output_end"))   return AssistantEvent::RECOGNIZER_AUDIO_OUTPUT_END;
    if (s == QLatin1String("speak"))                              return AssistantEvent::SPEAK;
    if (s == QLatin1String("recognizer_loop:wakeword"))           return AssistantEvent::RECOGNIZER_WAKEWORD;
    if (s == QLatin1String("recognizer_loop:record_begin"))       return AssistantEvent::RECOGNIZER_RECORD_BEGIN;
    if (s == QLatin1String("recognizer_loop:record_end"))         return AssistantEvent::RECOGNIZER_RECORD_END;
    if (s == QLatin1String("recognizer_loop:recognition_unknown")) return AssistantEvent::SPEECH_RECOGNITION_UNKNOWN;
    if (s == QLatin1String("recognizer_loop:sleep"))              return AssistantEvent::RECOGNIZER_SLEEP;
    if (s == QLatin1String("recognizer_loop:wake_up"))            return AssistantEvent::RECOGNIZER_WAKE_UP;
    if (s == QLatin1String("mycroft.awoken"))                     return AssistantEvent::OVOS_AWOKEN;
    if (s == QLatin1String("recognizer_loop:utterance"))          return AssistantEvent::RECOGNIZER_UTTERANCE;
    if (s == QLatin1String("mycroft.stop.handled") || s == QLatin1String("mycroft.stop"))
        return AssistantEvent::STOP_HANDLED;
    if (s == QLatin1String("complete_intent_failure"))            return AssistantEvent::INTENT_FAILURE;
    if (s == QLatin1String("ovos.utterance.handled"))             return AssistantEvent::UTTERANCE_HANDLED;
    if (s == QLatin1String("ovos.utterance.cancelled"))           return AssistantEvent::UTTERANCE_CANCELLED;
    if (s == QLatin1String("mycroft.skill.handler.start"))        return AssistantEvent::SKILL_HANDLER_START;
    if (s == QLatin1String("mycroft.skill.handler.complete"))     return AssistantEvent::SKILL_HANDLER_COMPLETE;
    if (s == QLatin1String("mycroft.skills.all_loaded.response")) return AssistantEvent::SKILLS_LOADED_RESPONSE;
    if (s == QLatin1String("mycroft.ready"))                      return AssistantEvent::READY;
    if (s == QLatin1String("screen.close.idle.event"))            return AssistantEvent::SCREEN_CLOSE_IDLE_EVENT;
    // Enclosure eyes
    if (s == QLatin1String("enclosure.eyes.on"))        return AssistantEvent::ENCLOSURE_EYES_ON;
    if (s == QLatin1String("enclosure.eyes.off"))       return AssistantEvent::ENCLOSURE_EYES_OFF;
    if (s == QLatin1String("enclosure.eyes.blink"))     return AssistantEvent::ENCLOSURE_EYES_BLINK;
    if (s == QLatin1String("enclosure.eyes.narrow"))    return AssistantEvent::ENCLOSURE_EYES_NARROW;
    if (s == QLatin1String("enclosure.eyes.look"))      return AssistantEvent::ENCLOSURE_EYES_LOOK;
    if (s == QLatin1String("enclosure.eyes.color"))     return AssistantEvent::ENCLOSURE_EYES_COLOR;
    if (s == QLatin1String("enclosure.eyes.level"))     return AssistantEvent::ENCLOSURE_EYES_LEVEL;
    if (s == QLatin1String("enclosure.eyes.volume"))    return AssistantEvent::ENCLOSURE_EYES_VOLUME;
    if (s == QLatin1String("enclosure.eyes.spin"))      return AssistantEvent::ENCLOSURE_EYES_SPIN;
    if (s == QLatin1String("enclosure.eyes.timedspin")) return AssistantEvent::ENCLOSURE_EYES_TIMEDSPIN;
    if (s == QLatin1String("enclosure.eyes.reset"))     return AssistantEvent::ENCLOSURE_EYES_RESET;
    if (s == QLatin1String("enclosure.eyes.setpixel"))  return AssistantEvent::ENCLOSURE_EYES_SETPIXEL;
    if (s == QLatin1String("enclosure.eyes.fill"))      return AssistantEvent::ENCLOSURE_EYES_FILL;
    // Enclosure mouth
    if (s == QLatin1String("enclosure.mouth.events.activate"))   return AssistantEvent::ENCLOSURE_MOUTH_EVENTS_ACTIVATE;
    if (s == QLatin1String("enclosure.mouth.events.deactivate")) return AssistantEvent::ENCLOSURE_MOUTH_EVENTS_DEACTIVATE;
    if (s == QLatin1String("enclosure.mouth.talk"))     return AssistantEvent::ENCLOSURE_MOUTH_TALK;
    if (s == QLatin1String("enclosure.mouth.think"))    return AssistantEvent::ENCLOSURE_MOUTH_THINK;
    if (s == QLatin1String("enclosure.mouth.listen"))   return AssistantEvent::ENCLOSURE_MOUTH_LISTEN;
    if (s == QLatin1String("enclosure.mouth.smile"))    return AssistantEvent::ENCLOSURE_MOUTH_SMILE;
    if (s == QLatin1String("enclosure.mouth.viseme"))   return AssistantEvent::ENCLOSURE_MOUTH_VISEME;
    if (s == QLatin1String("enclosure.mouth.viseme_list")) return AssistantEvent::ENCLOSURE_MOUTH_VISEME_LIST;
    if (s == QLatin1String("enclosure.mouth.reset"))    return AssistantEvent::ENCLOSURE_MOUTH_RESET;
    if (s == QLatin1String("enclosure.mouth.text"))     return AssistantEvent::ENCLOSURE_MOUTH_TEXT;
    if (s == QLatin1String("enclosure.mouth.display"))  return AssistantEvent::ENCLOSURE_MOUTH_DISPLAY;
    if (s == QLatin1String("enclosure.weather.display")) return AssistantEvent::ENCLOSURE_WEATHER_DISPLAY;
    return static_cast<AssistantEvent>(-1);
}

inline const char* toString(AssistantEvent e) {
    switch (e) {
        case AssistantEvent::RECOGNIZER_AUDIO_OUTPUT_START: return "recognizer_loop:audio_output_start";
        case AssistantEvent::RECOGNIZER_AUDIO_OUTPUT_END:   return "recognizer_loop:audio_output_end";
        case AssistantEvent::SPEAK:                         return "speak";
        case AssistantEvent::RECOGNIZER_WAKEWORD:           return "recognizer_loop:wakeword";
        case AssistantEvent::RECOGNIZER_RECORD_BEGIN:       return "recognizer_loop:record_begin";
        case AssistantEvent::RECOGNIZER_RECORD_END:         return "recognizer_loop:record_end";
        case AssistantEvent::SPEECH_RECOGNITION_UNKNOWN:    return "recognizer_loop:recognition_unknown";
        case AssistantEvent::RECOGNIZER_SLEEP:              return "recognizer_loop:sleep";
        case AssistantEvent::RECOGNIZER_WAKE_UP:            return "recognizer_loop:wake_up";
        case AssistantEvent::OVOS_AWOKEN:                   return "mycroft.awoken";
        case AssistantEvent::RECOGNIZER_UTTERANCE:          return "recognizer_loop:utterance";
        case AssistantEvent::STOP_HANDLED:                  return "mycroft.stop.handled";
        case AssistantEvent::INTENT_FAILURE:                return "complete_intent_failure";
        case AssistantEvent::UTTERANCE_HANDLED:             return "ovos.utterance.handled";
        case AssistantEvent::UTTERANCE_CANCELLED:           return "ovos.utterance.cancelled";
        case AssistantEvent::SKILL_HANDLER_START:           return "mycroft.skill.handler.start";
        case AssistantEvent::SKILL_HANDLER_COMPLETE:        return "mycroft.skill.handler.complete";
        case AssistantEvent::SKILLS_LOADED_RESPONSE:        return "mycroft.skills.all_loaded.response";
        case AssistantEvent::READY:                         return "mycroft.ready";
        case AssistantEvent::SCREEN_CLOSE_IDLE_EVENT:       return "screen.close.idle.event";
        case AssistantEvent::ENCLOSURE_EYES_ON:             return "enclosure.eyes.on";
        case AssistantEvent::ENCLOSURE_EYES_OFF:            return "enclosure.eyes.off";
        case AssistantEvent::ENCLOSURE_EYES_BLINK:          return "enclosure.eyes.blink";
        case AssistantEvent::ENCLOSURE_EYES_NARROW:         return "enclosure.eyes.narrow";
        case AssistantEvent::ENCLOSURE_EYES_LOOK:           return "enclosure.eyes.look";
        case AssistantEvent::ENCLOSURE_EYES_COLOR:          return "enclosure.eyes.color";
        case AssistantEvent::ENCLOSURE_EYES_LEVEL:          return "enclosure.eyes.level";
        case AssistantEvent::ENCLOSURE_EYES_VOLUME:         return "enclosure.eyes.volume";
        case AssistantEvent::ENCLOSURE_EYES_SPIN:           return "enclosure.eyes.spin";
        case AssistantEvent::ENCLOSURE_EYES_TIMEDSPIN:      return "enclosure.eyes.timedspin";
        case AssistantEvent::ENCLOSURE_EYES_RESET:          return "enclosure.eyes.reset";
        case AssistantEvent::ENCLOSURE_EYES_SETPIXEL:       return "enclosure.eyes.setpixel";
        case AssistantEvent::ENCLOSURE_EYES_FILL:           return "enclosure.eyes.fill";
        case AssistantEvent::ENCLOSURE_MOUTH_EVENTS_ACTIVATE:   return "enclosure.mouth.events.activate";
        case AssistantEvent::ENCLOSURE_MOUTH_EVENTS_DEACTIVATE: return "enclosure.mouth.events.deactivate";
        case AssistantEvent::ENCLOSURE_MOUTH_TALK:          return "enclosure.mouth.talk";
        case AssistantEvent::ENCLOSURE_MOUTH_THINK:         return "enclosure.mouth.think";
        case AssistantEvent::ENCLOSURE_MOUTH_LISTEN:        return "enclosure.mouth.listen";
        case AssistantEvent::ENCLOSURE_MOUTH_SMILE:         return "enclosure.mouth.smile";
        case AssistantEvent::ENCLOSURE_MOUTH_VISEME:        return "enclosure.mouth.viseme";
        case AssistantEvent::ENCLOSURE_MOUTH_VISEME_LIST:   return "enclosure.mouth.viseme_list";
        case AssistantEvent::ENCLOSURE_MOUTH_RESET:         return "enclosure.mouth.reset";
        case AssistantEvent::ENCLOSURE_MOUTH_TEXT:          return "enclosure.mouth.text";
        case AssistantEvent::ENCLOSURE_MOUTH_DISPLAY:       return "enclosure.mouth.display";
        case AssistantEvent::ENCLOSURE_WEATHER_DISPLAY:     return "enclosure.weather.display";
        default: return "UNKNOWN_ASSISTANT";
    }
}

// ============================================================================
// Unified message classification
// ============================================================================

/**
 * Which protocol layer a message belongs to.
 */
enum class MessageLayer {
    WIRE,       // Classic mycroft GUI wire protocol
    SHELL,      // Shell feature extensions (gui.*)
    ASSISTANT,  // Forwarded OVOS bus events
    UNKNOWN,    // Not recognized
};

/**
 * Result of parsing a message type string.
 * Exactly one of wire/shell/assistant is valid based on the layer field.
 */
struct ParsedMessage {
    MessageLayer layer;
    WireMessage wire;
    ShellEvent shell;
    AssistantEvent assistant;
};

/**
 * Parse a message type string and classify it into the correct protocol layer.
 *
 * Usage:
 *   auto parsed = GuiBusMessages::parseMessage(typeStr);
 *   switch (parsed.layer) {
 *       case MessageLayer::WIRE:
 *           handleWireMessage(parsed.wire, doc);
 *           break;
 *       case MessageLayer::SHELL:
 *           handleShellEvent(parsed.shell, doc);
 *           break;
 *       case MessageLayer::ASSISTANT:
 *           handleAssistantEvent(parsed.assistant, doc);
 *           break;
 *       case MessageLayer::UNKNOWN:
 *           qDebug() << "Unknown message:" << typeStr;
 *           break;
 *   }
 */
inline ParsedMessage parseMessage(const QString &typeStr) {
    ParsedMessage result;
    result.wire = static_cast<WireMessage>(-1);
    result.shell = static_cast<ShellEvent>(-1);
    result.assistant = static_cast<AssistantEvent>(-1);

    // Try wire protocol first (most frequent in normal operation)
    auto w = wireFromString(typeStr);
    if (w != static_cast<WireMessage>(-1)) {
        result.layer = MessageLayer::WIRE;
        result.wire = w;
        return result;
    }

    // Try assistant events (second most frequent)
    auto a = assistantFromString(typeStr);
    if (a != static_cast<AssistantEvent>(-1)) {
        result.layer = MessageLayer::ASSISTANT;
        result.assistant = a;
        return result;
    }

    // Try shell events
    auto s = shellFromString(typeStr);
    if (s != static_cast<ShellEvent>(-1)) {
        result.layer = MessageLayer::SHELL;
        result.shell = s;
        return result;
    }

    result.layer = MessageLayer::UNKNOWN;
    qDebug() << "Unknown GUI message type:" << typeStr << "(not in any layer, ignored)";
    return result;
}

} // namespace GuiBusMessages
