# OVOS GUI Wire Protocol Specification

**Version**: 3.0 (2026-03-12)
**Status**: Production
**Canonical Reference**: This document specifies the complete wire protocol between OVOS GUI clients (mycroft-gui-qt5, mycroft-gui-qt6) and the legacy-plugin adapter. Both Qt5 and Qt6 clients speak the identical protocol.

**Source of truth**: `import/guibusmessages.h` -- enum `GUIBusMessageType` with `fromString()`/`toString()` converters.

---

## Architecture

```
OVOS Core Message Bus (port 8181)
         |
         v
ovos-gui service (Python) -- manages namespaces, pages, session data
         |
         v
ovos-legacy-mycroft-gui-plugin -- Tornado WebSocket server
         |
         v  JSON over WebSocket (port 18181, /gui endpoint)
mycroft-gui-qt5 / mycroft-gui-qt6
```

**CRITICAL**: All wire protocol strings use the `"mycroft.*"` prefix. These are NOT renamed -- they are the protocol standard shared by all clients.

---

## Message Envelope

Every message is a JSON object with these fields:

```json
{
  "type": "mycroft.session.set",
  "namespace": "skill.id",
  "data": { ... },
  "context": {
    "session": {"session_id": "default"},
    "source": "ovos-gui"
  }
}
```

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `type` | string | Yes | Wire message identifier |
| `namespace` | string | Varies | Skill ID or `"mycroft.system.active_skills"` |
| `data` | object/array | Varies | Message payload |
| `context` | object | No | Routing metadata, session info |

Additional top-level fields appear on certain message types (e.g., `position`, `property`, `event_name`, `event_type`, `from`, `to`, `items_number`).

---

## Category 1: INITIALIZATION

### `mycroft.gui.connected`

**Direction**: Client --> Server
**Purpose**: Announce client presence after WebSocket connection established.
**Enum**: `GUI_CONNECTED` -- `guibusmessages.h:64`

```json
{
  "type": "mycroft.gui.connected",
  "data": {
    "gui_id": "uuid-string",
    "session_id": "default",
    "site_id": "uuid-or-name"
  },
  "context": {}
}
```

**Handler**: `OVOSController::registerView()` sends this when a view is registered and the socket is connected -- `ovoscontroller.cpp:347-351`.

After connecting, the client also sends `mycroft.skills.all_loaded` to check readiness -- `ovoscontroller.cpp:83`.

---

## Category 2: ACTIVE SKILLS MANAGEMENT

Active skills use `namespace: "mycroft.system.active_skills"` with the session list operations. A skill ID must be present in the active skills list before any session data or GUI pages for that namespace will be accepted.

### `mycroft.session.list.insert` (active_skills)

**Direction**: Server --> Client
**Purpose**: Add skill(s) to the active skills list.
**Enum**: `SESSION_LIST_INSERT` -- `guibusmessages.h:79`

```json
{
  "type": "mycroft.session.list.insert",
  "namespace": "mycroft.system.active_skills",
  "position": 0,
  "data": [
    {"skill_id": "mycroft.weather"},
    {"skill_id": "mycroft.timer"}
  ]
}
```

**Handler**: `abstractskillview.cpp:424-439`. Calls `ActiveSkillsModel::insertSkills()`.

### `mycroft.session.list.remove` (active_skills)

**Direction**: Server --> Client
**Purpose**: Remove skill(s) from the active skills list. Cleans up session data and translators.
**Enum**: `SESSION_LIST_REMOVE` -- `guibusmessages.h:80`

```json
{
  "type": "mycroft.session.list.remove",
  "namespace": "mycroft.system.active_skills",
  "position": 0,
  "items_number": 1
}
```

**Handler**: `abstractskillview.cpp:443-476`.

### `mycroft.session.list.move` (active_skills)

**Direction**: Server --> Client
**Purpose**: Reorder skills in the active skills list.
**Enum**: `SESSION_LIST_MOVE` -- `guibusmessages.h:81`

```json
{
  "type": "mycroft.session.list.move",
  "namespace": "mycroft.system.active_skills",
  "from": 2,
  "to": 0,
  "items_number": 1
}
```

**Handler**: `abstractskillview.cpp:478-496`.

---

## Category 3: SESSION DATA

Session data is a per-skill key-value dictionary kept synchronized between server and client. Values can be scalars (string, number, boolean) or lists (which become `SessionDataModel` instances).

### `mycroft.session.set`

