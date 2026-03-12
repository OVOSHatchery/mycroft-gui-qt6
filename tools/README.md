# Tools for mycroft-gui-qt6

Utility scripts and services for testing and development.

## Mock GUI Service

**File**: `mock_gui_service.py`

Standalone WebSocket server implementing the Mycroft GUI protocol for testing the Qt6 GUI client without requiring a full OVOS installation.

### Quick Start

```bash
python tools/mock_gui_service.py --script hello
```

### Options

- `--host HOST` - Bind to host (default: 0.0.0.0)
- `--port PORT` - Bind to port (default: 18181)
- `--script SCRIPT` - Load test script: `hello`, `weather`

### Usage in Tests

The mock service is automatically started by pytest fixtures when running E2E tests:

```bash
pytest test/e2e/ -v
```

### Protocol

The service implements the Mycroft GUI protocol (WebSocket on port 18181) with:
- Client connection handshake (`mycroft.gui.connected`)
- Skill list management (`mycroft.gui.list.insert`)
- Session data updates (`mycroft.session.set`)
- Voice utterance handling (`recognizer_loop:utterance`)

For full protocol documentation, see [test/e2e/README.md](../test/e2e/README.md).

### Scripted Responses

Pre-defined test scripts for simulating skill responses:

- `hello` - Hello world skill with greeting message
- `weather` - Weather skill with current conditions and forecast

Create custom scripts by modifying `mock_gui_service.py`:

```python
custom_script = [
    {
        'type': 'mycroft.gui.list.insert',
        'data': {'gui_id': 'my-skill', 'skills': ['my-skill']},
        'context': {}
    },
    {
        'type': 'mycroft.session.set',
        'data': {'mySkill': {'result': 'data'}},
        'context': {'skill_id': 'my-skill'}
    }
]
```

## Future Tools

This directory will contain additional testing and development utilities as the project grows.
