# SUGGESTIONS — mycroft-gui-qt6

## Priority 1: Correctness

1. **Add session data cleanup on disconnect** — Clear `m_skillData`, `m_activeSkillsModel`, and `m_translatorsForSkill` when WebSocket disconnects. Prevents stale state on reconnect.

2. **Migrate MediaService to Qt6 Multimedia** — Replace deprecated `QAbstractVideoSurface` with `QVideoSink`, `QAudioProbe` with `QAudioOutput`. Current code uses Qt5 APIs that are removed in Qt6.

3. **Add exponential backoff to reconnect timer** — Currently retries every 1s forever. Add backoff (1s, 2s, 4s, 8s... up to 30s max) to reduce CPU when server is unavailable.

## Priority 2: Testing

4. **Add E2E integration tests** — Create tests that connect to a real ovos-gui service + legacy-plugin adapter. Verify full message flow end-to-end.

5. **Add message ordering tests** — Verify behavior when messages arrive out of order (e.g., session data before skill activation).

6. **Regenerate `plugins.qmltypes`** — Run `qmlplugindump` against the built plugin to generate accurate type info for IDE completion.

## Priority 3: Protocol

7. **Formalize hints protocol server-side** — The `gui.hints.get` / `gui.hints.get.response` messages are defined in the Qt client but need corresponding handler in ovos-gui service and legacy-plugin adapter.

8. **Add protocol version negotiation** — Client currently sends `qt_version` in `mycroft.gui.connected` but there's no protocol version field. Adding one would enable graceful feature detection.

## Priority 4: Performance

9. **Batch session data updates** — When multiple `mycroft.session.set` messages arrive rapidly, batch them into a single QML property update cycle to reduce rendering overhead.

10. **Add message compression** — For large session data payloads, consider WebSocket per-message deflate extension.
