# Maintenance Report — mycroft-gui-qt6

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