**Direction**: Server --> Client (also Client --> Server)
**Purpose**: Set or replace key-value pairs in a skill's session data.
**Enum**: `SESSION_SET` -- `guibusmessages.h:74`

```json
{
  "type": "mycroft.session.set",
  "namespace": "mycroft.weather",
  "data": {
    "temperature": "28",
    "icon": "cloudy",
    "forecast": [
      {"day": "Monday", "temp": 25},
      {"day": "Tuesday", "temp": 22}
    ]
  }
}
```

If a value is an array of objects, it is automatically converted to a `SessionDataModel` (or the existing model is cleared and repopulated).

**Handler**: `abstractskillview.cpp:349-393`.

### `mycroft.session.delete`

**Direction**: Server --> Client (also Client --> Server)
**Purpose**: Remove a single key from a skill's session data.
**Enum**: `SESSION_DELETE` -- `guibusmessages.h:75`

```json
{
  "type": "mycroft.session.delete",
  "namespace": "mycroft.weather",
  "property": "temperature"
}
```

**Handler**: `abstractskillview.cpp:396-418`.

---

## Category 4: DATA MODEL LISTS

These operate on list-type values within a skill's session data (NOT the active skills list). Distinguished from active skills operations by `namespace` not being `"mycroft.system.active_skills"`.

### `mycroft.session.list.insert` (skill data)

**Direction**: Server --> Client
**Purpose**: Insert items into a list property, creating the list if it does not exist.
**Enum**: `SESSION_LIST_INSERT` -- `guibusmessages.h:79`

```json
{
  "type": "mycroft.session.list.insert",
  "namespace": "mycroft.weather",
  "property": "forecast",
  "position": 2,
  "data": [
    {"day": "Wednesday", "temp": 20},
    {"day": "Thursday", "temp": 18}
  ]
}
```

**Rules**: `data` must always be an array. Each item must have the same set of keys. New keys in subsequent inserts are ignored by the GUI.

**Handler**: `abstractskillview.cpp:636-670`.

### `mycroft.session.list.update`

**Direction**: Server --> Client
**Purpose**: Update values of existing items starting at `position`.
**Enum**: `SESSION_LIST_UPDATE` -- `guibusmessages.h:82`

```json
{
  "type": "mycroft.session.list.update",
  "namespace": "mycroft.weather",
  "property": "forecast",
  "position": 2,
  "data": [
    {"day": "Wednesday", "temp": 21}
  ]
}
```

**Handler**: `abstractskillview.cpp:673-707`.

### `mycroft.session.list.move` (skill data)

**Direction**: Server --> Client
**Purpose**: Move items within a list property.
**Enum**: `SESSION_LIST_MOVE` -- `guibusmessages.h:81`

```json
{
  "type": "mycroft.session.list.move",
  "namespace": "mycroft.weather",
  "property": "forecast",
  "from": 2,
  "to": 5,
  "items_number": 2
}
```

**Handler**: `abstractskillview.cpp:711-747`.

### `mycroft.session.list.remove` (skill data)

**Direction**: Server --> Client
**Purpose**: Remove items from a list property.
**Enum**: `SESSION_LIST_REMOVE` -- `guibusmessages.h:80`

```json
{
  "type": "mycroft.session.list.remove",
  "namespace": "mycroft.weather",
  "property": "forecast",
  "position": 2,
  "items_number": 5
}
```

**Handler**: `abstractskillview.cpp:751-783`.

---

## Category 5: PAGE RENDERING (GUI MODEL)

Each active skill has an associated list of QML page URLs. Pages use `SYSTEM:<TemplateName>.qml` for system templates or `file://` for custom QML.

### `mycroft.gui.list.insert`

**Direction**: Server --> Client
**Purpose**: Insert QML template page(s) for a skill.
**Enum**: `GUI_LIST_INSERT` -- `guibusmessages.h:68`

```json
{
  "type": "mycroft.gui.list.insert",
  "namespace": "mycroft.weather",
  "position": 0,
  "data": [
    {"url": "SYSTEM:Weather.qml"},
    {"url": "file:///path/to/custom.qml"}
  ]
}
```

`SYSTEM:` prefix resolves via `resolveSystemTemplate()` -- `abstractskillview.cpp:48-60`.

**Handler**: `abstractskillview.cpp:502-563`.

### `mycroft.gui.list.remove`

**Direction**: Server --> Client
**Purpose**: Remove page(s) from a skill's GUI list.
**Enum**: `GUI_LIST_REMOVE` -- `guibusmessages.h:69`

