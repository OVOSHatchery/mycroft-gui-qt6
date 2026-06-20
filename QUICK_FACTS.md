# QUICK FACTS — ovos-gui-qt6

| Field | Value |
|:------|:------|
| **Package Name** | `ovos-gui-qt6` |
| **Version** | 1.0.1 |
| **Language** | C++17 |
| **Qt Version** | 6.5.0+ |
| **KF6 Version** | 6.0+ |
| **CMake Version** | 3.24+ |
| **Build Mode** | **Monolithic** (Self-contained) |
| **Entry Point** | `ovos-gui-app` |
| **QML URI** | `OVOS.GUI 1.0` |
| **Protocol** | WebSocket (`ws://` or `wss://` for TLS) |
| **Default Port** | 18181 |

## Key Classes

| Class | File | Description |
|:------|:-----|:------------|
| `GuiBusClient` | `import/guibusclient.h` | Singleton managing WebSocket connection to the GUI bus. |
| `GuiNamespace` | `import/guinamespace.h` | Per-namespace GUI manager. |
| `GuiPage` | `import/guipage.h` | Base renderer for UI pages. |
| `MediaService` | `import/mediaservice.h` | Centralized audio/video playback handler. |
| `NamespaceModel` | `import/namespacemodel.h` | Model tracking the stack of active namespaces. |
| `NamespaceDataMap` | `import/namespacedatamap.h` | Per-namespace key-value reactive data store. |
| `PageModel` | `import/pagemodel.h` | Per-namespace list of loaded QML pages. |
| `GlobalSettings` | `import/globalsettings.h` | Persistent client configuration. |

## Configuration Environment Variables

| Variable | Default | Description |
|:---------|:--------|:------------|
| `MYCROFT_GUI_HOST` | `0.0.0.0` | GUI adapter WebSocket host |
| `MYCROFT_GUI_PORT` | `18181` | GUI adapter WebSocket port |
| `MYCROFT_GUI_TLS` | `0` | Enable TLS (1 or true for wss://) |
| `MYCROFT_GUI_TOKEN` | (empty) | Bearer token for authentication |
| `MYCROFT_SITE_ID` | `default` | Device identifier for multi-screen |
| `OVOS_SYSTEM_TEMPLATES` | (Internal) | Local override for bundled templates |

## Architecture

**CRITICAL**: The GUI client connects to the **GUI protocol adapter** (port 18181), NOT the private OVOS messagebus (port 8181).

```
OVOS Core (8181) -> ovos-gui service -> legacy-plugin (18181) -> ovos-gui-qt6
```

## Protocol

`ovos-gui-qt6` supports **72 message types** via WebSocket classified into Wire, Shell, and Assistant layers (see `import/guibusmessages.h`).

| Resource | Purpose |
|:---------|:--------|
| `import/guibusmessages.h` | Registry of all supported messages. |
| [docs/PROTOCOL.md](docs/PROTOCOL.md) | Full wire protocol specification. |
| [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) | System design and monolithic build details. |

## See Also

- [FAQ](FAQ.md)
- [docs/index.md](docs/index.md)
- [MAINTENANCE_REPORT.md](MAINTENANCE_REPORT.md)
