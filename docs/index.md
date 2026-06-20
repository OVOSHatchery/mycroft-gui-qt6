# OVOS GUI Qt6 Documentation

## What Is This

`ovos-gui-qt6` is a self-contained Qt6/KF6 application that renders GUI namespaces for the OpenVoiceOS voice assistant platform. It connects to the `ovos-gui` service (via the legacy-plugin adapter on port 18181) over a WebSocket and dynamically renders UI pages using structured namespace data.

This project is a **monolithic application**, meaning all core logic is compiled directly into the binary. All system templates are bundled as internal resources, eliminating external file dependencies for core functionality.

QML namespaces import the module as:

```qml
import OVOS.GUI 1.0 as OVOS
```

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
GuiBusClient (C++ singleton)     -- import/guibusclient.cpp
    |
    +---> GuiNamespace           -- import/guinamespace.cpp
    |         |
    |         +---> NamespaceModel       -- import/namespacemodel.h
    |         +---> NamespaceDataMap     -- import/namespacedatamap.h
    |         +---> PageModel            -- import/pagemodel.h
    |         +---> GuiPage              -- import/guipage.h
    |
    +---> MediaService           -- import/mediaservice.h
```

All WebSocket messages flow through a single connection managed by `GuiBusClient`. Protocol messages (namespace data, page lists, and events) are forwarded to registered `GuiNamespace` instances for processing.

---

## Key Classes

| Class | File | Role |
|-------|------|------|
| `GuiBusClient` | `import/guibusclient.h` | Singleton WebSocket manager. Connects to the GUI bus, routes messages, and tracks assistant state (speaking/listening). |
| `GuiNamespace` | `import/guinamespace.h` | Container for a specific UI namespace. Manages the `NamespaceModel`, `NamespaceDataMap`, and `PageModel`. |
| `GuiPage` | `import/guipage.h` | Base QML item for all UI pages. Exposes `namespaceData`, layout properties, and lifecycle events. |
| `NamespaceModel` | `import/namespacemodel.h` | Tracks active namespaces and their display priority. Supports white-listing for targeted views. |
| `NamespaceDataMap` | `import/namespacedatamap.h` | Holds the key-value session data for a specific namespace, enabling reactive QML bindings. |
| `PageModel` | `import/pagemodel.h` | Manages the stack of loaded QML pages for a namespace. |
| `MediaService` | `import/mediaservice.h` | Handles audio/video playback via Qt6 Multimedia. Supports the "GUI Media Player Bundle Pattern" for native client-side rendering. |
| `GlobalSettings` | `import/globalsettings.h` | Manages persistent application settings like host, port, and auto-connect behavior. |
| `GuiBusMessages` | `import/guibusmessages.h` | Central registry and parser for the three protocol layers: Wire, Shell, and Assistant events. |

---

## Monolithic Design

Unlike previous versions that relied on a QML plugin system, this application uses a consolidated build:
- **No Shared Libraries**: All C++ logic is linked statically into the main executable.
- **Bundled Resources**: QML files, system templates, and assets are compiled into the binary via `ovos.qrc`.
- **Direct Registration**: QML types are registered manually in `main.cpp` under the `OVOS.GUI 1.0` namespace.

---

## Environment Variables

| Variable | Purpose | Default |
|----------|---------|---------|
| `MYCROFT_GUI_HOST` | WebSocket server hostname | From `GlobalSettings` |
| `MYCROFT_GUI_PORT` | WebSocket server port | `18181` |
| `MYCROFT_GUI_TLS` | Enable TLS (`1` or `true`) | Disabled |
| `MYCROFT_GUI_TOKEN` | Authentication token for connection | Empty |
| `MYCROFT_SITE_ID` | Site identifier for multi-screen | `default` |
| `OVOS_SYSTEM_TEMPLATES` | Path override for system templates | Internal Resources |

---

## Documentation Index

| File | Description |
|------|-------------|
| [PROTOCOL.md](PROTOCOL.md) | Complete wire protocol specification |
| [ARCHITECTURE.md](ARCHITECTURE.md) | Deep dive into system design and data flow |
| [COMPONENTS.md](COMPONENTS.md) | API reference for classes and templates |
| [SESSION_AND_SITE_ID.md](SESSION_AND_SITE_ID.md) | Multi-screen and session management |
| [INTEGRATION.md](INTEGRATION.md) | Deployment and ecosystem overview |

---

**Last Updated**: 2026-03-13  
**Status**: Architecture Modernized & Naming Refactored.
