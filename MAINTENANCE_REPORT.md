# Maintenance Report — mycroft-gui-qt6

## 2026-03-13 — Monolithic Consolidation & Architectural Naming Cleanup

- **AI Model**: Gemini 2.0 Flash
- **Actions Taken**:
  - **Monolithic Refactoring**:
    - Eliminated shared library dependencies (`libmycroft-gui-qt6.so`, `libovosplugin.so`).
    - Consolidated all C++ logic into a single `OvosGuiCommon` static library linked directly into the `ovos-gui-app` and test binaries.
    - Bundled all system templates (25+ QML files) into the binary via `ovos.qrc`, ensuring standalone execution without external file dependencies.
    - Moved QML type registration from the C++ plugin directly into `application/main.cpp`.
  - **Comprehensive Naming Refactor**:
    - Renamed logical concepts for architectural accuracy: `Skill` -> **`Namespace`**, `Delegate` -> **`Page`**, `Controller` -> **`GuiBusClient`**.
    - Updated over 50 QML files to use the new `OVOS.GUI 1.0` namespace and updated property/signal conventions.
    - Renamed all core files (e.g., `abstractskillview.cpp` -> `guinamespace.cpp`).
  - **Stability & UI Fixes**:
    - Fixed namespace conflict in QML registration by shifting to `OVOS.GUI` URI.
    - Implemented missing `whiteList` property in `NamespaceModel` to support filtered views.
    - Resolved `QFile::open` compilation warnings by adding explicit return value checks.
    - Modernized `Idle.qml` by merging it with the `HomeScreen.qml` logic.
    - Added standalone `demotest` and `run_demos.sh` for headless UI verification.
- **Oversight**: HIGH — Verified via successful clean build and automated demo test execution.

## 2026-03-13 — UI Visibility & Navigation Improvements

- **AI Model**: Gemini 2.0 Flash
- **Actions Taken**:
  - **Enhanced Navigation**:
    - Made the **Back Button** in the footer visible on all platforms (previously mobile-only).
    - Added **Settings** and **About** actions to the main page toolbar for easier access on desktop.
  - **Verified Existing Assets**: Confirmed presence and functionality of `SettingsPage.qml`, `AboutPage.qml`, and `HintsPage.qml`.
- **Oversight**: HIGH — Visual verification of QML logic changes.

## 2026-03-13 — Local Demos & Template Modernization

- **AI Model**: Gemini 2.0 Flash
- **Actions Taken**:
  - **Template Cleanup**: Dropped the non-canonical `Color.qml` system template.
  - **Enhanced Local Demos**:
    - Added `/demo3` command for music playback demonstration (`AudioPlayer`).
    - Added `/demo4` command for video playback demonstration (`VideoPlayer`).
    - Implemented `OVOSController::startDemoMedia(type)` to handle targeted media demos.
  - **Ported Legacy Components**:
    - Ported and modernized `HomeScreen.qml` from the legacy `ovos-skill-homescreen` repository.
    - Integrated `HomeScreen` into the multi-page local demo cycle (`/demo2`).
    - Added mock session data for `HomeScreen` in `OVOSController`.
  - **Documentation**: Updated `MAINTENANCE_REPORT.md` and `AUDIT.md`.
- **Oversight**: HIGH — Verified template removal and demo routing logic.

## 2026-03-13 — Audit Resolution & Stability Hardening

- **AI Model**: Gemini 2.0 Flash
- **Actions Taken**:
  - **Stability Hardening**:
    - Refactored `MediaService::mediaLoadUrl` to be fully asynchronous, eliminating UI-freezing `QEventLoop` calls during network validation.
    - Fixed memory leaks and null-pointer crashes in `AudioProviderService` by properly managing `QAudioSink` and `AudioStreamDevice` lifecycles.
    - Secured `OVOSController` singleton initialization using thread-safe C++11 magic statics.
  - **Architectural Cleanup**:
    - Centralized WebSocket connection management in `OVOSController`.
    - Removed redundant `m_guiWebSocket` and `m_reconnectTimer` from `AbstractSkillView`, simplifying the per-view logic.
    - Mitigated session data race conditions in `SessionDataMap` by ensuring local updates are cleared when the server sends fresh data.
  - **Feature Parity (Qt5 -> Qt6)**:
    - Restored `application/shell` and `theme` components in the build.
    - Fixed KF6 compatibility issues in `OpenVoiceStyleFactory` and `OvosTheme` (namespace migrations to `Kirigami::Platform`).
    - Resolved build dependencies for `openvoiceshellplugin` (linked `KF6::CoreAddons`).
    - Fixed installation error by re-enabling `application/shell/qmldir`.
  - **Documentation**:
    - Updated `AUDIT.md`, `FAQ.md`, and `SUGGESTIONS.md` to reflect resolved status and improved stability.
