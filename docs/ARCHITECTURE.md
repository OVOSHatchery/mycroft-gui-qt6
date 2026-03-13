# mycroft-gui-qt6 Architecture

## Overview

`mycroft-gui-qt6` is a Qt6-based GUI client for OpenVoiceOS. It renders skill UIs using a template-based system driven by WebSocket messages from the OVOS core stack.

---

## System Architecture

```
┌──────────────────────────────────────────────────────────┐
│                OVOS Core (Python, port 8181)              │
│  Skills emit gui messages --> ovos-gui service            │
│  ovos-gui manages namespaces, pages, session data         │
└──────────────────┬───────────────────────────────────────┘
                   │
                   v
┌──────────────────────────────────────────────────────────┐
│  ovos-legacy-mycroft-gui-plugin (Tornado, port 18181)    │
│  Translates OVOS bus messages <--> Qt WebSocket JSON     │
│  Serves /gui endpoint, supports multiple clients         │
└──────────────────┬───────────────────────────────────────┘
                   │ JSON over WebSocket
                   v
┌──────────────────────────────────────────────────────────┐
│              mycroft-gui-qt6 (This Project)              │
│                                                          │
│  ┌────────────────────────────────────────────────────┐  │
│  │          OVOSController (Singleton)                │  │
│  │  ovoscontroller.h:33 / ovoscontroller.cpp:40       │  │
│  │  - Owns the single QWebSocket connection           │  │
│  │  - Routes state change messages locally            │  │
│  │  - Forwards session/GUI/event msgs to views        │  │
│  │  - Tracks: isSpeaking, isListening, serverReady    │  │
│  └────────┬───────────────────────────────────────────┘  │
│           │ handleIncomingMessage()                       │
│           v                                              │
│  ┌────────────────────────────────────────────────────┐  │
│  │       AbstractSkillView (per-view instance)        │  │
│  │  abstractskillview.h:33 / abstractskillview.cpp:71 │  │
│  │  - Owns ActiveSkillsModel                         │  │
│  │  - Owns per-skill SessionDataMap instances         │  │
│  │  - Handles all session/GUI/event protocol msgs     │  │
│  │  - Resolves SYSTEM: template URIs                  │  │
│  └────────┬───────────────────────────────────────────┘  │
│           │                                              │
│     ┌─────┼──────────┐                                   │
│     v     v          v                                   │
│  ┌──────┐ ┌────────┐ ┌──────────────┐                   │
│  │Active│ │Session │ │DelegatesModel│                    │
│  │Skills│ │DataMap │ │(per-skill)   │                    │
│  │Model │ │(per-   │ │              │                    │
│  │      │ │skill)  │ │DelegateLoader│                    │
│  └──────┘ └───┬────┘ └──────┬───────┘                    │
│               │             │                            │
│               v             v                            │
│          SessionData   AbstractDelegate                  │
│          Model (lists)  (QML items)                      │
│                                                          │
│  ┌────────────────────────────────────────────────────┐  │
│  │           MediaService (Singleton)                 │  │
│  │  mediaservice.h:35                                 │  │
│  │  - Audio/Video playback via Qt6 Multimedia         │  │
│  │  - AudioProviderService + VideoProviderService     │  │
│  │  - Spectrum analysis, transport controls           │  │
│  └────────────────────────────────────────────────────┘  │
│                                                          │
│  QML Layer: system-templates/ + qml/ components          │
│  Uses KF6::Kirigami for UI primitives                    │
└──────────────────────────────────────────────────────────┘
```

---

## Message Routing

`OVOSController::onMainSocketMessageReceived()` -- `ovoscontroller.cpp:153` is the single entry point for all incoming WebSocket messages. It:

1. Parses JSON and converts `type` string to `GUIBusMessageType` enum via `GuiBusMessages::fromString()` -- `guibusmessages.h:207`
2. Handles **state change** messages directly (speaking, listening, stop, ready) -- `ovoscontroller.cpp:179-256`
3. Forwards **session data**, **page rendering**, and **event** messages to all registered `AbstractSkillView` instances -- `ovoscontroller.cpp:265-272`

```
Incoming WebSocket message
    |
    v
Parse JSON, convert type to enum
    |
    ├── STATE_CHANGE category --> handle in OVOSController
    │   (isSpeaking, isListening, serverReady, etc.)
    │
    ├── CLEAR_NAMESPACE --> forward to all views
    │
    ├── SESSION_DATA / PAGE_RENDERING / EVENTS_TRIGGERED
    │   --> forward to all views via handleIncomingMessage()
    │
    └── Unknown type --> log, check for skill:event pattern
```