```json
{
  "type": "mycroft.gui.list.remove",
  "namespace": "mycroft.weather",
  "position": 0,
  "items_number": 1
}
```

**Handler**: `abstractskillview.cpp:567-594`.

### `mycroft.gui.list.move`

**Direction**: Server --> Client
**Purpose**: Reorder pages in a skill's GUI list.
**Enum**: `GUI_LIST_MOVE` -- `guibusmessages.h:70`

```json
{
  "type": "mycroft.gui.list.move",
  "namespace": "mycroft.weather",
  "from": 0,
  "to": 2,
  "items_number": 1
}
```

**Handler**: `abstractskillview.cpp:597-628`.

---

## Category 6: EVENTS

### `mycroft.events.triggered`

**Direction**: Both (Server --> Client and Client --> Server)
**Purpose**: Trigger a named event on a skill or system-wide.
**Enum**: `EVENTS_TRIGGERED` -- `guibusmessages.h:156`

```json
{
  "type": "mycroft.events.triggered",
  "namespace": "mycroft.weather",
  "event_name": "system.pick",
  "parameters": {"item": 3}
}
```

**Special events**:

- `page_gained_focus` -- Server requests a specific page to gain focus. `data.number` is the page index.
- `mycroft.gui.close.screen` -- Emits `activeSkillClosed()`.
- Events starting with `system.` are available to all skills.

**Client --> Server handler**: `AbstractSkillView::triggerEvent()` -- `abstractskillview.cpp:186-201`.
**Server --> Client handler**: `abstractskillview.cpp:789-833`.

---

## Category 7: FORWARDED EVENT WRAPPERS

State change events (Category 8) and server-pushed shell events are no longer sent as bare messages on the wire. Instead, they are wrapped in one of two forwarding envelopes. The Qt client extracts the inner `event_type` and `data` and dispatches them as if they were top-level messages.

### `mycroft.gui.forward.assistant`

**Direction**: Server --> Client
**Purpose**: Wraps forwarded OVOS bus status events (wakeword, speaking state, enclosure animations, speak, lifecycle, etc.).
**Enum**: `GUI_FORWARD_ASSISTANT` -- `guibusmessages.h`

```json
{
  "type": "mycroft.gui.forward.assistant",
  "event_type": "recognizer_loop:wakeword",
  "data": {}
}
```

The `event_type` field carries the original event type string (e.g., `recognizer_loop:wakeword`, `speak`, `enclosure.eyes.color`, `mycroft.stop.handled`). The `data` field carries the original event payload.

**Replaces**: The legacy adapter previously sent these events via `mycroft.events.triggered` (with `event_name` carrying the inner type). Bare assistant events on the wire are no longer supported -- the Qt client logs a warning if it receives one without the forwarding wrapper.

**Handler**: `ovoscontroller.cpp` -- `WIRE` case for `GUI_FORWARD_ASSISTANT`. Extracts `event_type` and dispatches to the appropriate state change handler.

### `mycroft.gui.forward.shell`

**Direction**: Server --> Client
**Purpose**: Wraps server-pushed shell feature events (notifications, widgets, color scheme pushes, etc.).
**Enum**: `GUI_FORWARD_SHELL` -- `guibusmessages.h`

```json
{
  "type": "mycroft.gui.forward.shell",
  "event_type": "gui.notification.set",
  "data": {
    "text": "Timer finished",
    "action": "skill.timer:dismiss",
    "style": "info"
  }
}
```

The `event_type` field carries the shell event type string (e.g., `gui.notification.set`, `gui.widget.display`, `gui.color_scheme.set`). The `data` field carries the original event payload.

**Note**: Bidirectional shell request/response messages (`gui.brightness.get`, `gui.brightness.set`, `gui.config.get`, etc.) are still sent as bare protocol messages since they are client-initiated request/response pairs, not server-pushed events.

**Handler**: `ovoscontroller.cpp` -- `WIRE` case for `GUI_FORWARD_SHELL`. Extracts `event_type` and dispatches to the appropriate shell feature handler.

---

## Category 8: STATE CHANGES

These messages inform the client of OVOS core state. They do NOT carry namespace or session data -- they update global UI state (listening indicator, speaking animation, etc.).

**IMPORTANT**: These events are now delivered inside a `mycroft.gui.forward.assistant` wrapper (see Category 7). The event types listed below appear as the `event_type` field within the wrapper, not as bare `type` values on the wire.

