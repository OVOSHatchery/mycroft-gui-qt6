#!/usr/bin/env python3
"""
Mock GUI Service - WebSocket server implementing the Mycroft GUI protocol.
Used for testing Qt6 GUI client without requiring full OVOS stack.

Protocol: WebSocket on port 18181
Message format: JSON with type, data, context fields
"""

import asyncio
import json
import logging
import argparse
from datetime import datetime
from typing import Dict, Set, Any
import websockets

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger('MockGUIService')


class MockGUIService:
    """Mock implementation of the GUI service protocol."""

    def __init__(self, host='0.0.0.0', port=18181):
        self.host = host
        self.port = port
        self.clients: Set[Any] = set()
        self.current_skill = None
        self.session_data: Dict[str, Any] = {}
        self.script = []
        self.script_idx = 0

    def load_script(self, script: list):
        """Load a test script of server responses."""
        self.script = script
        self.script_idx = 0
        logger.info(f"Loaded {len(script)} responses")

    async def handle_client(self, websocket):
        """Handle incoming WebSocket connection."""
        self.clients.add(websocket)
        logger.info(f"Client connected: {websocket.remote_address}")

        try:
            # Send connected message
            await self.send_to_client(websocket, {
                'type': 'mycroft.gui.connected',
                'data': {'version': '1.0'},
                'context': {}
            })

            # Process incoming messages
            async for message in websocket:
                try:
                    msg = json.loads(message)
                    msg_type = msg.get('type', '')
                    data = msg.get('data', {})
                    logger.info(f"Received: {msg_type}")

                    # Handle common message types
                    if msg_type == 'recognizer_loop:utterance':
                        await self.handle_utterance(websocket, data)
                    elif msg_type == 'mycroft.gui.list.insert':
                        self.current_skill = data.get('gui_id')
                        logger.info(f"Active skill: {self.current_skill}")
                    elif msg_type == 'mycroft.session.set':
                        self.session_data.update(data)
                    else:
                        logger.debug(f"Unhandled message type: {msg_type}")

                except json.JSONDecodeError:
                    logger.error(f"Invalid JSON: {message}")
                except Exception as e:
                    logger.error(f"Error processing message: {e}", exc_info=True)

        except websockets.exceptions.ConnectionClosed:
            logger.info(f"Client disconnected: {websocket.remote_address}")
        except Exception as e:
            logger.error(f"Error in handle_client: {e}", exc_info=True)
        finally:
            self.clients.discard(websocket)
            
    async def send_to_client(self, websocket, msg: dict):
        """Send message to specific client."""
        try:
            await websocket.send(json.dumps(msg))
            logger.debug(f"Sent: {msg['type']}")
        except Exception as e:
            logger.error(f"Send failed: {e}")
            
    async def broadcast(self, msg: dict):
        """Broadcast message to all connected clients."""
        for client in self.clients:
            await self.send_to_client(client, msg)
            
    async def handle_utterance(self, websocket, data: dict):
        """Handle voice utterance - send back scripted or default response."""
        utterance = data.get('utterance', '')
        logger.info(f"Processing utterance: {utterance}")
        
        if self.script and self.script_idx < len(self.script):
            response = self.script[self.script_idx]
            self.script_idx += 1
            await self.broadcast(response)
        else:
            # Default echo response
            await self.broadcast({
                'type': 'mycroft.gui.list.insert',
                'data': {
                    'gui_id': 'default-response',
                    'skills': ['default-response'],
                    'index': 0
                },
                'context': {}
            })
            
    async def run(self):
        """Start the WebSocket server."""
        logger.info(f"Starting MockGUIService on ws://{self.host}:{self.port}/gui")
        # Note: websockets.serve() accepts all paths, clients should connect to /gui route
        async with websockets.serve(self.handle_client, self.host, self.port):
            await asyncio.Future()  # run forever


def create_test_script_weather():
    """Create a script for weather skill response."""
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
                    }
                }
            },
            'context': {'skill_id': 'weather-skill'}
        }
    ]


def create_test_script_hello():
    """Create a script for hello world skill response."""
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
                    'message': 'Hello from Mock GUI Service!'
                }
            },
            'context': {'skill_id': 'hello-world'}
        }
    ]


async def main():
    parser = argparse.ArgumentParser(description='Mock GUI Service for testing Qt clients')
    parser.add_argument('--host', default='0.0.0.0', help='Bind to host')
    parser.add_argument('--port', type=int, default=18181, help='Bind to port')
    parser.add_argument('--script', choices=['weather', 'hello'], help='Load test script')
    args = parser.parse_args()
    
    service = MockGUIService(args.host, args.port)
    
    if args.script == 'weather':
        service.load_script(create_test_script_weather())
    elif args.script == 'hello':
        service.load_script(create_test_script_hello())
    
    await service.run()


if __name__ == '__main__':
    asyncio.run(main())
