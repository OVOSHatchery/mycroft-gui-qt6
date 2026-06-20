# OVOS GUI Wire Protocol Specification

**Version**: 4.0 (2026-03-13)
**Status**: Production
**Canonical Reference**: This document specifies the complete wire protocol between OVOS GUI clients (`ovos-gui-qt6`) and the `legacy-plugin` adapter.

**Source of truth**: `import/guibusmessages.h` -- Centralized registry of all protocol layers.

---

## Architecture

```
OVOS Core Message Bus (port 8181)
         |
         v
ovos-gui service (Python) -- manages namespaces, pages, namespace data
         |
         v
ovos-legacy-mycroft-gui-plugin -- Tornado WebSocket server
         |
         v  JSON over WebSocket (port 18181, /gui endpoint)
ovos-gui-qt6 (Monolithic App)
```

**CRITICAL**: All wire protocol strings use the `"mycroft.*"` prefix. These are NOT renamed -- they are the protocol standard shared by all clients.

---

## Message Envelope

Every message is a JSON object with these fields:

```json
{
  "type": "mycroft.session.set",
  "namespace": "ovos.skill.weather",
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
| `namespace` | string | Varies | Namespace ID or `"mycroft.system.active_skills"` |
| `data` | object/array | Varies | Message payload |
| `context` | object | No | Routing metadata, session info |

---

## Protocol Layers

The protocol is divided into three logical layers processed by `GuiBusClient::onMainSocketMessageReceived()`:

1.  **WIRE**: Core management (Initialization, Namespaces, Pages, Data Sync).
2.  **SHELL**: Extended features (Brightness, Notifications, Config).
3.  **ASSISTANT**: Forwarded bus events (Wakeword, Speaking, Listening).

---

## Category 1: INITIALIZATION (WIRE Layer)

### `mycroft.gui.connected`

**Direction**: Client --> Server
**Purpose**: Announce client presence after WebSocket connection established.

```json
{
  "type": "mycroft.gui.connected",
  "data": {
    "gui_id": "uuid-string",
    "session_id": "default",
    "site_id": "default"
  }
}
```

**Handler**: `GuiBusClient::registerNamespace()` sends this when an instance is registered and the socket is connected.

---

## Category 2: NAMESPACE MANAGEMENT (WIRE Layer)

Active namespaces use `namespace: "mycroft.system.active_skills"`. A namespace ID must be present in this list before data or pages for it are accepted.

### `mycroft.session.list.insert` (active_skills)

**Purpose**: Add namespace(s) to the active stack.

```json
{
  "type": "mycroft.session.list.insert",
  "namespace": "mycroft.system.active_skills",
  "position": 0,
  "data": [
    {"namespace_id": "ovos.skill.weather"}
  ]
}
```

**Handler**: `guinamespace.cpp` -> Calls `NamespaceModel::insertNamespaces()`.

---

## Category 3: NAMESPACE DATA (WIRE Layer)

Key-value dictionary kept synchronized between server and client.

### `mycroft.session.set`

**Purpose**: Set or replace key-value pairs in a namespace's data map.

```json
{
  "type": "mycroft.session.set",
  "namespace": "ovos.skill.weather",
  "data": {
    "temperature": "22°C",
    "condition": "Cloudy"
  }
}
```

**Handler**: `guinamespace.cpp`. Updates the `NamespaceDataMap`.

---

## Category 4: PAGE RENDERING (WIRE Layer)

### `mycroft.gui.list.insert`

**Purpose**: Insert QML pages for a namespace.

```json
{
  "type": "mycroft.gui.list.insert",
  "namespace": "ovos.skill.weather",
  "position": 0,
  "data": [
    {"url": "SYSTEM:Weather.qml"}
  ]
}
```

**Handler**: `guinamespace.cpp`. Resolves `SYSTEM:` URIs to `qrc:/system-templates/`.

---

## Category 5: FORWARDED EVENT WRAPPERS

To maintain a clean wire protocol, server-pushed events are wrapped in envelopes.

### `mycroft.gui.forward.assistant`

**Purpose**: Wraps forwarded OVOS bus status events (wakeword, speaking, etc.).

```json
{
  "type": "mycroft.gui.forward.assistant",
  "event_type": "recognizer_loop:wakeword",
  "data": {}
}
```

**Handler**: `GuiBusClient::handleAssistantEvent()`.

---

## Category 6: STATE CHANGES (ASSISTANT Layer)

| Event Type | Purpose |
|------------|---------|
| `recognizer_loop:wakeword` | Sets `isListening=true` |
| `recognizer_loop:record_end` | Sets `isListening=false` |
| `recognizer_loop:audio_output_start` | Sets `isSpeaking=true` |
| `recognizer_loop:audio_output_end` | Sets `isSpeaking=false` |

---

## Category 7: SHELL FEATURES (SHELL Layer)

Extended bidirectional features for smart displays.

- **Brightness**: `gui.brightness.get`, `gui.brightness.set`.
- **Notifications**: `gui.notification.set`, `gui.notification.clear`.
- **Player**: `gui.player.play`, `gui.player.pause`, `gui.player.seek`.

---

## Error Handling

| Scenario | Behavior |
|----------|----------|
| Invalid JSON | Log warning, skip message. |
| Unknown type | Log debug, classified as `UNKNOWN`. |
| Disconnected | Queue or drop (policy-dependent). |

---

## See Also

- `import/guibusmessages.h` -- Canonical enum definitions.
- `docs/ARCHITECTURE.md` -- System design and data flow.
