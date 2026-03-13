# mycroft-gui-qt6 Documentation

## What Is This

`mycroft-gui-qt6` is a Qt6/KF6 QML plugin and desktop shell application that renders skill GUIs for the OpenVoiceOS voice assistant platform. It connects to ovos-gui (via the legacy-plugin adapter on port 18181) over a WebSocket and dynamically renders skill templates using structured session data.

QML skills import the module as:

```qml
import OVOS 1.0 as OVOS
```

The QML plugin library is installed to `${KDE_INSTALL_QMLDIR}/OVOS` -- `import/CMakeLists.txt:45`.

---

## Architecture Overview

```
OVOS Core (Python, port 8181)
    |
    v
ovos-gui service (Python)
    |
    v
ovos-legacy-mycroft-gui-plugin (Tornado WebSocket server, port 18181)
    |
    v  JSON over WebSocket
OVOSController (C++ singleton)  -- import/ovoscontroller.cpp:40
    |
    +---> AbstractSkillView     -- import/abstractskillview.cpp:71
    |         |
    |         +---> ActiveSkillsModel   -- import/activeskillsmodel.h:26
    |         +---> SessionDataMap      -- import/sessiondatamap.h:25
    |         +---> DelegatesModel      -- import/delegatesmodel.h
    |         +---> AbstractDelegate    -- import/abstractdelegate.h:58
    |
    +---> MediaService          -- import/mediaservice.h:35
```

All WebSocket messages flow through a single connection managed by `OVOSController`. Session data, GUI list, and event messages are forwarded to registered `AbstractSkillView` instances for processing -- `ovoscontroller.cpp:265-272`.

---

## Key Classes

| Class | File | Role |
|-------|------|------|
| `OVOSController` | `import/ovoscontroller.h:33` | Singleton WebSocket manager. Connects to legacy-plugin, routes messages, tracks speaking/listening state. |
| `AbstractSkillView` | `import/abstractskillview.h:33` | Per-view UI container. Manages `ActiveSkillsModel`, per-skill `SessionDataMap`, and `DelegatesModel`. Handles all session/GUI/event protocol messages. |
| `AbstractDelegate` | `import/abstractdelegate.h:58` | Base QML item for skill delegates. Exposes `sessionData`, padding/inset, timeout, background. |
| `ActiveSkillsModel` | `import/activeskillsmodel.h:26` | `QAbstractListModel` tracking active skill namespaces. Supports black/white lists. |
| `SessionDataMap` | `import/sessiondatamap.h:25` | `QQmlPropertyMap` subclass holding per-skill key-value data. Supports `insertAndNotify` and `clearAndNotify`. |
| `SessionDataModel` | `import/sessiondatamodel.h` | Model for list-type session data (e.g., playlist items, search results). |
| `DelegatesModel` | `import/delegatesmodel.h` | Maps skill IDs to loaded QML delegate components. |
| `MediaService` | `import/mediaservice.h:35` | Audio/video playback via Qt6 Multimedia. Uses `AudioProviderService` and `VideoProviderService`. Exposes spectrum data and transport controls. |
| `GlobalSettings` | `import/globalsettings.h` | Application settings (WebSocket address, etc.). |
| `FileReader` | `import/filereader.h` | Utility for reading local files from QML. |
| `GUIBusMessageType` | `import/guibusmessages.h:61` | Enum of all 70+ supported wire protocol message types with `fromString()`/`toString()` converters. |

---

## Qt6-Specific Design

### QML_ELEMENT Registration

All classes use the `QML_ELEMENT` macro instead of `qmlRegisterType()`:

```cpp
class OVOSController : public QObject {
    Q_OBJECT
    QML_ELEMENT  // ovoscontroller.h:36
    ...
};
```

This provides compile-time type checking and eliminates the need for manual registration in the plugin loader. The old `OVOSPlugin::registerTypes()` method is now a no-op -- `ovosplugin.cpp:69-76`.

### Build Dependencies

- **Qt 6.5+** with C++17 -- `CMakeLists.txt:5-6`
- **KDE Frameworks 6** (Kirigami, CoreAddons, GuiAddons, Config, ConfigWidgets, IconThemes, DBusAddons) -- `CMakeLists.txt:32-40`
- **Qt6 Multimedia** for `MediaService` (uses `QVideoSink` instead of Qt5's `QVideoWidget`) -- `mediaservice.h:22`
- Optional: Qt6::WebView, KF6Plasma, KF6KIO

### Environment Variables

| Variable | Purpose | Default |
|----------|---------|---------|
| `MYCROFT_GUI_HOST` | WebSocket server hostname | From `GlobalSettings` |
| `MYCROFT_GUI_PORT` | WebSocket server port | `18181` |
| `MYCROFT_GUI_TLS` | Enable TLS (`1` or `true`) | Disabled |
| `MYCROFT_GUI_TOKEN` | Authentication token for `/gui?token=` | Empty |
| `MYCROFT_SITE_ID` | Site identifier for multi-screen | Auto-generated UUID |
| `OVOS_SYSTEM_TEMPLATES` | Override path for system templates | Compiled-in default |

---

## Documentation Files

| File | Description |
|------|-------------|
| [PROTOCOL.md](PROTOCOL.md) | Complete wire protocol specification (all message types) |
| [ARCHITECTURE.md](ARCHITECTURE.md) | System design, data flow, threading model |
| [MIGRATION_FROM_LEGACY.md](MIGRATION_FROM_LEGACY.md) | What changed from legacy mycroft-gui |
| [COMPONENTS.md](COMPONENTS.md) | API reference for all classes and templates |
| [CODE_GUIDE.md](CODE_GUIDE.md) | Qt/C++ concepts for non-Qt developers |
| [INTEGRATION.md](INTEGRATION.md) | Ecosystem overview and deployment |
| [PORTING_GUIDE.md](PORTING_GUIDE.md) | Qt5 to Qt6 migration patterns |
| [SESSION_AND_SITE_ID.md](SESSION_AND_SITE_ID.md) | Multi-screen session/site ID docs |
| [QUICK_START.md](QUICK_START.md) | 5-minute setup guide |

---

## Tests

| Test | File | Purpose |
|------|------|---------|
| `servertest` | `autotests/servertest.cpp` | WebSocket server and connection lifecycle |
| `modeltest` | `autotests/modeltest.cpp` | Model data operations |
| `stresstest` | `autotests/stresstest.cpp` | Load testing |
| `message_routing_test` | `autotests/message_routing_test.cpp` | Protocol message routing |
| `qml_framework_components_test` | `autotests/qml_framework_components_test.cpp` | QML component loading |

```bash
cd build && cmake .. -DBUILD_TESTING=ON && make -j$(nproc) && ctest --verbose
```

---

## Related Projects

- **[ovos-gui](https://github.com/OpenVoiceOS/ovos-gui)** -- Central GUI service managing skill namespaces
- **[ovos-legacy-mycroft-gui-plugin](https://github.com/OpenVoiceOS/ovos-legacy-mycroft-gui-plugin)** -- Adapter bridge (port 18181) that this client connects to
- **[ovos-gui-api-client](https://github.com/OpenVoiceOS/ovos-gui-api-client)** -- Python library skills use to send templates
- **[mycroft-gui-qt5](../../../mycroft-gui-qt5/)** -- Qt5 sibling of this project (same protocol)

---

**Last Updated**: 2026-03-12
