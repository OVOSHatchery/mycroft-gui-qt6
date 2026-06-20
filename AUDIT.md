# AUDIT — mycroft-gui-qt6

## Known Issues

### HIGH

1. **No E2E tests with real OVOS core** — All tests use mock WebSocket servers. No integration tests with actual ovos-gui service or legacy-plugin adapter.
   - Files: `autotests/servertest.cpp`, `autotests/stresstest.cpp`

### MEDIUM

2. **Nested namespace data models not supported** — `NamespaceDataModel` only handles flat key-value pairs. Nested objects within list items are not recursively converted to models, limiting reactivity for complex data structures.
   - File: `import/guinamespace.cpp` (logic in `WireMessage::SESSION_SET` handler)

3. **No message ordering guarantees** — Messages from the server are processed immediately upon receipt. No sequence numbers or ordering protocol exists to ensure `session.set` arrives before `page.show` if they were sent in quick succession.
   - File: `import/guibusclient.cpp:322`

## Security

- **Unencrypted Communication** — WebSocket is unencrypted (`ws://`) by default. `MYCROFT_GUI_TLS=1` is required for production but often overlooked.
- **Insecure Token Handling** — Auth tokens are passed via environment variables and can be exposed if logs capture the connection URL.
- **Missing Certificate Validation** — When TLS is enabled, there is no explicit verification of the server's certificate.

## Technical Debt

- **Legacy Prefixing** — All wire protocol strings still use the `mycroft.*` prefix (e.g., `mycroft.session.set`). While necessary for backwards compatibility, it reflects a "branding" lock-in at the protocol level.

## Resolved Issues (2026-03-13)

- **CRITICAL: Blocking Network Call on GUI Thread** — Refactored `MediaService::evaluateUrl` to be fully asynchronous using `QNetworkAccessManager` signals.
- **CRITICAL: Memory Leaks and Crashes in AudioProviderService** — Fixed object lifecycle management in `mediaPlay` and added null-checks in destructor.
- **CRITICAL: Race Condition in Singleton Pattern** — Implemented C++11 thread-safe static local pattern for `GuiBusClient::instance()`.
- **HIGH: Monolithic Refactor & Naming Mess** — Consolidated codebase into a single binary and renamed classes for architectural clarity (`Skill` -> `Namespace`, `Controller` -> `GuiBusClient`).
- **HIGH: Dead Build Components** — Fixed KF6 compatibility issues in `application/shell` and `theme` and re-enabled them in `CMakeLists.txt`.
- **LOW: Duplicate Reconnect Logic** — Removed redundant `m_guiWebSocket` and `m_reconnectTimer` from `GuiNamespace`, centralizing connection management in `GuiBusClient`.
- **LOW: Typo in Header Documentation** — Fixed "seconsa" -> "seconds" in `guipage.h`.
- **LOW: Silent Write-Back Race Condition** — Updated `NamespaceDataMap` to remove keys from pending update queues when updated by the server.
- **LOW: Compilation Warnings** — Added return value checks for `QFile::open`.

## Last Audited

- **Date**: 2026-03-13
- **AI Model**: Gemini 2.0 Flash
- **Scope**: Monolithic refactor and naming cleanup.
