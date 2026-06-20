# SUGGESTIONS — ovos-gui-qt6

## Priority 1: Stability & Features ("The Greatness Plan")

1. **Developer HUD (Heads-Up Display)** — Implement an overlay toggled via command (e.g. `/hud`) that displays real-time protocol messages, the active namespace stack, and GUI performance metrics.
   - Target: `application/main.qml`, `import/guibusclient.cpp`

2. **Recursive Namespace Data Models** — Enhance `SESSION_SET` handling in `GuiNamespace` to recursively convert nested data structures into reactive models, enabling deep QML bindings.
   - Target: `import/guinamespace.cpp`

3. **Dynamic Theming Engine** — Extend `GlobalSettings` to load an external `theme.json` and support the `gui.color_scheme.set` message for skill-specific UI overrides.
   - Target: `import/globalsettings.cpp`, `theme/ovostheme.cpp`

4. **Shared Element Transitions** — Standardize page transitions in `NamespaceView` to support smooth animations for shared assets (like album art) between different pages.
   - Target: `import/qml/NamespaceView.qml`

## Priority 2: Developer Experience (DX)

5. **Enhanced `demotest` CLI** — Update the `demotest` tool to accept arguments for testing specific templates (e.g. `./demotest --template Weather`) and add a file-system watcher for "Live Reload" during development.
   - Target: `autotests/demotest.cpp`

6. **IDE Autocompletion** — Regenerate `plugins.qmltypes` to match the new monolithic `OVOS.GUI 1.0` API, ensuring developers get accurate suggestions in QtCreator and VSCode.
   - Target: `import/plugins.qmltypes`

## Priority 3: Security

7. **Secure Secret Storage** — Migrate auth tokens and sensitive configurations from environment variables to the system keyring (via Secret Service API or similar).
   - Target: `import/guibusclient.cpp`, `import/globalsettings.cpp`

---

## Resolved Suggestions (2026-03-13)

- **Monolithic Consolidation**: Successfully eliminated shared library dependencies and bundled all resources.
- **Naming Cleanup**: Renamed `Skill` -> `Namespace`, `Controller` -> `GuiBusClient`, `Delegate` -> `Page` for architectural accuracy.
- **Asynchronous Media Loading**: Eliminated blocking network calls in `MediaService`.
- **Restored Shell Mode**: Fixed KF6 compatibility and re-enabled `application/shell` in the build.
- **Automated UI Demos**: Implemented `demotest` and `run_demos.sh` for headless verification.
