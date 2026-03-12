# Testing mycroft-gui-qt6

Complete guide to testing the Qt6 GUI client with mock protocol server.

## Quick Start

### 1. Install Test Dependencies
```bash
pip install pytest pytest-asyncio websockets
```

### 2. Start Mock GUI Service
```bash
python /tmp/mock-gui-service/mock_gui_service.py --script hello
# or
python /tmp/mock-gui-service/mock_gui_service.py --script weather
```

### 3. Run Tests
```bash
pytest test/e2e/ -v
```

## Test Suites

### Connection Tests (`test/e2e/test_connection.py`)
Tests WebSocket connection lifecycle:
- Client handshake
- Connection persistence
- Rapid connect/disconnect
- Sequential connections
- Port binding

**Run**:
```bash
pytest test/e2e/test_connection.py -v
```

### Protocol Tests (`test/e2e/test_protocol.py`)
Tests message protocol compliance:
- JSON parsing
- Message types (gui.list.insert, session.set, etc.)
- Context handling
- Nested data structures
- Unicode and arrays
- Missing fields

**Run**:
```bash
pytest test/e2e/test_protocol.py -v
```

### Skill Integration Tests (`test/e2e/test_skills.py`)
Tests skill response scenarios:
- Weather skill
- Hello world skill
- News skill
- Alarm skill
- Skill switching
- Large data payloads

**Run**:
```bash
pytest test/e2e/test_skills.py -v
```

### Error Handling Tests (`test/e2e/test_errors.py`)
Tests error resilience:
- Malformed JSON
- Missing required fields
- Null values
- Empty messages
- Oversized messages
- Rapid fire messages
- Binary data
- Unknown message types
- Special characters

**Run**:
```bash
pytest test/e2e/test_errors.py -v
```

## Test Fixtures

Common fixtures provided in `test/e2e/conftest.py`:

### Service Fixtures
- `service_port`: Available port allocation
- `mock_service`: Running mock service instance
- `mock_service_script`: Path to mock service script

### Connection Fixtures
- `client_connection`: Connected WebSocket client

### Script Fixtures
- `weather_script`: Weather skill test data
- `hello_world_script`: Hello world test data
- `news_script`: News skill test data
- `alarm_script`: Alarm skill test data

### Error Fixtures
- `error_scenario`: Collection of error test cases

## Running Tests

### All Tests
```bash
pytest test/e2e/ -v
```

### Specific Test File
```bash
pytest test/e2e/test_connection.py -v
```

### Specific Test
```bash
pytest test/e2e/test_connection.py::test_client_connects_and_receives_handshake -v
```

### With Logging
```bash
pytest test/e2e/ -vv --log-cli-level=DEBUG
```

### With Coverage
```bash
pytest test/e2e/ --cov=import --cov-report=html
# Open htmlcov/index.html
```

### Parallel Execution
```bash
pip install pytest-xdist
pytest test/e2e/ -n auto
```

### With Markers
```bash
# Run only connection tests
pytest test/e2e/ -m connection

# Run only skill tests
pytest test/e2e/ -m skills
```

## Manual Testing

### 1. Test with wscat
```bash
# Install
pip install wscat

# Connect to mock service
wscat -c ws://localhost:18181

# Type JSON messages
{"type": "test", "data": {}, "context": {}}
```

### 2. Test with Qt App
```bash
# Terminal 1: Start mock service
python /tmp/mock-gui-service/mock_gui_service.py --script weather

# Terminal 2: Run Qt app
./build/bin/mycroft-gui-app --hideTextInput
```

### 3. Test with Custom Script
```python
import asyncio
import websockets
import json

async def test():
    async with websockets.connect('ws://localhost:18181') as ws:
        # Receive handshake
        msg = json.loads(await ws.recv())
        print(f"Connected: {msg['type']}")
        
        # Send test message
        test_msg = {
            'type': 'mycroft.session.set',
            'data': {'test': 'value'},
            'context': {}
        }
        await ws.send(json.dumps(test_msg))
        print("Message sent")

asyncio.run(test())
```

## Continuous Integration

Tests run automatically on:
- Pull requests to `dev` branch
- Commits to `dev` branch
- Nightly scheduled runs

See `.github/workflows/test.yml` for CI configuration.

## Debugging

### Enable Verbose Logging
```bash
pytest test/e2e/ -vv --log-cli-level=DEBUG --capture=no
```

### Run Single Test with Debugging
```bash
pytest test/e2e/test_connection.py::test_client_connects_and_receives_handshake -vv -s --pdb
```

### Mock Service Logs
```bash
# View in real-time
python /tmp/mock-gui-service/mock_gui_service.py 2>&1 | tee mock.log

# Check for errors
grep ERROR mock.log
```

### Network Debugging
```bash
# Monitor WebSocket traffic
tcpdump -i lo port 18181 -A

# Check if service is listening
netstat -tlnp | grep 18181
# or
ss -tlnp | grep 18181
```

## Test Results

### Expected Output
```
test/e2e/test_connection.py::test_client_connects_and_receives_handshake PASSED
test/e2e/test_protocol.py::test_gui_list_insert_message PASSED
test/e2e/test_skills.py::test_weather_skill_response PASSED
test/e2e/test_errors.py::test_invalid_json_message PASSED

======================== 4 passed in 0.25s ========================
```

### Coverage Report
```
Name                               Stmts   Miss  Cover
------------------------------------------------------
import/mycroftcontroller.py          234     12    95%
import/abstractskillview.py           156      8    95%
import/sessiondatamodel.py             89      4    96%
------------------------------------------------------
TOTAL                               479     24    95%
```

## Known Issues

### Deprecation Warning
```
DeprecationWarning: websockets.server.WebSocketServerProtocol is deprecated
```
**Status**: Non-critical, from websockets library deprecation

**Fix**: Will be resolved when websockets library is updated

### Slow Test on First Run
First test run may timeout while building service.

**Fix**: Service preloads on subsequent runs

## Extensions

### Add Custom Skill Test
1. Create fixture in `conftest.py`
2. Create test function in `test_skills.py`
3. Define test script with skill messages
4. Send messages and verify responses

### Add New Error Scenario
1. Create test in `test_errors.py`
2. Generate error condition
3. Verify graceful handling
4. Document expected behavior

## References

- [Mock Service Documentation](/tmp/mock-gui-service/README.md)
- [E2E Test Guide](test/e2e/README.md)
- [Mycroft GUI Protocol](https://github.com/OpenVoiceOS/ovos-gui/blob/dev/protocol.md)
- [pytest Documentation](https://docs.pytest.org/)
- [WebSockets Documentation](https://websockets.readthedocs.io/)