Inside `AbstractSkillView::onGuiSocketMessageReceived()` -- `abstractskillview.cpp:327`:

```
Incoming message (forwarded from controller)
    |
    ├── SESSION_SET --> update SessionDataMap for skill
    ├── SESSION_DELETE --> remove key from SessionDataMap
    │
    ├── SESSION_LIST_INSERT (active_skills) --> ActiveSkillsModel::insertSkills()
    ├── SESSION_LIST_REMOVE (active_skills) --> cleanup + removeRows()
    ├── SESSION_LIST_MOVE (active_skills) --> moveRows()
    │
    ├── SESSION_LIST_INSERT (skill data) --> SessionDataModel::insertData()
    ├── SESSION_LIST_UPDATE --> SessionDataModel::updateData()
    ├── SESSION_LIST_MOVE (skill data) --> SessionDataModel::moveRows()
    ├── SESSION_LIST_REMOVE (skill data) --> SessionDataModel::removeRows()
    │
    ├── GUI_LIST_INSERT --> create DelegateLoaders, resolve URLs
    ├── GUI_LIST_REMOVE --> DelegatesModel::removeRows()
    ├── GUI_LIST_MOVE --> DelegatesModel::moveRows()
    │
    └── EVENTS_TRIGGERED --> dispatch to AbstractDelegate::guiEvent()
        ├── page_gained_focus --> focus specific delegate
        ├── mycroft.gui.close.screen --> emit activeSkillClosed()
        └── other --> broadcast to all skill delegates
```

---

## Data Flow: Skill Activation

```
Server                          Client
  |                               |
  |-- session.list.insert ------->|  Add skill to ActiveSkillsModel
  |   (active_skills)             |
  |                               |
  |-- session.set --------------->|  Populate SessionDataMap
  |   (skill data)                |
  |                               |
  |-- gui.list.insert ----------->|  Create DelegateLoader(s)
  |   (page templates)            |    resolve SYSTEM: URIs
  |                               |    load QML components
  |                               |
  |-- events.triggered ---------->|  page_gained_focus
  |   (focus page)                |    set focus on delegate
  |                               |
  |                               |  QML binds to SessionDataMap
  |                               |  properties, renders template
```

---

## Qt6-Specific Architecture Details

### Module Dependencies

```cmake
# Required (CMakeLists.txt:13-23)
Qt6::Core, Qt6::Qml, Qt6::Quick, Qt6::Network,
Qt6::WebSockets, Qt6::Multimedia, Qt6::Gui, Qt6::Widgets, Qt6::DBus

# KDE Frameworks 6 (CMakeLists.txt:32-40)
KF6::Kirigami, KF6::CoreAddons, KF6::GuiAddons,
KF6::Config, KF6::ConfigWidgets, KF6::IconThemes, KF6::DBusAddons

# Optional
Qt6::WebView, KF6::Plasma, KF6::KIO
```

### QML_ELEMENT Registration (vs Qt5 qmlRegisterType)

Qt5 required explicit registration in the plugin loader:

```cpp
// Qt5 (old)
void MycroftPlugin::registerTypes(const char *uri) {
    qmlRegisterSingletonType<MycroftController>(uri, 1, 0, "MycroftController", ...);
    qmlRegisterType<AbstractDelegate>(uri, 1, 0, "Delegate");
}
```

Qt6 uses the `QML_ELEMENT` macro in class declarations:

```cpp
// Qt6 (current) -- ovoscontroller.h:36, abstractskillview.h:36, abstractdelegate.h:61
class OVOSController : public QObject {
    Q_OBJECT
    QML_ELEMENT  // Automatically registered at compile time
    ...
};
```

The `OVOSPlugin::registerTypes()` method is now a no-op -- `ovosplugin.cpp:69-76`. Singletons are instantiated in `main.cpp` and registered via context properties.

### MediaService and Qt6 Multimedia

The `MediaService` class wraps Qt6 Multimedia APIs:

- Uses `QVideoSink` instead of Qt5's `QVideoWidget` -- `mediaservice.h:22`
- `AudioProviderService` handles audio playback with FFT spectrum analysis -- `mediaproviders/audioproviderservice.h`
- `VideoProviderService` handles video with `QVideoSink` integration -- `mediaproviders/videoproviderservice.h`
- Provider switching via `changeProvider()` -- `mediaservice.h:83`

### C++17 and Qt6 API Changes

