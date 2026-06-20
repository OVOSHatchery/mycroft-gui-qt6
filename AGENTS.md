# mycroft-gui-qt6 — Agent Onboarding

**Qt6 GUI client for OpenVoiceOS.** A monolithic Qt6/C++17 application that renders OVOS GUI templates via WebSocket protocol (port 18181). AI-generated port from the abandoned Qt5 codebase.

## Key Files

| Path | Purpose |
|------|---------|
| `CMakeLists.txt` | Build system (Qt6 + KF6 + ECM) |
| `import/guibusclient.cpp` | WebSocket singleton, message routing |
| `import/guinamespace.cpp` | Per-view namespace handler |
| `application/main.cpp` | Entry point |
| `autotests/` | 5 test executables (demotest, servertest, modeltest) |

## Build

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
make -j$(nproc)
```

## Branch Model

`dev` = work branch, `master` = stable (org rule). Currently on `dev`.

## Dependencies

- Qt6 6.5+ (Core, Qml, Quick, Network, WebSockets, Multimedia, Gui, Widgets, DBus)
- KF6 6.0+ (Kirigami, CoreAddons, GuiAddons, Config, ConfigWidgets, IconThemes, DBusAddons)
- ECM 6.0+

## Architecture

```
Skills → ovos-gui → ovos-legacy-mycroft-gui-plugin (port 18181) → mycroft-gui-qt6
```

25 system templates, 23 OVOS bus message types, type-safe routing.

## Status

Experimental. 100% AI-generated Qt6 port. All tests pass. Full feature parity with Qt5 version.
