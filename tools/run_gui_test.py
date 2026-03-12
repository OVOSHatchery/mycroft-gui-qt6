#!/usr/bin/env python3
"""
Run Qt GUI app with mock services for testing.

Starts:
1. Mock messagebus on port 8181 (responds to gui.connected with gui.port)
2. Mock GUI service on port 18181/gui (serves skill data)
3. Qt application
"""

import asyncio
import json
import logging
import subprocess
import sys
import time
from pathlib import Path
import websockets

logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger('RunGUITest')


class MockMessageBus:
    """Mock OVOS MessageBus that responds to gui.connected."""

    def __init__(self, host='0.0.0.0', port=8181, gui_port=18181):
        self.host = host
        self.port = port
        self.gui_port = gui_port
        self.clients = set()

    async def handle_client(self, websocket):
        """Handle incoming messagebus connection."""
        self.clients.add(websocket)
        logger.info(f"MessageBus: Client connected: {websocket.remote_address}")

        try:
            async for message in websocket:
                try:
                    msg = json.loads(message)
                    msg_type = msg.get('type', '')

                    if msg_type == 'mycroft.gui.connected':
                        # Send back the GUI port
                        response = {
                            'type': 'mycroft.gui.port',
                            'data': {
                                'port': self.gui_port,
                                'gui_id': msg.get('data', {}).get('gui_id', 'default')
                            },
                            'context': {}
                        }
                        await websocket.send(json.dumps(response))
                        logger.info(f"MessageBus: Sent GUI port {self.gui_port}")
                    else:
                        logger.debug(f"MessageBus: Received {msg_type}")

                except json.JSONDecodeError:
                    logger.error(f"MessageBus: Invalid JSON: {message}")

        except websockets.exceptions.ConnectionClosed:
            logger.info(f"MessageBus: Client disconnected: {websocket.remote_address}")
        finally:
            self.clients.discard(websocket)

    async def run(self):
        """Start the messagebus server."""
        logger.info(f"Starting MockMessageBus on ws://{self.host}:{self.port}/core")
        async with websockets.serve(self.handle_client, self.host, self.port):
            await asyncio.Future()  # run forever


async def run_services():
    """Run both messagebus and GUI service."""
    messagebus = MockMessageBus(gui_port=18181)

    # Start messagebus
    messagebus_task = asyncio.create_task(messagebus.run())
    await asyncio.sleep(0.5)

    # Start GUI service
    gui_script = Path(__file__).parent / "mock_gui_service.py"
    gui_proc = subprocess.Popen(
        [sys.executable, str(gui_script), "--port", "18181"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )

    await asyncio.sleep(1)

    if gui_proc.poll() is not None:
        stdout, stderr = gui_proc.communicate()
        logger.error(f"GUI service failed to start:\n{stderr.decode()}")
        return

    logger.info("Both services running. To test Qt app, run in another terminal:")
    logger.info("  cd mycroft-gui-qt6")
    logger.info("  ./build/bin/mycroft-gui-app")

    # Keep services running
    try:
        await messagebus_task
    except KeyboardInterrupt:
        logger.info("Shutting down services...")
        gui_proc.terminate()
        gui_proc.wait(timeout=5)


if __name__ == '__main__':
    try:
        asyncio.run(run_services())
    except KeyboardInterrupt:
        logger.info("Interrupted by user")
        sys.exit(0)
