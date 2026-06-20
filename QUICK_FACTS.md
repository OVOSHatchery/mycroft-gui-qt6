# QUICK FACTS — mycroft-gui-qt6

| Field | Value |
|:------|:------|
| **Package Name** | `ovos-gui` (Qt6) |
| **Version** | 1.0.1 |
| **Language** | C++17 |
| **Qt Version** | 6.5.0+ |
| **KF6 Version** | 6.0+ |
| **CMake Version** | 3.24+ |
| **Entry Point** | `ovos-gui-app` (application binary) |
| **Module Path** | `import/OVOS` (QML module) |
| **Protocol** | WebSocket (`ws://` or `wss://` for TLS) |
| **Default Port** | 18181 |

## Key Classes

| Class | File | Description |
|:------|:-----|:------------|
| `OVOSController` | `import/ovoscontroller.h` | Singleton managing WebSocket connection to GUI adapter |
| `AbstractSkillView` | `import/abstractskillview.h` | Per-skill GUI view manager |
| `AbstractDelegate` | `import/abstractdelegate.h` | QML template renderer for skill pages |
| `MediaService` | `import/mediaservice.h` | Audio/video playback with spectrum analysis |
| `ActiveSkillsModel` | `import/activeskillsmodel.h` | Model tracking active skill namespaces |
| `SessionDataMap` | `import/sessiondatamap.h` | Per-skill key-value session data |
| `DelegatesModel` | `import/delegatesmodel.h` | Per-skill QML page list |
| `GlobalSettings` | `import/globalsettings.h` | Persistent configuration |

## Configuration Environment Variables

| Variable | Default | Description |
|:---------|:--------|:------------|
| `MYCROFT_GUI_HOST` | `0.0.0.0` | GUI adapter WebSocket host |
| `MYCROFT_GUI_PORT` | `18181` | GUI adapter WebSocket port |
| `MYCROFT_GUI_TLS` | `0` | Enable TLS (1 or true for wss://) |
| `MYCROFT_GUI_TOKEN` | (empty) | Bearer token for authentication |
| `MYCROFT_SITE_ID` | (auto UUID) | Device identifier for multi-screen |

## Architecture

**CRITICAL**: The GUI client connects to the **GUI protocol adapter** (port 18181), NOT the private OVOS messagebus (port 8181).

```
OVOS Core (8181) -> ovos-gui service -> legacy-plugin (18181) -> mycroft-gui-qt6
```

## Protocol

mycroft-gui-qt6 supports **71 message types** via WebSocket (see `import/guibusmessages.h`).

| Resource | Purpose |
|:---------|:--------|
| `import/guibusmessages.h` | Centralized enum of all 71 supported messages |
| `docs/PROTOCOL.md` | Full protocol specification |
| `docs/MIGRATION_FROM_LEGACY.md` | What changed from legacy mycroft-gui |
| `docs/ARCHITECTURE.md` | Architecture with component diagrams |

## See Also

- [FAQ](FAQ.md)
- [docs/index.md](docs/index.md)
- [docs/PROTOCOL.md](docs/PROTOCOL.md)