### Speech Recognition

| Wire String | Enum | Direction | Purpose |
|-------------|------|-----------|---------|
| `recognizer_loop:wakeword` | `RECOGNIZER_WAKEWORD` | Server-->Client | Wakeword detected, sets `isListening=true` |
| `recognizer_loop:record_begin` | `RECOGNIZER_RECORD_BEGIN` | Server-->Client | Recording started |
| `recognizer_loop:record_end` | `RECOGNIZER_RECORD_END` | Server-->Client | Recording ended, sets `isListening=false` |
| `recognizer_loop:recognition_unknown` | `SPEECH_RECOGNITION_UNKNOWN` | Server-->Client | Speech not recognized |
| `recognizer_loop:sleep` | `RECOGNIZER_SLEEP` | Server-->Client | Listener going to sleep |
| `recognizer_loop:wake_up` | `RECOGNIZER_WAKE_UP` | Server-->Client | Listener waking up |
| `mycroft.awoken` | `OVOS_AWOKEN` | Server-->Client | Listener fully awake |

### Audio Output

| Wire String | Enum | Direction | Purpose |
|-------------|------|-----------|---------|
| `recognizer_loop:audio_output_start` | `RECOGNIZER_AUDIO_OUTPUT_START` | Server-->Client | TTS playback started, sets `isSpeaking=true` |
| `recognizer_loop:audio_output_end` | `RECOGNIZER_AUDIO_OUTPUT_END` | Server-->Client | TTS playback ended, sets `isSpeaking=false` |
| `speak` | `SPEAK` | Server-->Client | TTS utterance queued |

### Skill Lifecycle

| Wire String | Enum | Direction | Purpose |
|-------------|------|-----------|---------|
| `mycroft.stop.handled` / `mycroft.stop` | `STOP_HANDLED` | Server-->Client | Skill stop confirmed |
| `complete_intent_failure` | `INTENT_FAILURE` | Server-->Client | All intents failed |
| `ovos.utterance.handled` | `UTTERANCE_HANDLED` | Server-->Client | Utterance processed by a skill |
| `ovos.utterance.cancelled` | `UTTERANCE_CANCELLED` | Server-->Client | Utterance processing cancelled |
| `mycroft.skill.handler.start` | `SKILL_HANDLER_START` | Server-->Client | Skill handler began executing |
| `mycroft.skill.handler.complete` | `SKILL_HANDLER_COMPLETE` | Server-->Client | Skill handler finished |

**Handlers**: `ovoscontroller.cpp:179-223`.

### User Interaction (Client --> Server)

| Wire String | Enum | Direction | Purpose |
|-------------|------|-----------|---------|
| `recognizer_loop:utterance` | `RECOGNIZER_UTTERANCE` | Client-->Server | Text input from GUI |

**Handler**: `OVOSController::sendText()` -- `ovoscontroller.cpp:332-335`.

---

## Category 9: NAMESPACE CLEARING

### `gui.clear.namespace`

**Direction**: Server --> Client
**Purpose**: Clear all session data and GUI pages for a skill namespace.
**Enum**: `CLEAR_NAMESPACE` -- `guibusmessages.h:86`

```json
{
  "type": "gui.clear.namespace",
  "data": {
    "namespace": "mycroft.weather"
  }
}
```

**Handler**: `ovoscontroller.cpp:229-237`. Forwarded to all registered views.

---

## Category 10: CORE LIFECYCLE

| Wire String | Enum | Direction | Purpose |
|-------------|------|-----------|---------|
| `mycroft.skills.all_loaded.response` | `SKILLS_LOADED_RESPONSE` | Server-->Client | Reports whether all skills are loaded (`data.status: true/false`). Sets `serverReady`. |
| `mycroft.ready` | `READY` | Server-->Client | OVOS core fully ready. Sets `serverReady=true`. |
| `screen.close.idle.event` | `SCREEN_CLOSE_IDLE_EVENT` | Server-->Client | Idle timeout -- return to homescreen. `data.skill_idle_event_id` identifies the skill. |

**Handlers**: `ovoscontroller.cpp:243-256`.

---

## Category 11: ENCLOSURE EVENTS

These control hardware LED/display animations on embedded devices.

### Eyes (13 messages)

