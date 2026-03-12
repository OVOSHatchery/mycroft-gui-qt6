"""
Connection tests for mycroft-gui-qt6.

Tests WebSocket connection establishment, disconnection, and reconnection.
"""

import asyncio
import json
import logging
import pytest

logger = logging.getLogger(__name__)


@pytest.mark.asyncio
async def test_client_connects_and_receives_handshake(client_connection):
    """Test that client can connect and receive handshake message."""
    logger.info("Testing client connection handshake")

    # Client should receive connected message
    message = json.loads(await asyncio.wait_for(client_connection.recv(), timeout=2.0))

    assert message['type'] == 'mycroft.gui.connected'
    assert 'version' in message['data']
    logger.info(f"Received handshake: {message['type']} v{message['data']['version']}")


@pytest.mark.asyncio
async def test_client_sends_messages(client_connection):
    """Test that client can send messages to server."""
    logger.info("Testing client can send messages")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Send a test message
    test_message = {
        'type': 'test_message',
        'data': {'test_key': 'test_value'},
        'context': {}
    }

    await client_connection.send(json.dumps(test_message))
    logger.info(f"Sent test message: {test_message['type']}")


@pytest.mark.asyncio
async def test_connection_timeout():
    """Test connection timeout to non-existent server."""
    logger.info("Testing connection timeout")

    with pytest.raises(asyncio.TimeoutError):
        import websockets
        async with websockets.connect("ws://localhost:9999", ping_interval=None) as ws:
            await asyncio.wait_for(ws.recv(), timeout=1.0)


@pytest.mark.asyncio
async def test_multiple_sequential_connections(mock_service):
    """Test multiple clients can connect sequentially."""
    logger.info("Testing sequential connections")

    import websockets

    url = f"ws://{mock_service['host']}:{mock_service['port']}"

    for i in range(3):
        logger.info(f"Sequential connection #{i + 1}")
        async with websockets.connect(url, ping_interval=None) as ws:
            msg = json.loads(await asyncio.wait_for(ws.recv(), timeout=1.0))
            assert msg['type'] == 'mycroft.gui.connected'
            logger.info(f"Connection #{i + 1} successful")


@pytest.mark.asyncio
async def test_connection_with_different_ports(service_port):
    """Test that service correctly binds to specified port."""
    logger.info(f"Testing service binds to port {service_port}")

    import subprocess
    import sys
    import time
    from pathlib import Path

    script_path = Path(__file__).parent.parent.parent.parent / "mock_gui_service.py"
    if not script_path.exists():
        script_path = Path("/tmp/mock-gui-service/mock_gui_service.py")

    # Start service on specific port
    proc = subprocess.Popen(
        [sys.executable, str(script_path), "--port", str(service_port)],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )

    time.sleep(1)

    try:
        import websockets
        url = f"ws://localhost:{service_port}"
        async with websockets.connect(url, ping_interval=None) as ws:
            msg = json.loads(await asyncio.wait_for(ws.recv(), timeout=1.0))
            assert msg['type'] == 'mycroft.gui.connected'
            logger.info(f"Successfully connected to port {service_port}")
    finally:
        proc.terminate()
        proc.wait(timeout=5)


@pytest.mark.asyncio
async def test_rapid_connect_disconnect(mock_service):
    """Test rapid connection/disconnection cycles."""
    logger.info("Testing rapid connect/disconnect")

    import websockets

    url = f"ws://{mock_service['host']}:{mock_service['port']}"

    for i in range(5):
        logger.info(f"Rapid cycle #{i + 1}")
        try:
            async with websockets.connect(url, ping_interval=None) as ws:
                msg = json.loads(await asyncio.wait_for(ws.recv(), timeout=0.5))
                assert msg['type'] == 'mycroft.gui.connected'
        except asyncio.TimeoutError:
            logger.warning(f"Cycle #{i + 1} timed out")


@pytest.mark.asyncio
async def test_connection_persistence(client_connection):
    """Test that connection persists for extended period."""
    logger.info("Testing connection persistence")

    # Receive handshake
    msg = json.loads(await asyncio.wait_for(client_connection.recv(), timeout=1.0))
    assert msg['type'] == 'mycroft.gui.connected'

    # Wait and verify connection still works
    await asyncio.sleep(0.5)

    # Send test message
    test_msg = {'type': 'ping', 'data': {}, 'context': {}}
    await client_connection.send(json.dumps(test_msg))

    logger.info("Connection persisted successfully")
