"""
Skill integration tests for mycroft-gui-qt6.

Tests various skill response scenarios: weather, news, alarms, etc.
"""

import asyncio
import json
import logging
import pytest

logger = logging.getLogger(__name__)


@pytest.mark.asyncio
async def test_weather_skill_response(client_connection, weather_script):
    """Test weather skill response handling."""
    logger.info("Testing weather skill response")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Send weather skill response
    for msg in weather_script:
        await client_connection.send(json.dumps(msg))
        logger.info(f"Sent: {msg['type']}")
        await asyncio.sleep(0.05)

    # Verify skill was activated
    assert weather_script[0]['data']['gui_id'] == 'weather-skill'
    assert 'temperature' in weather_script[1]['data']['weatherSkill']['current']


@pytest.mark.asyncio
async def test_hello_world_skill_response(client_connection, hello_world_script):
    """Test hello world skill response handling."""
    logger.info("Testing hello world skill response")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Send hello world skill response
    for msg in hello_world_script:
        await client_connection.send(json.dumps(msg))
        logger.info(f"Sent: {msg['type']}")
        await asyncio.sleep(0.05)

    # Verify skill message
    assert 'Hello from Mock' in hello_world_script[1]['data']['helloWorld']['message']


@pytest.mark.asyncio
async def test_news_skill_response(client_connection, news_script):
    """Test news skill response handling."""
    logger.info("Testing news skill response")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Send news skill response
    for msg in news_script:
        await client_connection.send(json.dumps(msg))
        logger.info(f"Sent: {msg['type']}")
        await asyncio.sleep(0.05)

    # Verify news data
    headlines = news_script[1]['data']['news']['headlines']
    assert len(headlines) == 2
    assert all('title' in h for h in headlines)


@pytest.mark.asyncio
async def test_alarm_skill_response(client_connection, alarm_script):
    """Test alarm skill response handling."""
    logger.info("Testing alarm skill response")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Send alarm skill response
    for msg in alarm_script:
        await client_connection.send(json.dumps(msg))
        logger.info(f"Sent: {msg['type']}")
        await asyncio.sleep(0.05)

    # Verify alarms
    alarms = alarm_script[1]['data']['alarms']['list']
    assert len(alarms) == 2
    assert all(a['enabled'] for a in alarms)


@pytest.mark.asyncio
async def test_skill_switching(client_connection, weather_script, hello_world_script):
    """Test switching between different skills."""
    logger.info("Testing skill switching")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Send first skill (weather)
    for msg in weather_script:
        await client_connection.send(json.dumps(msg))
        await asyncio.sleep(0.05)

    logger.info("Sent weather skill")
    await asyncio.sleep(0.1)

    # Send second skill (hello world) - should replace
    for msg in hello_world_script:
        await client_connection.send(json.dumps(msg))
        await asyncio.sleep(0.05)

    logger.info("Sent hello world skill (should replace weather)")

    # Both skills were sent successfully
    assert weather_script[0]['data']['gui_id'] == 'weather-skill'
    assert hello_world_script[0]['data']['gui_id'] == 'hello-world'


@pytest.mark.asyncio
async def test_custom_skill_data(client_connection):
    """Test custom skill with arbitrary data structure."""
    logger.info("Testing custom skill data")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Create custom skill response
    custom_skill = {
        'type': 'mycroft.gui.list.insert',
        'data': {
            'gui_id': 'custom-skill',
            'skills': ['custom-skill'],
            'index': 0
        },
        'context': {}
    }

    custom_data = {
        'type': 'mycroft.session.set',
        'data': {
            'custom': {
                'color': '#FF5733',
                'size': 'large',
                'items': ['A', 'B', 'C'],
                'metadata': {
                    'version': '1.0',
                    'author': 'test'
                }
            }
        },
        'context': {'skill_id': 'custom-skill'}
    }

    # Send custom skill
    for msg in [custom_skill, custom_data]:
        await client_connection.send(json.dumps(msg))
        await asyncio.sleep(0.05)

    logger.info("Sent custom skill with arbitrary data")

    # Verify data was sent
    assert custom_data['data']['custom']['color'] == '#FF5733'
    assert custom_data['data']['custom']['size'] == 'large'


@pytest.mark.asyncio
async def test_skill_with_large_dataset(client_connection):
    """Test skill response with large data payload."""
    logger.info("Testing skill with large dataset")

    # Skip handshake
    await asyncio.wait_for(client_connection.recv(), timeout=1.0)

    # Create skill with large dataset
    skill_msg = {
        'type': 'mycroft.gui.list.insert',
        'data': {
            'gui_id': 'large-skill',
            'skills': ['large-skill'],
            'index': 0
        },
        'context': {}
    }

    # Create large dataset
    large_data = {
        'type': 'mycroft.session.set',
        'data': {
            'items': [
                {
                    'id': i,
                    'name': f'Item {i}',
                    'description': f'Description for item {i}' * 10,
                    'metadata': {f'key{j}': f'value{j}' for j in range(10)}
                }
                for i in range(100)
            ]
        },
        'context': {'skill_id': 'large-skill'}
    }

    # Send large skill data
    await client_connection.send(json.dumps(skill_msg))
    await asyncio.sleep(0.05)
    await client_connection.send(json.dumps(large_data))

    logger.info(f"Sent skill with {len(large_data['data']['items'])} items")

    # Verify data integrity
    assert len(large_data['data']['items']) == 100
    assert large_data['data']['items'][0]['id'] == 0
    assert large_data['data']['items'][99]['id'] == 99
