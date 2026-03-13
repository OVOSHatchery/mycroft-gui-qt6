# Migration from Legacy mycroft-gui

This document describes what changed between the original Mycroft AI `mycroft-gui` repository and the OVOS `mycroft-gui-qt6` rewrite.

The Qt5 sibling (`mycroft-gui-qt5`) underwent the same logical changes. This document notes Qt6-specific additions where applicable.

---

## Removed Features

The following features from the original mycroft-gui have been completely removed:

### Android Support
- `QTextToSpeech` integration for Android TTS
- `Qt::AndroidExtras` module usage
- `RemoteStt.qml` -- remote speech-to-text from Android
- Android-specific build paths and manifests

### Audio Recording / Streaming
- `AudioRec` class -- audio recording and streaming to server
- Push-to-talk client functionality
- Remote TTS relay

### Skill Filesystem Scraping
- Direct filesystem access to scan for skill QML files
- Skills now provide templates via the WebSocket protocol, never via filesystem

### Per-Skill WebSocket Ports
- `mycroft.gui.port` message handler -- the original client negotiated a unique WebSocket port per GUI view
- The legacy-plugin adapter now serves everything on a single port (18181)
- All messages flow through one WebSocket connection managed by `OVOSController` -- `ovoscontroller.cpp:107`

### Per-Skill WebSocket Connections in AbstractSkillView
- The original `AbstractSkillView` opened a second WebSocket to a per-view port received from the server
- In the OVOS rewrite, `AbstractSkillView` receives messages forwarded from `OVOSController` via `handleIncomingMessage()` -- `abstractskillview.h:75`
- The `m_guiWebSocket` member still exists but is vestigial for the legacy-plugin architecture

### active_skill_request Handler
- The original client could request the server to activate a specific skill
- This handler has been removed; skill activation is server-driven only

### currentSkill / currentIntent Tracking
- The original client tracked `currentSkill` and `currentIntent` as properties
- Removed in favor of the `ActiveSkillsModel` which tracks skill ordering

### Enclosure Message Filtering
- The original client had filtering logic for enclosure messages based on device type
- All enclosure messages are now forwarded unconditionally by the protocol -- `guibusmessages.h:117-148`

### m_reannounceGuiTimer
- Timer that periodically re-announced the GUI to the server
- Removed; the reconnect timer handles connection recovery -- `ovoscontroller.cpp:94-107`

### HiveMind Protocol Flag
- The original client had a protocol flag for HiveMind connections
- Removed from the client; HiveMind integration is handled at the adapter layer

---

## Renamed (Same as Qt5)

### QML Module
- **Old**: `import Mycroft 1.0`
- **New**: `import OVOS 1.0 as OVOS`
- Install path: `${KDE_INSTALL_QMLDIR}/OVOS` -- `import/CMakeLists.txt:45`

### C++ Class Names

| Old Name | New Name | File |
|----------|----------|------|
| `MycroftController` | `OVOSController` | `import/ovoscontroller.h:33` |
| `MycroftPlugin` | `OVOSPlugin` | `import/ovosplugin.h` |
| All `Mycroft*` prefixed classes | `OVOS*` equivalent | Throughout |

### Build Artifacts
- Library: `mycroft-gui` --> `mycroft-gui-qt6` (main target) / `ovosplugin` (QML plugin)
- Application: `mycroft-gui-app` (unchanged name, different binary)

### Resource Files
- `mycroft.qrc` --> `ovos.qrc` -- `import/CMakeLists.txt:30`

---

## NOT Renamed (Wire Protocol)

The following are deliberately NOT renamed -- they are the protocol standard:

### All `mycroft.*` Message Strings
Every wire protocol string retains the `mycroft.*` prefix:
- `mycroft.gui.connected`
- `mycroft.gui.list.insert` / `remove` / `move`
- `mycroft.session.set` / `delete`
- `mycroft.session.list.insert` / `remove` / `move` / `update`
- `mycroft.events.triggered`
- `mycroft.stop` / `mycroft.stop.handled`
- `mycroft.ready`
- `mycroft.skills.all_loaded.response`
- `mycroft.awoken`
- `mycroft.skill.handler.start` / `complete`
- `mycroft.system.active_skills` (namespace)

See `import/guibusmessages.h` for the complete list.

### Environment Variables
All environment variables retain their original names:
- `MYCROFT_GUI_HOST`
- `MYCROFT_GUI_PORT`
- `MYCROFT_GUI_TLS`
- `MYCROFT_GUI_TOKEN`
- `MYCROFT_SITE_ID`