| Wire String | Enum | Direction |
|-------------|------|-----------|
| `enclosure.eyes.on` | `ENCLOSURE_EYES_ON` | Server-->Client |
| `enclosure.eyes.off` | `ENCLOSURE_EYES_OFF` | Server-->Client |
| `enclosure.eyes.blink` | `ENCLOSURE_EYES_BLINK` | Server-->Client |
| `enclosure.eyes.narrow` | `ENCLOSURE_EYES_NARROW` | Server-->Client |
| `enclosure.eyes.look` | `ENCLOSURE_EYES_LOOK` | Server-->Client |
| `enclosure.eyes.color` | `ENCLOSURE_EYES_COLOR` | Server-->Client |
| `enclosure.eyes.level` | `ENCLOSURE_EYES_LEVEL` | Server-->Client |
| `enclosure.eyes.volume` | `ENCLOSURE_EYES_VOLUME` | Server-->Client |
| `enclosure.eyes.spin` | `ENCLOSURE_EYES_SPIN` | Server-->Client |
| `enclosure.eyes.timedspin` | `ENCLOSURE_EYES_TIMEDSPIN` | Server-->Client |
| `enclosure.eyes.reset` | `ENCLOSURE_EYES_RESET` | Server-->Client |
| `enclosure.eyes.setpixel` | `ENCLOSURE_EYES_SETPIXEL` | Server-->Client |
| `enclosure.eyes.fill` | `ENCLOSURE_EYES_FILL` | Server-->Client |

### Mouth (11 messages)

| Wire String | Enum | Direction |
|-------------|------|-----------|
| `enclosure.mouth.events.activate` | `ENCLOSURE_MOUTH_EVENTS_ACTIVATE` | Server-->Client |
| `enclosure.mouth.events.deactivate` | `ENCLOSURE_MOUTH_EVENTS_DEACTIVATE` | Server-->Client |
| `enclosure.mouth.talk` | `ENCLOSURE_MOUTH_TALK` | Server-->Client |
| `enclosure.mouth.think` | `ENCLOSURE_MOUTH_THINK` | Server-->Client |
| `enclosure.mouth.listen` | `ENCLOSURE_MOUTH_LISTEN` | Server-->Client |
| `enclosure.mouth.smile` | `ENCLOSURE_MOUTH_SMILE` | Server-->Client |
| `enclosure.mouth.viseme` | `ENCLOSURE_MOUTH_VISEME` | Server-->Client |
| `enclosure.mouth.viseme_list` | `ENCLOSURE_MOUTH_VISEME_LIST` | Server-->Client |
| `enclosure.mouth.reset` | `ENCLOSURE_MOUTH_RESET` | Server-->Client |
| `enclosure.mouth.text` | `ENCLOSURE_MOUTH_TEXT` | Server-->Client |
| `enclosure.mouth.display` | `ENCLOSURE_MOUTH_DISPLAY` | Server-->Client |

### Weather Display (1 message)

| Wire String | Enum | Direction |
|-------------|------|-----------|
| `enclosure.weather.display` | `ENCLOSURE_WEATHER_DISPLAY` | Server-->Client |

**Enum definitions**: `guibusmessages.h:117-148`.

---

## Category 12: SHELL FEATURES (Protocol Extensions)

These are protocol extensions for managing display hardware and shell configuration.

### Brightness (5 messages)

| Wire String | Enum | Direction |
|-------------|------|-----------|
| `gui.brightness.set` | `GUI_BRIGHTNESS_SET` | Client-->Server |
| `gui.brightness.get` | `GUI_BRIGHTNESS_GET` | Client-->Server |
| `gui.brightness.get.response` | `GUI_BRIGHTNESS_GET_RESPONSE` | Server-->Client |
| `gui.brightness.auto_dim.set` | `GUI_BRIGHTNESS_AUTO_DIM_SET` | Client-->Server |
| `gui.brightness.night_mode.set` | `GUI_BRIGHTNESS_NIGHT_MODE_SET` | Client-->Server |

### Color Scheme (3 messages)

| Wire String | Enum | Direction |
|-------------|------|-----------|
| `gui.color_scheme.set` | `GUI_COLOR_SCHEME_SET` | Both |
| `gui.color_scheme.get` | `GUI_COLOR_SCHEME_GET` | Client-->Server |
| `gui.color_scheme.get.response` | `GUI_COLOR_SCHEME_GET_RESPONSE` | Server-->Client |

### Notifications (2 messages)

| Wire String | Enum | Direction |
|-------------|------|-----------|
| `gui.notification.set` | `GUI_NOTIFICATION_SET` | Server-->Client |
| `gui.notification.clear` | `GUI_NOTIFICATION_CLEAR` | Client-->Server |