- **Oversight**: HIGH — Verified via full compilation, installation, and successful launch of the GUI.

## 2026-03-12 — Three-Enum Refactoring + FORWARD_ASSISTANT Wrapping

- **AI Model**: Claude Opus 4.6
- **Actions Taken**:
  - Refactored `guibusmessages.h`: single `GUIBusMessageType` enum replaced with three enums (`WireMessage`, `ShellEvent`, `AssistantEvent`) + `ParsedMessage` struct + `parseMessage()` dispatcher
  - Refactored `ovoscontroller.cpp`: `switch(parsed.layer)` dispatch replaces flat if-chain
  - Extracted `handleAssistantEvent()` as private method
  - Added `FORWARD_ASSISTANT` and `FORWARD_SHELL` wire messages — assistant events now wrapped explicitly
  - Bare assistant events on wire produce a warning (no legacy compat, pre-release)
  - Added exponential backoff reconnect (1s to 30s cap)
  - CMake fixes: install target, `OVOS_SYSTEM_TEMPLATES_DIR`, QRC filename
  - Build passes, 12/12 message routing tests pass
- **Oversight**: Human-directed, interactive session

## 2026-03-12 — OVOS Rewrite: Mycroft→OVOS Rename + Full Documentation

- **AI Model**: Claude Opus 4.6
- **Actions Taken**:
  - Renamed all non-protocol Mycroft references to OVOS across entire codebase
    - C++ classes: MycroftController→OVOSController, MycroftPlugin→OVOSPlugin
    - QML module: `Mycroft 1.0` → `OVOS 1.0`, 75+ QML files updated
    - Removed Android support, AudioRec, RemoteStt, TTS/STT functionality
    - Removed deprecated /opt/mycroft/skills filesystem scraping
    - Rewrote HintsPage to use bus protocol (gui.hints.get/response)
    - Added gui.hints.get and gui.hints.get.response to guibusmessages.h
  - Fixed 14 bugs (3 fatal) from haiku's prior Qt6 port:
    - Fixed enclosure message early return that dropped all messages
    - Fixed dead mycroft.gui.port handling (per-skill sockets never connect)
    - Fixed inverted translator check (!contains → contains)
    - Added session/GUI/event forwarding from controller to views
    - Added gui.clear.namespace handling
    - Added TLS/auth support
  - Rewrote documentation suite:
    - `docs/index.md` — full overview
    - `docs/PROTOCOL.md` — canonical wire protocol spec (71 message types)
    - `docs/MIGRATION_FROM_LEGACY.md` — complete delta from legacy
    - `docs/ARCHITECTURE.md` — component diagrams and message flows
  - Created QUICK_FACTS.md, AUDIT.md, SUGGESTIONS.md
  - Fixed stale MycroftController references in 9 existing docs files
- **Oversight**: HIGH — human-directed, multi-session iterative review

## 2026-03-12 — Documentation Accuracy Fixes

- **AI Model**: Claude Opus 4.6
- **Actions Taken**:
  - Fixed inaccurate Qt5 comparison in INTEGRATION.md (was "unmaintained/abandoned/C++11", corrected to "deprecated but functional, modernized C++17")
  - Fixed adapter scope — clarified that BOTH Qt5 and Qt6 use the same legacy adapter plugin
  - Replaced "old protocol" / "legacy protocol" language with "mycroft gui protocol"
  - Added GUI History section explaining Mycroft AI → OVOS transition
  - Added incompatibility warning about pre-OVOS binaries
  - Documented ovos-media legacy QML situation
  - Fixed README.md adapter description and connection diagram
  - Fixed DEPRECATION_GUIDE.md "abandoned" reference and C++ comparison
  - Created FAQ.md and MAINTENANCE_REPORT.md (were missing)
- **Oversight**: HIGH — human-verified plan, corrections based on direct user feedback about inaccuracies
