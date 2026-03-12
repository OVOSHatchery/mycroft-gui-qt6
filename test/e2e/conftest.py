"""
Pytest fixtures for End-to-End tests of mycroft-gui-qt6.

Provides:
- Mock GUI service instance
- WebSocket client connections
- Pre-defined test scripts (weather, hello world, etc.)
- Port allocation
"""

import asyncio
import json
import logging
import socket
import subprocess
import sys
import time
from pathlib import Path
from typing import List, Dict, Any

import pytest
import websockets
from websockets.client import WebSocketClientProtocol

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)


def get_available_port() -> int:
    """Get an available port on localhost."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind(('', 0))
        s.listen(1)
        port = s.getsockname()[1]
    return port


@pytest.fixture
def service_port():
    """Get available port for mock service."""
    return get_available_port()


@pytest.fixture
def mock_service_script(service_port):
    """Path to mock service script."""
    # Look in tools directory first (preferred location)
    script_path = Path(__file__).parent.parent.parent.parent / "tools" / "mock_gui_service.py"
    if not script_path.exists():
        # Fallback to /tmp location for development
        script_path = Path("/tmp/mock-gui-service/mock_gui_service.py")
    return script_path


@pytest.fixture
async def mock_service(service_port, mock_service_script):
    """Start mock GUI service and return connection details."""
    if not mock_service_script.exists():
        pytest.skip(f"Mock service script not found at {mock_service_script}")

    logger.info(f"Starting mock service on port {service_port}")

    # Start service process
    proc = subprocess.Popen(
        [sys.executable, str(mock_service_script), f"--port", str(service_port)],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )

    # Wait for service to start
    time.sleep(1)

    if proc.poll() is not None:
        stdout, stderr = proc.communicate()
        pytest.fail(f"Mock service failed to start:\n{stderr}")

    yield {
        'host': 'localhost',
        'port': service_port,
        'process': proc
    }

    # Cleanup
    logger.info("Stopping mock service")
    proc.terminate()
    try:
        proc.wait(timeout=5)
    except subprocess.TimeoutExpired:
        proc.kill()
        proc.wait()


@pytest.fixture
async def client_connection(mock_service):
    """Connect WebSocket client to mock service."""
    host = mock_service['host']
    port = mock_service['port']
    url = f"ws://{host}:{port}"

    logger.info(f"Connecting client to {url}")

    # Retry connection
    for attempt in range(5):
        try:
            async with websockets.connect(url) as websocket:
                yield websocket
                return
        except Exception as e:
            logger.debug(f"Connection attempt {attempt + 1} failed: {e}")
            await asyncio.sleep(0.5)

    pytest.fail(f"Could not connect to mock service at {url}")


@pytest.fixture
def weather_script() -> List[Dict[str, Any]]:
    """Test script for weather skill response."""
    return [
        {
            'type': 'mycroft.gui.list.insert',
            'data': {
                'gui_id': 'weather-skill',
                'skills': ['weather-skill'],
                'index': 0
            },
            'context': {}
        },
        {
            'type': 'mycroft.session.set',
            'data': {
                'weatherSkill': {
                    'current': {
                        'temperature': '72°F',
                        'condition': 'Sunny',
                        'location': 'Test City'
                    },
                    'forecast': [
                        {
                            'day': 'Tomorrow',
                            'high': '75°F',
                            'low': '65°F',
                            'condition': 'Partly Cloudy'
                        }
                    ]
                }
            },
            'context': {'skill_id': 'weather-skill'}
        }
    ]


@pytest.fixture
def hello_world_script() -> List[Dict[str, Any]]:
    """Test script for hello world skill response."""
    return [
        {
            'type': 'mycroft.gui.list.insert',
            'data': {
                'gui_id': 'hello-world',
                'skills': ['hello-world'],
                'index': 0
            },
            'context': {}
        },
        {
            'type': 'mycroft.session.set',
            'data': {
                'helloWorld': {
                    'message': 'Hello from Mock GUI Service!',
                    'timestamp': time.time()
                }
            },
            'context': {'skill_id': 'hello-world'}
        }
    ]


@pytest.fixture
def news_script() -> List[Dict[str, Any]]:
    """Test script for news skill response."""
    return [
        {
            'type': 'mycroft.gui.list.insert',
            'data': {
                'gui_id': 'news-skill',
                'skills': ['news-skill'],
                'index': 0
            },
            'context': {}
        },
        {
            'type': 'mycroft.session.set',
            'data': {
                'news': {
                    'headlines': [
                        {
                            'title': 'Test Headline 1',
                            'source': 'Test News',
                            'time': 'Just now'
                        },
                        {
                            'title': 'Test Headline 2',
                            'source': 'Test News',
                            'time': '5 minutes ago'
                        }
                    ]
                }
            },
            'context': {'skill_id': 'news-skill'}
        }
    ]


@pytest.fixture
def alarm_script() -> List[Dict[str, Any]]:
    """Test script for alarm skill response."""
    return [
        {
            'type': 'mycroft.gui.list.insert',
            'data': {
                'gui_id': 'alarm-skill',
                'skills': ['alarm-skill'],
                'index': 0
            },
            'context': {}
        },
        {
            'type': 'mycroft.session.set',
            'data': {
                'alarms': {
                    'list': [
                        {
                            'id': 'alarm-1',
                            'time': '07:00 AM',
                            'label': 'Morning',
                            'enabled': True
                        },
                        {
                            'id': 'alarm-2',
                            'time': '09:00 PM',
                            'label': 'Bedtime',
                            'enabled': True
                        }
                    ]
                }
            },
            'context': {'skill_id': 'alarm-skill'}
        }
    ]


@pytest.fixture
def error_scenario() -> Dict[str, Any]:
    """Test scenario with malformed messages."""
    return {
        'invalid_json': 'not valid json',
        'missing_type': {'data': {}, 'context': {}},
        'null_data': {'type': 'test', 'data': None, 'context': {}},
        'empty_message': ''
    }