### Widgets (2 messages)

| Wire String | Enum | Direction |
|-------------|------|-----------|
| `gui.widget.display` | `GUI_WIDGET_DISPLAY` | Server-->Client |
| `gui.widget.remove` | `GUI_WIDGET_REMOVE` | Both |

### Configuration (5 messages)

| Wire String | Enum | Direction |
|-------------|------|-----------|
| `gui.config.list.get` | `GUI_CONFIG_LIST_GET` | Client-->Server |
| `gui.config.list.get.response` | `GUI_CONFIG_LIST_GET_RESPONSE` | Server-->Client |
| `gui.config.get` | `GUI_CONFIG_GET` | Client-->Server |
| `gui.config.get.response` | `GUI_CONFIG_GET_RESPONSE` | Server-->Client |
| `gui.config.set` | `GUI_CONFIG_SET` | Client-->Server |

### Hints (2 messages)

| Wire String | Enum | Direction |
|-------------|------|-----------|
| `gui.hints.get` | `GUI_HINTS_GET` | Client-->Server |
| `gui.hints.get.response` | `GUI_HINTS_GET_RESPONSE` | Server-->Client |

**Enum definitions**: `guibusmessages.h:160-189`.

---

## Message Categories (Enum)

The `GUIBusMessageCategory` enum classifies messages for routing -- `guibusmessages.h:562-568`:

| Category | Description | Routing |
|----------|-------------|---------|
| `INIT` | Connection negotiation | Handled by `OVOSController` |
| `PAGE_RENDERING` | QML template display | Forwarded to `AbstractSkillView` |
| `SESSION_DATA` | Skill data updates | Forwarded to `AbstractSkillView` |
| `STATE_CHANGE` | Status updates | Handled by `OVOSController` |
| `USER_INTERACTION` | Events from GUI to core | Forwarded to `AbstractSkillView` (events) or sent directly |

Shell feature extensions are categorized as `USER_INTERACTION` (client-initiated) or `SESSION_DATA` (server responses) -- `guibusmessages.h:634-671`.

---

## Connection Flow

```
1. Client opens WebSocket to ws://host:18181/gui
2. Client sends: mycroft.gui.connected {gui_id, session_id, site_id}
3. Client sends: mycroft.skills.all_loaded {} (check readiness)
4. Server responds: mycroft.skills.all_loaded.response {status: true}
   OR: mycroft.ready {}
5. Server sends active skills: mycroft.session.list.insert (namespace: mycroft.system.active_skills)
6. Server sends session data: mycroft.session.set
7. Server sends pages: mycroft.gui.list.insert
8. Normal operation: data updates, events, state changes
9. On disconnect: client clears all session data and active skills
```

---

## Error Handling

| Scenario | Handler | Behavior |
|----------|---------|----------|
| Invalid JSON | `onGuiSocketMessageReceived` | Log warning, skip message |
| Empty `type` | `onGuiSocketMessageReceived` | Log warning, skip message |
| Unknown message type | `fromString()` | Log debug, return invalid enum (-1) |
| Empty `namespace` | Per-handler | Log warning, return |
| Skill not in active list | `SESSION_SET`/`SESSION_DELETE` | Log warning, return |
| Invalid position | Per-handler | Log warning, return |
| Invalid `items_number` | Per-handler | Log warning, return |
| Socket disconnected | Send methods | Log warning, return without sending |

---

## Total Message Count

| Category | Count |
|----------|-------|
| Initialization | 1 |
| Active Skills | 3 (shared with session list ops) |
| Session Data | 6 |
| Forwarding Wrappers | 2 |
| Namespace Clearing | 1 |
| Page Rendering | 3 |
| Events | 1 |
| State Changes (speech) | 7 |
| State Changes (audio) | 3 |
| State Changes (lifecycle) | 8 |
| Enclosure Eyes | 13 |
| Enclosure Mouth | 11 |
| Enclosure Weather | 1 |
| User Interaction | 2 |
| Shell Features | 19 |
| **Total** | **~72 unique wire strings** |

---

## See Also

- `import/guibusmessages.h` -- Canonical enum and string conversion functions
- `import/ovoscontroller.cpp` -- State change and routing handlers
- `import/abstractskillview.cpp` -- Session data, GUI model, and event handlers
- `ovos-legacy-mycroft-gui-plugin` -- Server-side implementation
