# FAQ — mycroft-gui-qt6

## What is mycroft-gui-qt6?
A Qt6 port of the OVOS Qt GUI client, providing 100% feature parity with the modernized mycroft-gui-qt5. Experimental but functional.

## How does it connect to OVOS?
Through the `ovos-legacy-mycroft-gui-plugin` adapter (WebSocket port 18181). This is the SAME adapter used by mycroft-gui-qt5 — both Qt5 and Qt6 clients connect the same way.

## What does "legacy" mean in the adapter name?
It refers to the protocol's Mycroft AI origins, not its current status. The mycroft gui protocol is the CURRENT, ACTIVE standard for all Qt GUI clients.

## Is Qt5 still maintained?
mycroft-gui-qt5 received a one-time modernization in Q1 2026 (C++17, TLS/SSL, auth tokens, memory leak fixes) but is deprecated and will not receive further updates. It remains functional for systems where Qt6 is unavailable.

## Will old Mycroft AI GUI binaries work?
No. Pre-OVOS `mycroft-gui` binaries are NOT compatible with modern OVOS. You must recompile from the current mycroft-gui-qt5 or mycroft-gui-qt6 source.

## Build requirements?
Qt 6.5+, KF6, CMake 3.22+, C++17 compiler. See QUICK_FACTS.md for full details.

## Is this production-ready?
Experimental. Functional and well-documented, but needs broader user testing before production deployment.

## Shell Mode

### What is shell mode?

Shell mode launches the GUI as a full-screen touch-optimized desktop environment (EGLFS) for embedded devices without a traditional desktop environment (window manager). It includes a homescreen, notifications, OSD, and quick settings — all features of the integrated shell absorbed from the separate ovos-shell package.

### How do I launch shell mode?

```bash
ovos-gui-app --shell
```

### When should I use shell mode?

Use shell mode for:
- Embedded devices without a desktop environment (Mycroft Mark 2, Raspberry Pi, etc.)
- Devices with integrated touchscreens
- Systems where the OVOS interface should be the entire user experience

Use regular `ovos-gui-app` for:
- Desktop/laptop systems with an existing desktop environment
- Development machines
- Running OVOS alongside other applications

### Does Qt6 shell mode have feature parity with Qt5?

Yes. Qt6 shell mode includes all features of the Qt5 shell: homescreen, widgets, notifications, OSD, and device controls. Qt6 provides improved performance and modern graphics rendering.

### What hardware works with shell mode?

Minimum: 512 MB RAM, 1 GHz CPU, touchscreen or pointer device. Modern embedded systems like Mycroft Mark 2 and Raspberry Pi 3+ work well. Qt6 requires 64-bit architecture.

## Protocol Layers

### What protocol layers exist on the GUI WebSocket?

Three layers, each with its own enum in `guibusmessages.h`:

1. **Wire (`WireMessage`)** — `mycroft.*` messages: session sync, GUI list operations, namespace management, and the new `FORWARD_ASSISTANT`/`FORWARD_SHELL` wrapper messages.
2. **Shell (`ShellEvent`)** — `gui.*` messages: hints, page interaction, connected status.
3. **Assistant (`AssistantEvent`)** — Forwarded bus events (wakeword, speaking state, etc.) wrapped inside a `mycroft.gui.forward.assistant` wire message with an `event_type` field.

### How are assistant events (wakeword, speaking state) sent to Qt clients?

They are wrapped in a `mycroft.gui.forward.assistant` wire message with an `event_type` field. Bare assistant events on the wire produce a warning — they must be wrapped explicitly. See `WireMessage::FORWARD_ASSISTANT` in `guibusmessages.h` and the `handleAssistantEvent()` private method in `ovoscontroller.cpp`.

### How do Qt clients request voice command hints?

Send `gui.hints.get` on the WebSocket. The adapter responds with `gui.hints.get.response` containing `{"hints": [...]}`.

## See Also
- [QUICK_FACTS.md](QUICK_FACTS.md) — Machine-readable reference
- [AUDIT.md](AUDIT.md) — Known issues
- [SUGGESTIONS.md](SUGGESTIONS.md) — Improvement proposals
- [docs/INTEGRATION.md](docs/INTEGRATION.md) — Ecosystem integration details
