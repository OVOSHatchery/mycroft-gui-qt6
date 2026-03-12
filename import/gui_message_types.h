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

#pragma once

#include <QLatin1String>

/**
 * GUI MessageBus protocol message type constants.
 *
 * These should be used instead of string literals to avoid typos and
 * ensure consistency across the codebase. Each constant corresponds to
 * the GUIMessageType enum in ovos-gui/message_types.py.
 *
 * Usage:
 *   if (message.type() == GUIMessageTypes::GUI_PAGE_SHOW) { ... }
 *   controller->sendRequest(GUIMessageTypes::GUI_BRIGHTNESS_SET, data);
 */
class GUIMessageTypes
{
public:
    // Connection & Lifecycle
    static constexpr QLatin1String OVOS_GUI_CONNECTED{"mycroft.gui.connected"};
    static constexpr QLatin1String OVOS_GUI_UNAVAILABLE{"mycroft.gui.unavailable"};

    // Namespace Management
    static constexpr QLatin1String GUI_PAGE_SHOW{"gui.page.show"};
    static constexpr QLatin1String GUI_PAGE_DELETE{"gui.page.delete"};
    static constexpr QLatin1String GUI_PAGE_DELETE_ALL{"gui.page.delete.all"};
    static constexpr QLatin1String GUI_CLEAR_NAMESPACE{"gui.clear.namespace"};
    static constexpr QLatin1String OVOS_GUI_SCREEN_CLOSE{"ovos.gui.screen.close"};
    static constexpr QLatin1String GUI_NAMESPACE_REMOVED{"gui.namespace.removed"};
    static constexpr QLatin1String GUI_NAMESPACE_DISPLAYED{"gui.namespace.displayed"};

    // Session Data
    static constexpr QLatin1String GUI_VALUE_SET{"gui.value.set"};
    static constexpr QLatin1String OVOS_SESSION_SET{"ovos.session.set"};
    static constexpr QLatin1String OVOS_SESSION_DELETE{"ovos.session.delete"};
    static constexpr QLatin1String OVOS_SESSION_LIST_INSERT{"ovos.session.list.insert"};
    static constexpr QLatin1String OVOS_SESSION_LIST_UPDATE{"ovos.session.list.update"};
    static constexpr QLatin1String OVOS_SESSION_LIST_MOVE{"ovos.session.list.move"};
    static constexpr QLatin1String OVOS_SESSION_LIST_REMOVE{"ovos.session.list.remove"};

    // Page Interaction
    static constexpr QLatin1String GUI_PAGE_INTERACTION{"gui.page_interaction"};
    static constexpr QLatin1String GUI_PAGE_GAINED_FOCUS{"gui.page_gained_focus"};

    // Status Events (forwarded to adapters)
    static constexpr QLatin1String OVOS_RECOGNIZER_LOOP_RECORD_BEGIN{"ovos.recognizer_loop.record_begin"};
    static constexpr QLatin1String OVOS_RECOGNIZER_LOOP_RECORD_END{"ovos.recognizer_loop.record_end"};
    static constexpr QLatin1String OVOS_RECOGNIZER_LOOP_UTTERANCE{"ovos.recognizer_loop.utterance"};
    static constexpr QLatin1String OVOS_RECOGNIZER_LOOP_WAKE_WORD{"ovos.recognizer_loop.wake_word"};
    static constexpr QLatin1String OVOS_AUDIO_OUTPUT_START{"ovos.audio_output.start"};
    static constexpr QLatin1String OVOS_AUDIO_OUTPUT_END{"ovos.audio_output.end"};
    static constexpr QLatin1String OVOS_SKILL_HANDLER_START{"ovos.skill.handler.start"};
    static constexpr QLatin1String OVOS_SKILL_HANDLER_ERROR{"ovos.skill.handler.error"};

    // Shell Features (Brightness)
    static constexpr QLatin1String GUI_BRIGHTNESS_SET{"gui.brightness.set"};
    static constexpr QLatin1String GUI_BRIGHTNESS_GET{"gui.brightness.get"};
    static constexpr QLatin1String GUI_BRIGHTNESS_AUTO_DIM_SET{"gui.brightness.auto_dim.set"};
    static constexpr QLatin1String GUI_BRIGHTNESS_NIGHT_MODE_SET{"gui.brightness.night_mode.set"};

    // Shell Features (Color Scheme)
    static constexpr QLatin1String GUI_COLOR_SCHEME_SET{"gui.color_scheme.set"};
    static constexpr QLatin1String GUI_COLOR_SCHEME_GET{"gui.color_scheme.get"};

    // Shell Features (Notifications)
    static constexpr QLatin1String GUI_NOTIFICATION_SET{"gui.notification.set"};
    static constexpr QLatin1String GUI_NOTIFICATION_CLEAR{"gui.notification.clear"};

    // Shell Features (Widgets)
    static constexpr QLatin1String GUI_WIDGET_DISPLAY{"gui.widget.display"};
    static constexpr QLatin1String GUI_WIDGET_REMOVE{"gui.widget.remove"};

    // Shell Features (Configuration)
    static constexpr QLatin1String GUI_CONFIG_LIST_GET{"gui.config.list.get"};
    static constexpr QLatin1String GUI_CONFIG_GET{"gui.config.get"};
    static constexpr QLatin1String GUI_CONFIG_SET{"gui.config.set"};

    // Events
    static constexpr QLatin1String GUI_EVENT_SEND{"gui.event.send"};
    static constexpr QLatin1String OVOS_EVENTS_TRIGGERED{"ovos.events.triggered"};

    // Legacy/Reserved
    static constexpr QLatin1String LEGACY_MYCROFT_GUI_AVAILABLE{"mycroft.gui.available"};
    static constexpr QLatin1String LEGACY_MYCROFT_GUI_UNAVAILABLE{"mycroft.gui.unavailable"};
    static constexpr QLatin1String LEGACY_MYCROFT_SYSTEM_ACTIVE_SKILLS{"mycroft.system.active_skills"};
};
