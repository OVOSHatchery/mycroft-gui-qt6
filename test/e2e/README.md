# End-to-End Tests for mycroft-gui-qt6

Tests for the Qt6 GUI client using a mock protocol server, validating the WebSocket protocol implementation without requiring a full OVOS stack.

## Architecture

```
Test Suite
    ↓
Mock GUI Service (Python)
    ↓ ws://localhost:18181
Qt6 GUI Client (C++)
    ↓
Test Assertions
```

The mock service implements the legacy Mycroft GUI protocol (WebSocket on port 18181), allowing isolated testing of the Qt client's protocol handling.

## Running Tests

### Prerequisites

```bash
pip install pytest websockets
```

### Run All Tests

```bash
pytest test/e2e/ -v
```

### Run Specific Test

```bash
pytest test/e2e/test_connection.py::test_client_connects -v
```

### Run with Coverage

```bash
pytest test/e2e/ --cov=../../import --cov-report=html
```

## Test Scenarios

### 1. Connection Tests (`test_connection.py`)

**test_client_connects**: Client successfully connects and receives `mycroft.gui.connected` message

**test_connection_timeout**: Connection fails gracefully when server is unavailable

**test_multiple_clients**: Multiple GUI clients can connect simultaneously

### 2. Protocol Tests (`test_protocol.py`)

**test_gui_id_message**: Client receives and processes `mycroft.gui.list.insert` with skill ID

**test_session_data**: Client correctly handles `mycroft.session.set` messages

**test_message_types**: All standard message types are parsed without errors

### 3. Skill Integration Tests (`test_skills.py`)

**test_weather_skill**: Weather skill response displays correctly

**test_hello_world_skill**: Hello world skill response displays

**test_skill_switching**: Client switches between skills without errors

### 4. Error Handling Tests (`test_errors.py`)

**test_invalid_json**: Malformed JSON is handled gracefully

**test_connection_loss**: Client handles abrupt server disconnection

**test_reconnection**: Client can reconnect after losing connection

### 5. Load Tests (`test_load.py`)

**test_high_frequency_messages**: Server handles rapid message sequences

**test_large_session_data**: Large session data payloads are processed

**test_concurrent_connections**: Multiple clients under load

## Test Fixtures

Common fixtures are provided in `conftest.py`:

- `mock_service` - Mock GUI service instance
- `service_port` - Available port for service
- `client_connection` - Connected WebSocket client
- `weather_script` - Weather skill response script
- `hello_script` - Hello world skill response script

## Protocol Message Reference

### Client → Server

```json
{
  "type": "recognizer_loop:utterance",
  "data": {"utterance": "what's the weather"},
  "context": {}
}
```

### Server → Client

```json
{
  "type": "mycroft.gui.connected",
  "data": {"version": "1.0"},
  "context": {}
}
```

```json
{
  "type": "mycroft.gui.list.insert",
  "data": {
    "gui_id": "weather-skill",
    "skills": ["weather-skill"],
    "index": 0
  },
  "context": {}
}
```

```json
{
  "type": "mycroft.session.set",
  "data": {
    "weatherSkill": {
      "current": {
        "temperature": "72°F",
        "condition": "Sunny"
      }
    }
  },
  "context": {"skill_id": "weather-skill"}
}
```

## Debugging Tests

Enable verbose logging:

```bash
pytest test/e2e/ -vv --log-cli-level=DEBUG
```

Check mock service logs:

```bash
tail -f /tmp/mock-gui.log
```

Debug single test with breakpoint:

```bash
pytest test/e2e/test_connection.py::test_client_connects -vv -s --pdb
```

## Extending Tests

To add a new test scenario:

1. Create test file in `test/e2e/test_*.py`
2. Import fixtures from `conftest.py`
3. Define test function `test_*`
4. Use assertions to validate protocol behavior

Example:

```python
async def test_custom_skill(mock_service, client_connection):
    """Test custom skill response."""
    script = [
        {
            'type': 'mycroft.gui.list.insert',
            'data': {'gui_id': 'custom-skill', 'skills': ['custom-skill']},
            'context': {}
        }
    ]
    mock_service.load_script(script)
    
    # Send utterance
    await client_connection.send(json.dumps({
        'type': 'recognizer_loop:utterance',
        'data': {'utterance': 'test'},
        'context': {}
    }))
    
    # Verify response
    response = json.loads(await client_connection.recv())
    assert response['type'] == 'mycroft.gui.list.insert'
    assert response['data']['gui_id'] == 'custom-skill'
```

## CI Integration

Tests run automatically on pull requests via GitHub Actions. See `.github/workflows/test.yml`.

## Known Limitations

- Tests run in headless mode (no GUI rendering)
- Theme and shell plugins not tested (require KDE Framework setup)
- Audio/multimedia not tested
- Android/mobile modes not tested
