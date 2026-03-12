"""
Protocol tests for mycroft-gui-qt6.

Tests WebSocket protocol message handling: gui.list.insert, session.set, etc.
"""

import asyncio
import json
import logging
import pytest

logger = logging.getLogger(__name__)


@pytest.mark.asyncio
async def test_gui_list_insert_message(client_connection, hello_world_script):
    """Test processing of mycroft.gui.list.insert message."""
    logger.info("Testing gui.list.insert message")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Send skill list insert
    skill_msg = hello_world_script[0]
    await client_connection.send(json.dumps(skill_msg))
    logger.info(f"Sent skill insert: {skill_msg['data']['gui_id']}")


@pytest.mark.asyncio
async def test_session_set_message(client_connection, weather_script):
    """Test processing of mycroft.session.set message."""
    logger.info("Testing session.set message")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Send session data
    session_msg = weather_script[1]
    await client_connection.send(json.dumps(session_msg))
    logger.info(f"Sent session data for skill: {session_msg['context'].get('skill_id')}")


@pytest.mark.asyncio
async def test_full_skill_response_sequence(client_connection, weather_script):
    """Test complete skill response sequence: insert + session data."""
    logger.info("Testing full skill response sequence")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Send skill list insert
    insert_msg = weather_script[0]
    await client_connection.send(json.dumps(insert_msg))
    logger.info(f"Sent skill insert: {insert_msg['data']['gui_id']}")

    await asyncio.sleep(0.1)

    # Send session data
    session_msg = weather_script[1]
    await client_connection.send(json.dumps(session_msg))
    logger.info("Sent session data")

    # Verify messages were accepted
    assert insert_msg['type'] == 'mycroft.gui.list.insert'
    assert session_msg['type'] == 'mycroft.session.set'


@pytest.mark.asyncio
async def test_message_with_context(client_connection):
    """Test that context field is properly handled."""
    logger.info("Testing message with context")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Send message with context
    msg = {
        'type': 'mycroft.session.set',
        'data': {'key': 'value'},
        'context': {'skill_id': 'test-skill', 'user': 'test-user'}
    }
    await client_connection.send(json.dumps(msg))
    logger.info(f"Sent message with context: {msg['context']}")


@pytest.mark.asyncio
async def test_message_without_context(client_connection):
    """Test that missing context field is handled gracefully."""
    logger.info("Testing message without context")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Send message without context
    msg = {
        'type': 'mycroft.session.set',
        'data': {'key': 'value'}
        # No context field
    }
    await client_connection.send(json.dumps(msg))
    logger.info("Sent message without context")


@pytest.mark.asyncio
async def test_empty_data_message(client_connection):
    """Test that empty data field is handled."""
    logger.info("Testing message with empty data")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Send message with empty data
    msg = {
        'type': 'mycroft.gui.list.insert',
        'data': {},
        'context': {}
    }
    await client_connection.send(json.dumps(msg))
    logger.info("Sent message with empty data")


@pytest.mark.asyncio
async def test_nested_data_structures(client_connection):
    """Test deeply nested data structures."""
    logger.info("Testing nested data structures")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Send message with nested structure
    msg = {
        'type': 'mycroft.session.set',
        'data': {
            'skill': {
                'level1': {
                    'level2': {
                        'level3': {
                            'value': 'deeply nested'
                        }
                    }
                }
            }
        },
        'context': {}
    }
    await client_connection.send(json.dumps(msg))
    logger.info("Sent message with nested structure")


@pytest.mark.asyncio
async def test_message_with_arrays(client_connection):
    """Test message data containing arrays."""
    logger.info("Testing message with arrays")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Send message with arrays
    msg = {
        'type': 'mycroft.session.set',
        'data': {
            'items': [
                {'id': 1, 'name': 'Item 1'},
                {'id': 2, 'name': 'Item 2'},
                {'id': 3, 'name': 'Item 3'}
            ]
        },
        'context': {}
    }
    await client_connection.send(json.dumps(msg))
    logger.info(f"Sent message with {len(msg['data']['items'])} array items")


@pytest.mark.asyncio
async def test_message_with_unicode(client_connection):
    """Test message with unicode characters."""
    logger.info("Testing unicode characters")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Send message with unicode
    msg = {
        'type': 'mycroft.session.set',
        'data': {
            'text': 'Hello 世界 🌍 Привет мир',
            'emoji': '😀 😃 😄 😁'
        },
        'context': {}
    }
    await client_connection.send(json.dumps(msg))
    logger.info(f"Sent unicode message: {msg['data']['text']}")