- C++17 required (`CMAKE_CXX_STANDARD 17`) -- `CMakeLists.txt:6`
- `QOverload<>` used for overloaded signal connections -- `ovoscontroller.cpp:124`
- `qsizetype` replaces `int` in list property accessors -- `abstractdelegate.h:258`
- `geometryChange()` replaces `geometryChanged()` -- `abstractdelegate.h:207`

---

## Threading Model

- **Main thread**: Qt event loop handles all signal/slot connections, QML rendering, and WebSocket I/O
- **WebSocket**: Qt's internal thread pool manages socket I/O; messages are delivered to the main thread
- **QML rendering**: Qt's scene graph renderer runs on a dedicated render thread (GPU)
- **All model updates** happen on the main thread -- thread-safe by design

---

## Connection Lifecycle

```
                    start()
                      |
                      v
              ┌───────────────┐
              │  CONNECTING    │  open WebSocket
              │  (reconnect    │
              │   timer may    │
              │   be active)   │
              └───────┬───────┘
                      |
              [connected]
                      |
                      v
              ┌───────────────┐
              │     OPEN       │  send mycroft.gui.connected
              │                │  send mycroft.skills.all_loaded
              │                │  receive messages
              └───────┬───────┘
                      |
              [error or close]
                      |
                      v
              ┌───────────────┐
              │    CLOSED      │  clear session data
              │                │  start reconnect timer (1s)
              └───────┬───────┘
                      |
              [timer fires]
                      |
                      v
              (back to CONNECTING)
```

Reconnect timer: 1 second interval -- `ovoscontroller.cpp:94`.

---

## Memory Management

- `OVOSController`: Singleton, lives for application lifetime -- `ovoscontroller.cpp:40-47`
- `AbstractSkillView`: Created by QML, registered with controller. Auto-deregistered on destruction -- `ovoscontroller.cpp:343-345`
- `SessionDataMap`: Created per-skill on demand. Destroyed when skill is removed from active list or socket disconnects -- `abstractskillview.cpp:89-96`, `abstractskillview.cpp:467-473`
- `SessionDataModel`: Owned by parent `SessionDataMap`. Destroyed via `deleteLater()` when replaced or cleared
- `DelegateLoader`: Owned by `DelegatesModel`. Triggers QML component cache cleanup on destruction -- `abstractskillview.cpp:554`
- `QTranslator`: Per-skill, removed and deleted when skill is removed -- `abstractskillview.cpp:460-465`

---

## Template Resolution

When the server sends a page URL like `"SYSTEM:Weather.qml"`, it is resolved by `resolveDelegate()` -- `abstractskillview.cpp:62-69`:

1. If URL starts with `SYSTEM:`, strip prefix and call `resolveSystemTemplate()`
2. `resolveSystemTemplate()` checks `$OVOS_SYSTEM_TEMPLATES` env var first -- `abstractskillview.cpp:50-55`
3. Falls back to compiled-in `OVOS_SYSTEM_TEMPLATES_DIR` -- `abstractskillview.cpp:57-59`
4. Non-SYSTEM URLs are passed through `QUrl::fromUserInput()` (supports `file://` and remote URLs)

---

## Error Handling

| Layer | Strategy |
|-------|----------|
| WebSocket connection | Automatic reconnect with 1s timer |
| JSON parsing | Log warning, skip malformed messages |
| Missing namespace | Log warning, return without processing |
| Invalid position/count | Log warning, return without modification |
| Unknown message type | Log debug, ignore (allows protocol extensions) |
| QML template errors | Qt declarative engine logs and continues; broken template shows empty |
| Socket send while disconnected | Log warning, return without sending |

---

## Build Targets

| Target | Type | Output |
|--------|------|--------|
| `mycroft-gui-qt6` | Shared library | `libmycroft-gui-qt6.so` |
| `ovosplugin` | QML plugin library | Installed to `${KDE_INSTALL_QMLDIR}/OVOS` |
| `mycroft-gui-app` | Application | Desktop GUI executable |
| `servertest` | Test | WebSocket/connection tests |
| `modeltest` | Test | Data model tests |
| `stresstest` | Test | Load tests |
| `message_routing_test` | Test | Protocol routing tests |
| `qml_framework_components_test` | Test | QML component tests |

---

## See Also

- [PROTOCOL.md](PROTOCOL.md) -- Complete wire protocol specification
- [MIGRATION_FROM_LEGACY.md](MIGRATION_FROM_LEGACY.md) -- Changes from legacy mycroft-gui
- [COMPONENTS.md](COMPONENTS.md) -- API reference
- [SESSION_AND_SITE_ID.md](SESSION_AND_SITE_ID.md) -- Multi-screen identifiers
