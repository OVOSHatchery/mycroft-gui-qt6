# OVOS GUI Qt6 Architecture

## Overview

`ovos-gui-qt6` is a monolithic Qt6-based application designed to render UI namespaces for the OpenVoiceOS ecosystem. It utilizes a template-driven system controlled by structured JSON messages over a WebSocket connection.

---

## System Architecture

```
┌──────────────────────────────────────────────────────────┐
│                OVOS Core (Python, port 8181)              │
│  Skills emit gui messages --> ovos-gui service            │
│  ovos-gui manages namespaces, pages, namespace data       │
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
│              ovos-gui-qt6 (Monolithic App)               │
│                                                          │
│  ┌────────────────────────────────────────────────────┐  │
│  │          GuiBusClient (Singleton)                  │  │
│  │  guibusclient.h / guibusclient.cpp                 │  │
│  │  - Owns the single QWebSocket connection           │  │
│  │  - Routes state change messages locally            │  │
│  │  - Forwards namespace/GUI/event msgs to instances  │  │
│  │  - Tracks: speaking, listening, serverReady        │  │
│  └────────┬───────────────────────────────────────────┘  │
│           │ handleIncomingMessage()                       │
│           v                                              │
│  ┌────────────────────────────────────────────────────┐  │
│  │       GuiNamespace (per-view instance)             │  │
│  │  guinamespace.h / guinamespace.cpp                 │  │
│  │  - Owns NamespaceModel                             │  │
│  │  - Owns per-namespace NamespaceDataMap instances   │  │
│  │  - Handles all data/page/event protocol msgs       │  │
│  │  - Resolves SYSTEM: template URIs (Internal/Local) │  │
│  └────────┬───────────────────────────────────────────┘  │
│           │                                              │
│     ┌─────┼──────────┐                                   │
│     v     v          v                                   │
│  ┌──────┐ ┌────────┐ ┌──────────────┐                   │
│  │Namesp│ │Namespace│ │  PageModel   │                   │
│  │Model │ │DataMap  │ │(per-namesp)  │                   │
│  │      │ │(per-    │ │              │                   │
│  │      │ │namesp)  │ │  PageLoader  │                   │
│  └──────┘ └───┬────┘ └──────┬───────┘                    │
│               │             │                            │
│               v             v                            │
│          Namespace     GuiPage (QML)                     │
│          DataModel      (UI Item)                        │
│                                                          │
│  ┌────────────────────────────────────────────────────┐  │
│  │           MediaService (Singleton)                 │  │
│  │  mediaservice.h / mediaservice.cpp                 │  │
│  │  - Audio/Video playback via Qt6 Multimedia         │  │
│  │  - AudioProviderService + VideoProviderService     │  │
│  │  - Supports Bundled GUI Media Backends             │  │
│  └────────────────────────────────────────────────────┘  │
│                                                          │
│  QML Layer: Internal Resources (qrc:/system-templates/)  │
│  UI Framework: KF6::Kirigami                             │
└──────────────────────────────────────────────────────────┘
```

---

## Message Routing

`GuiBusClient::onMainSocketMessageReceived()` is the single entry point for all incoming WebSocket messages. It classifies messages into three layers defined in `guibusmessages.h`:

1. **WIRE Layer**: Core GUI protocol (data sync, page management).
2. **SHELL Layer**: Extended features (brightness, notifications).
3. **ASSISTANT Layer**: Forwarded bus events (wakeword, speaking state).

```
Incoming WebSocket message
    |
    v
Parse JSON, classify via GuiBusMessages::parseMessage()
    |
    ├── ASSISTANT Layer category --> handle in GuiBusClient
    │   (isSpeaking, isListening, serverReady, etc.)
    │
    ├── WIRE Layer: CLEAR_NAMESPACE --> forward to all instances
    │
    ├── WIRE/SHELL Layers (Data / Pages / Events)
    │   --> forward to registered GuiNamespace instances
    │
    └── UNKNOWN type --> log, check for namespace:event pattern
```

Inside `GuiNamespace::onGuiSocketMessageReceived()`:

```
Incoming message (forwarded from GuiBusClient)
    |
    ├── SESSION_SET --> update NamespaceDataMap for namespace
    ├── SESSION_DELETE --> remove key from NamespaceDataMap
    │
    ├── SESSION_LIST_INSERT (active_skills) --> NamespaceModel::insertNamespaces()
    ├── SESSION_LIST_REMOVE (active_skills) --> cleanup + removeRows()
    │
    ├── SESSION_LIST_INSERT (data model) --> NamespaceDataModel::insertData()
    ├── SESSION_LIST_UPDATE --> NamespaceDataModel::updateData()
    │
    ├── GUI_LIST_INSERT --> create PageLoaders, resolve URLs
    ├── GUI_LIST_REMOVE --> PageModel::removeRows()
    │
    └── EVENTS_TRIGGERED --> dispatch to GuiPage::guiEvent()
        ├── page_gained_focus --> focus specific page
        ├── mycroft.gui.close.screen --> emit closed()
        └── other --> broadcast to all pages in namespace
```

---

## Monolithic Build Structure

The application is built as a single executable without external shared library dependencies for its core logic:

- **OvosGuiCommon**: A static library (`.a`) containing all C++ logic.
- **Resource Bundling**: All system templates and assets are compiled into the binary using Qt's Resource System (`ovos.qrc`).
- **Namespace Resolution**: `SYSTEM:` URIs preferred resolution path is `qrc:/system-templates/`.

### Native Type Registration

QML types are registered directly in `application/main.cpp` using `qmlRegisterType` under the `OVOS.GUI 1.0` namespace. This eliminates the need for a separate QML plugin loader and ensures all types are available immediately upon application startup.

---

## Connection Lifecycle

```
                    start()
                      |
                      v
              ┌───────────────┐
              │  CONNECTING    │  open WebSocket
              │  (reconnect    │  exponential backoff
              │   active)      │
              └───────┬───────┘
                      |
              [connected]
                      |
                      v
              ┌───────────────┐
              │     OPEN       │  send mycroft.gui.connected
              │                │  receive protocol messages
              └───────┬───────┘
                      |
              [error or close]
                      |
                      v
              ┌───────────────┐
              │    CLOSED      │  clear namespace models
              │                │  start reconnect timer
              └───────┬───────┘
                      |
              [timer fires]
                      |
                      v
              (back to CONNECTING)
```

The reconnect mechanism uses **exponential backoff** (1s, 2s, 4s... up to 30s) to maintain a resilient connection without overloading the server -- `guibusclient.cpp`.

---

## Memory Management

- **GuiBusClient**: Singleton, application lifetime.
- **GuiNamespace**: Created by QML, registered with client. Auto-deregistered on destruction.
- **NamespaceDataMap**: Created per-namespace on demand. Destroyed when namespace is deactivated or socket disconnects.
- **PageLoader**: Owned by `PageModel`. Manages the lifecycle of the loaded QML component and triggers cache cleanup on destruction.
- **QTranslator**: Per-namespace, removed and deleted when namespace is removed.

---

## Template Resolution

When the server sends a page URL like `"SYSTEM:Weather.qml"`, it is resolved by `resolvePage()` in `guinamespace.cpp`:

1. If URL starts with `SYSTEM:`, strip prefix and call `resolveSystemTemplate()`.
2. `resolveSystemTemplate()` checks internal resources first (`qrc:/system-templates/`).
3. Checks `$OVOS_SYSTEM_TEMPLATES` environment variable for local overrides.
4. Non-SYSTEM URLs (e.g. `https://`, `file://`) are resolved via standard `QUrl`.

---

## Build Targets

| Target | Type | Description |
|--------|------|-------------|
| `OvosGuiCommon` | Static Library | Consolidated C++ logic and core models. |
| `ovos-gui-app` | Application | The monolithic GUI binary. |
| `demotest` | Test / Tool | Standalone tool to verify templates headlessly. |
| `servertest` | Test | Protocol and connection verification. |
| `modeltest` | Test | Data model and sync verification. |

---

## See Also

- [PROTOCOL.md](PROTOCOL.md) -- Wire protocol specification.
- [COMPONENTS.md](COMPONENTS.md) -- API reference for classes and QML items.
- [SESSION_AND_SITE_ID.md](SESSION_AND_SITE_ID.md) -- Multi-screen identifiers.
