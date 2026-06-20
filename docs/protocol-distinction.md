# Protocol Distinction: Mycroft Legacy vs OVOS Bus

This document clarifies the distinction between legacy Mycroft protocol and modern OVOS MessageBus events in the Qt6 GUI adapter.

## Background

The OpenVoiceOS GUI adapter was originally built for Mycroft AI and retains some legacy compatibility, but now uses the OVOS MessageBus for primary communication. This document clarifies which protocol is used for which types of messages.

## Message Type Origins

### Mycroft-Era Messages (Legacy but still used)

These message types originated from Mycroft AI and are still used for backward compatibility:

| Message Type | Purpose | Direction | Notes |
|--------------|---------|-----------|-------|
| `mycroft.gui.connected` | Client announces connection | Adapter → Core | Legacy format; modern: use `ovos.gui.connected` |
| `mycroft.gui.available` | Adapter available signal | Adapter → Core | Deprecated in favor of `mycroft.gui.connected` |
| `mycroft.skill.handler.start` | Skill handler executing | Core → Adapters | Still used in Mycroft-compatible contexts |
| `mycroft.recognizer_loop.*` | STT/voice events | Core → Adapters | Still prevalent in Mycroft systems |
| `mycroft.audio_output.*` | Audio playback events | Core → Adapters | Still prevalent in Mycroft systems |

### OVOS Bus Messages (Preferred for new code)

These are the modern OpenVoiceOS equivalents and should be used in new code:

| Message Type | Purpose | Direction | Replaces |
|--------------|---------|-----------|----------|
| `ovos.gui.connected` | Client connection announcement | Adapter → Core | `mycroft.gui.connected` |
| `ovos.session.*` | Session data operations | Bidirectional | `mycroft.session.*` |
| `ovos.recognizer_loop.*` | STT/voice events (OVOS) | Core → Adapters | `mycroft.recognizer_loop.*` |
| `ovos.audio_output.*` | Audio playback events (OVOS) | Core → Adapters | `mycroft.audio_output.*` |
| `ovos.skill.handler.*` | Skill execution events (OVOS) | Core → Adapters | `mycroft.skill.handler.*` |

### OVOS GUI-Specific Messages (New)

These are unique to the OVOS template-based GUI system:

| Message Type | Purpose | Direction |
|--------------|---------|-----------|
| `gui.page.show` | Display template page | Core → Adapter |
| `gui.page.delete` | Remove page from namespace | Skill → Core |
| `gui.value.set` | Update session data | Skill → Core |
| `gui.page_interaction` | User interaction event | Adapter → Core |
| `gui.brightness.set` | Control screen brightness | System → Adapter |
| `gui.notification.set` | Display system notification | System → Adapter |
| `gui.color_scheme.set` | Change theme/appearance | System → Adapter |
| `gui.widget.display` | Show custom widget | System → Adapter |
| `gui.config.*` | Configuration management | System ↔ Adapter |

## Implementation Strategy

### In Qt6 Adapter Code

**Old approach (avoid):**
```cpp
if (type == "mycroft.skill.handler.start") {
    // Handle skill start
}
```

**New approach (preferred):**
```cpp
#include "gui_message_types.h"

if (type == GUIMessageTypes::OVOS_SKILL_HANDLER_START) {
    // Handle skill start
}
```

### Message Flow

```
OVOS Core System
        ↓
    MessageBus (OVOS native protocol)
        ↓
   Qt6 GUI Adapter (ovoscontroller.cpp)
    - Parses incoming messages
    - Routes to template renderers
    - Sends user interactions back
        ↓
Qt6 GUI Renderer
    - Renders template pages
    - Displays notifications
    - Shows widgets
```

### Handling Both Protocols

The adapter must handle both legacy Mycroft messages (for backward compatibility) and modern OVOS messages. When both exist, OVOS messages take precedence:

```cpp
if (type == GUIMessageTypes::OVOS_SKILL_HANDLER_START ||
    type == "mycroft.skill.handler.start") {
    // Handle both old and new message types
    handleSkillStart(data);
}
```

## Forwarded Core Events

Status events from the OVOS core are **forwarded to adapters** to enable real-time UI updates:

### Voice/STT Events
- `ovos.recognizer_loop.wake_word` — Wakeword detected, show listening indicator
- `ovos.recognizer_loop.record_begin` — STT recording started
- `ovos.recognizer_loop.record_end` — STT recording ended
- `ovos.recognizer_loop.utterance` — User utterance captured

### Audio/Playback Events
- `ovos.audio_output.start` — Audio playback started
- `ovos.audio_output.end` — Audio playback ended

### Skill Execution Events
- `ovos.skill.handler.start` — Intent handler executing
- `ovos.skill.handler.error` — Intent handler failed

**These are NOT GUI-specific messages** — they're system status events that adapters can choose to visualize (e.g., show listening animation, disable input during skill execution).

## Configuration

In `mycroft.conf`, configure which message types to expect:

```json
{
  "gui": {
    "protocol": "ovos",
    "legacy_compat": true,
    "forwarded_events": [
      "ovos.recognizer_loop.*",
      "ovos.audio_output.*",
      "ovos.skill.handler.*"
    ]
  }
}
```

## See Also

- `[ovos-gui: message_types.py]` — Python `GUIMessageType` enum with complete definitions
- `[mycroft-gui-qt6: gui_message_types.h]` — C++ message type constants
- `[ovos-gui: docs/shell-features/index.md]` — Shell features and their messages
- `[ovos-gui: docs/adapter-development/bus-protocol.md]` — Complete MessageBus protocol reference
