# FAQ — ovos-gui-qt6

## What is ovos-gui-qt6?
A native Qt6/KF6 application that renders UI namespaces for the OpenVoiceOS platform. It is the modern, monolithic successor to the legacy mycroft-gui project.

## How does it connect to OVOS?
Through the `ovos-legacy-mycroft-gui-plugin` adapter (WebSocket port 18181). This client speaks the mycroft GUI protocol, which remains the standard for all Qt-based OVOS displays.

## What is a "Namespace" in this context?
In `ovos-gui-qt6`, a **Namespace** represents the UI state and pages belonging to a specific skill or system service (e.g. `ovos.skill.weather`). This distinguishes the UI logic from the Python "Skill" logic running in the core.

## Is this application monolithic?
Yes. Unlike previous versions that required a separate QML plugin to be installed in system paths, all C++ logic, QML components, and system templates are bundled into a single `ovos-gui-app` binary. This simplifies deployment and ensures consistency.

## How do I launch shell mode?
Shell mode provides a full-screen, touch-optimized environment with a homescreen, quick settings, and notifications. Launch it with:

```bash
ovos-gui-app --shell
```

## Protocol Layers

### What protocol layers exist on the GUI WebSocket?
The protocol is divided into three layers managed by `GuiBusClient`:

1. **Wire (`WireMessage`)**: Core protocol for session data, page lists, and namespace management.
2. **Shell (`ShellEvent`)**: Smart display extensions like brightness, notifications, and media player controls.
3. **Assistant (`AssistantEvent`)**: Forwarded bus events (wakeword, speaking, etc.) wrapped in a `mycroft.gui.forward.assistant` envelope.

### How are assistant events handled?
State events (like `recognizer_loop:wakeword`) are wrapped by the server into a wire envelope. `GuiBusClient` extracts these and updates the global application state (e.g. `isListening`), which the UI uses to show animations.

## Troubleshooting

### Why is the screen empty?
- Ensure `ovos-gui` and the legacy-plugin adapter are running.
- Check that the client is connecting to the correct host/port (configurable via `GlobalSettings`).
- Verify that templates are resolving correctly; check `qrc:/system-templates/` in the logs.

### Can I override the built-in templates?
Yes. While templates are bundled, you can set the `OVOS_SYSTEM_TEMPLATES` environment variable to a local directory path to force the client to load QML files from disk instead of internal resources.

## See Also
- [QUICK_FACTS.md](QUICK_FACTS.md) — Technical summary.
- [AUDIT.md](AUDIT.md) — Current status and known issues.
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) — System design details.
