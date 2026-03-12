# Maintenance Report — mycroft-gui-qt6

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
