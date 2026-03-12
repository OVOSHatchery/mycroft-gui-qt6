#!/usr/bin/env python3
"""
Run Qt GUI app with mock GUI service for testing.

Starts:
1. Mock GUI service on port 18181 (WebSocket at /gui route)
2. Waits for Qt application to connect
"""

import subprocess
import sys
import time
from pathlib import Path

print("""
╔════════════════════════════════════════════════════════════════╗
║           Mycroft GUI Qt6 - Interactive Test Mode              ║
╚════════════════════════════════════════════════════════════════╝

Starting mock GUI service on ws://localhost:18181/gui

""")

# Start GUI service
gui_script = Path(__file__).parent / "mock_gui_service.py"
gui_proc = subprocess.Popen(
    [sys.executable, str(gui_script), "--port", "18181"],
    stdout=subprocess.PIPE,
    stderr=subprocess.STDOUT,
    text=True,
    bufsize=1
)

time.sleep(1)

if gui_proc.poll() is not None:
    stdout, _ = gui_proc.communicate()
    print(f"❌ GUI service failed to start:\n{stdout}")
    sys.exit(1)

print("✓ Mock GUI service running on ws://localhost:18181/gui")
print("✓ Ready for Qt application to connect\n")

print("In another terminal, run:")
print("  cd mycroft-gui-qt6")
print("  ./build/bin/mycroft-gui-app\n")

print("Service logs:")
print("-" * 70)

try:
    # Read and display service output
    while True:
        line = gui_proc.stdout.readline()
        if not line:
            break
        print(line, end='')
except KeyboardInterrupt:
    print("\n" + "-" * 70)
    print("Shutting down services...")
    gui_proc.terminate()
    try:
        gui_proc.wait(timeout=5)
    except subprocess.TimeoutExpired:
        gui_proc.kill()
        gui_proc.wait()
    print("✓ Stopped")
