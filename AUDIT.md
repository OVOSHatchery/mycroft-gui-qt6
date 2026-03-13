# AUDIT — mycroft-gui-qt6

## Known Issues

### HIGH

1. **No E2E tests with real OVOS core** — All tests use mock WebSocket servers. No integration tests with actual ovos-gui service or legacy-plugin adapter.
   - Files: `autotests/servertest.cpp`, `autotests/stresstest.cpp`

2. **No session data cleanup on disconnect** — When WebSocket disconnects, session data maps and active skills model are not cleared, leading to stale state on reconnect.
   - File: `import/abstractskillview.cpp:53-55`

### MEDIUM

3. **Nested session data models not supported** — `SessionDataModel` only handles flat key-value pairs. Nested objects within list items are not recursively converted.
   - File: `import/abstractskillview.cpp` (TODO comment)

4. **No message ordering guarantees** — Messages from the server are processed as received. No sequence numbers or ordering protocol exists.

5. **Unlimited reconnect retries** — `m_reconnectTimer` retries every 1000ms indefinitely with no backoff. Could cause excessive CPU usage when server is down.
   - File: `import/ovoscontroller.cpp`

### LOW

6. **`plugins.qmltypes` may be stale** — Type info file was manually updated during rename but not regenerated via `qmlplugindump`. IDE completion may be inaccurate.
   - File: `import/plugins.qmltypes`

7. **MediaService uses deprecated Qt5 APIs** — `QAbstractVideoSurface`, `QAudioProbe` are removed in Qt6. The Qt6 MediaService needs migration to `QVideoSink` and `QAudioOutput`.
   - File: `import/mediaservice.h`

## Security

- WebSocket is unencrypted by default (`ws://`). TLS must be explicitly enabled via `MYCRYPT_GUI_TLS=1`.
- Auth token is sent as URL query parameter, visible in logs and network traces.
- No certificate validation when TLS is enabled.

## Technical Debt

- Wire protocol strings use `mycroft.*` prefix — historical artifact, not a bug. Changing would break all existing adapters.
- Some QML files still reference old Mycroft documentation URLs in comments.
- `gui_message_types.h` and `shellfeaturecontroller.h` were marked for deletion but may still exist in some branches.

## Last Audited

- **Date**: 2026-03-12
- **AI Model**: Claude Opus 4.6
- **Scope**: Full codebase review during Qt5/Qt6 sync and Mycroft→OVOS rename
