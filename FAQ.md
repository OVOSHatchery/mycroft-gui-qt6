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

## See Also
- [QUICK_FACTS.md](QUICK_FACTS.md) — Machine-readable reference
- [AUDIT.md](AUDIT.md) — Known issues
- [SUGGESTIONS.md](SUGGESTIONS.md) — Improvement proposals
- [docs/INTEGRATION.md](docs/INTEGRATION.md) — Ecosystem integration details