See `ovoscontroller.cpp:56-68` and `ovoscontroller.cpp:96-101`.

---

## Added Features (Same as Qt5 Plus Qt6-Specific)

### Common Additions (Shared with Qt5)

#### Type-Safe Message Routing
- `GUIBusMessageType` enum with 70+ values -- `guibusmessages.h:61-190`
- `fromString()` / `toString()` converter functions
- `GUIBusMessageCategory` enum for message classification
- Replaces error-prone string literal comparisons

#### gui.clear.namespace
- New message type for clearing entire skill namespaces -- `guibusmessages.h:86`
- Handler in `ovoscontroller.cpp:229-237`

#### Shell Feature Protocol Extensions
19 new message types for shell features:
- Brightness control (5 messages): `gui.brightness.*`
- Color scheme management (3 messages): `gui.color_scheme.*`
- Notifications (2 messages): `gui.notification.*`
- Widgets (2 messages): `gui.widget.*`
- Configuration UI (5 messages): `gui.config.*`
- Hints (2 messages): `gui.hints.*`

See `guibusmessages.h:159-189`.

#### Session/Site ID Support
- `sessionId` and `siteId` properties on `OVOSController` -- `ovoscontroller.h:46-47`
- Sent with `mycroft.gui.connected` for multi-screen deployments -- `ovoscontroller.cpp:349-350`
- `MYCROFT_SITE_ID` env var or auto-generated UUID -- `ovoscontroller.cpp:64-69`

#### TLS and Token Authentication
- `useTls` property -- `ovoscontroller.h:43`
- `authToken` property -- `ovoscontroller.h:44`
- WebSocket URL includes `?token=` when auth is configured -- `ovoscontroller.cpp:104`

#### SYSTEM: Template Resolution
- `SYSTEM:<TemplateName>.qml` URI scheme for server-side template references
- Resolution: `$OVOS_SYSTEM_TEMPLATES` env var --> compiled-in `OVOS_SYSTEM_TEMPLATES_DIR`
- `resolveSystemTemplate()` -- `abstractskillview.cpp:48-60`

#### Improved Session Data Cleanup
- Session data cleared on WebSocket disconnect -- `abstractskillview.cpp:89-96`
- Views auto-deregistered on destruction -- `ovoscontroller.cpp:343-345`

### Qt6-Specific Additions

#### QML_ELEMENT Macro
All QML-exported classes use `QML_ELEMENT` instead of `qmlRegisterType()`:
- `OVOSController` -- `ovoscontroller.h:36`
- `AbstractSkillView` -- `abstractskillview.h:36`
- `AbstractDelegate` -- `abstractdelegate.h:61`

The `OVOSPlugin::registerTypes()` method is now a no-op -- `ovosplugin.cpp:69-76`.

#### Qt6 Multimedia API
- `MediaService` uses `QVideoSink` (Qt6) instead of `QVideoWidget` (Qt5) -- `mediaservice.h:22`
- `AudioProviderService` and `VideoProviderService` adapted for Qt6 Multimedia API changes

#### C++17 Requirement
- `CMAKE_CXX_STANDARD 17` required (Qt5 used C++14) -- `CMakeLists.txt:6`

#### KDE Frameworks 6
- `KF6::Kirigami`, `KF6::CoreAddons`, etc. replace KF5 equivalents -- `CMakeLists.txt:32-40`
- ECM 6.0 required -- `CMakeLists.txt:28`

#### Qt6 Signal Connection Syntax
- Uses `QOverload<>` for overloaded signals (required in Qt6) -- `ovoscontroller.cpp:124`

---

## Summary Table

| Aspect | Legacy mycroft-gui | mycroft-gui-qt6 |
|--------|-------------------|-----------------|
| Qt version | Qt5 | Qt6.5+ |
| KDE Frameworks | KF5 | KF6 |
| C++ standard | C++14 | C++17 |
| QML module | `Mycroft 1.0` | `OVOS 1.0` |
| QML registration | `qmlRegisterType()` | `QML_ELEMENT` macro |
| Controller class | `MycroftController` | `OVOSController` |
| WebSocket ports | Per-view negotiated | Single port (18181) |
| Message routing | String comparisons | Enum-based (`GUIBusMessageType`) |
| Wire protocol | `mycroft.*` | `mycroft.*` (unchanged) |
| Android support | Yes | Removed |
| Audio recording | Yes | Removed |
| Shell features | None | 19 protocol extensions |
| TLS support | None | Yes |
| Multi-screen | None | session_id + site_id |
