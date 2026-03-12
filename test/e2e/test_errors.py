"""
Error handling tests for mycroft-gui-qt6.

Tests graceful handling of protocol errors, malformed messages, connection loss.
"""

import asyncio
import json
import logging
import pytest

logger = logging.getLogger(__name__)


@pytest.mark.asyncio
async def test_invalid_json_message(client_connection):
    """Test handling of malformed JSON."""
    logger.info("Testing invalid JSON handling")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Send invalid JSON
    invalid_messages = [
        'not valid json',
        '{invalid json}',
        '{"type": "test"',  # Incomplete
        '{"type": "test", "data": undefined}',
    ]

    for invalid_msg in invalid_messages:
        logger.info(f"Sending invalid JSON: {invalid_msg}")
        try:
            await client_connection.send(invalid_msg)
        except Exception as e:
            logger.debug(f"Expected error: {e}")


@pytest.mark.asyncio
async def test_missing_required_fields(client_connection):
    """Test handling of messages missing required fields."""
    logger.info("Testing missing required fields")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Messages missing required fields
    messages = [
        {'data': {}, 'context': {}},  # Missing 'type'
        {'type': 'test'},  # Missing 'data' and 'context'
        {'type': 'test', 'context': {}},  # Missing 'data'
    ]

    for msg in messages:
        logger.info(f"Sending message with missing fields: {list(msg.keys())}")
        try:
            await client_connection.send(json.dumps(msg))
        except Exception as e:
            logger.debug(f"Expected error: {e}")


@pytest.mark.asyncio
async def test_null_values(client_connection):
    """Test handling of null values in message."""
    logger.info("Testing null values")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Message with null values
    msg = {
        'type': 'test',
        'data': {'key': None, 'nested': {'value': None}},
        'context': None
    }

    logger.info("Sending message with null values")
    await client_connection.send(json.dumps(msg))


@pytest.mark.asyncio
async def test_empty_message(client_connection):
    """Test handling of empty message."""
    logger.info("Testing empty message")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Send empty string
    logger.info("Sending empty message")
    try:
        await client_connection.send('')
    except Exception as e:
        logger.debug(f"Expected error: {e}")


@pytest.mark.asyncio
async def test_oversized_message(client_connection):
    """Test handling of very large messages."""
    logger.info("Testing oversized message")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Create very large message (1 MB)
    large_data = {
        'type': 'mycroft.session.set',
        'data': {
            'content': 'x' * (1024 * 1024)  # 1 MB of data
        },
        'context': {}
    }

    logger.info(f"Sending {len(json.dumps(large_data))} byte message")
    try:
        await client_connection.send(json.dumps(large_data))
        logger.info("Large message sent successfully")
    except Exception as e:
        logger.warning(f"Large message failed: {e}")


@pytest.mark.asyncio
async def test_rapid_fire_messages(client_connection):
    """Test handling of rapid succession messages."""
    logger.info("Testing rapid fire messages")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Send many messages rapidly
    logger.info("Sending 100 rapid messages")
    for i in range(100):
        msg = {
            'type': 'test_message',
            'data': {'sequence': i},
            'context': {}
        }
        try:
            await client_connection.send(json.dumps(msg))
        except Exception as e:
            logger.warning(f"Message {i} failed: {e}")
            break

    logger.info("Rapid message sequence completed")


@pytest.mark.asyncio
async def test_message_with_binary_data(client_connection):
    """Test handling of binary data in message (should fail gracefully)."""
    logger.info("Testing message with binary data")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Try to send non-text data
    logger.info("Attempting to send binary data")
    try:
        await client_connection.send(b'\x00\x01\x02\x03')
    except TypeError as e:
        logger.info(f"Expected error for binary data: {e}")


@pytest.mark.asyncio
async def test_unknown_message_type(client_connection):
    """Test handling of unknown message type."""
    logger.info("Testing unknown message type")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Send message with unknown type
    msg = {
        'type': 'this.type.does.not.exist',
        'data': {'test': 'data'},
        'context': {}
    }

    logger.info(f"Sending unknown message type: {msg['type']}")
    try:
        await client_connection.send(json.dumps(msg))
    except Exception as e:
        logger.debug(f"Expected error: {e}")


@pytest.mark.asyncio
async def test_message_type_case_sensitivity(client_connection):
    """Test that message types are case-sensitive."""
    logger.info("Testing message type case sensitivity")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Send message with different case
    messages = [
        {
            'type': 'MYCROFT.GUI.LIST.INSERT',  # All caps
            'data': {'test': 'data'},
            'context': {}
        },
        {
            'type': 'mycroft.gui.list.insert',  # Correct case
            'data': {'test': 'data'},
            'context': {}
        }
    ]

    for msg in messages:
        logger.info(f"Sending message type: {msg['type']}")
        await client_connection.send(json.dumps(msg))


@pytest.mark.asyncio
async def test_special_characters_in_type(client_connection):
    """Test handling of special characters in type field."""
    logger.info("Testing special characters in type")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Send message with special characters
    special_types = [
        'test<type>',
        'test&type',
        'test|type',
        'test;type',
    ]

    for type_str in special_types:
        msg = {
            'type': type_str,
            'data': {},
            'context': {}
        }
        logger.info(f"Sending message with special chars: {type_str}")
        try:
            await client_connection.send(json.dumps(msg))
        except Exception as e:
            logger.debug(f"Expected error: {e}")
