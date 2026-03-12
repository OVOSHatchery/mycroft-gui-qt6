# Debugging Guide: mycroft-gui-qt6

**How to debug and troubleshoot mycroft-gui-qt6**

---

## Debugging Modes

### Debug Build

Build with debug symbols for better debugging:

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
make clean && make -j$(nproc)
```

This enables:
- ✅ Full debug symbols (file:line information)
- ✅ Assertions and Q_ASSERT checks
- ✅ Slower performance (acceptable for development)

---

## Command-Line Debugging

### Enable Debug Output

```bash
# Very verbose output
mycroft-gui-app --log-level debug 2>&1 | tee debug.log

# Just errors
mycroft-gui-app --log-level error

# Or set environment variable
QT_LOGGING_RULES="*.debug=true" mycroft-gui-app
```

**Debug output shows:**
- Connection status
- Message routing
- Signal emissions
- QML loading
- Session data changes

### Print Debugging (qDebug)

Add temporary debug output:

```cpp
// mycroftcontroller.cpp
void MycroftController::onWebSocketConnected() {
    qDebug() << "WebSocket connected!";
    qDebug() << "Server:" << m_socket.peerName();
    qDebug() << "Port:" << m_socket.peerPort();

    // Your actual code
    m_connected = true;
}
```

Run with output:
```bash
mycroft-gui-app 2>&1 | grep "WebSocket connected"
```

### QML Debugging

Debug QML with:

```qml
// In any QML file
import QtQuick

Item {
    Component.onCompleted: {
        console.log("Item loaded:", this)
        console.log("Width:", width, "Height:", height)
    }

    onWidthChanged: console.log("Width changed to:", width)
}
```

Output appears in console/log when running with:
```bash
mycroft-gui-app --log-level debug
```

---

## Using Qt Creator IDE

### Setup Qt Creator

```bash
# Install Qt Creator (if not already installed)
sudo apt install qtcreator  # Ubuntu/Debian
sudo dnf install qt-creator  # Fedora

# Open Qt Creator
qtcreator &
```

### Open and Debug Project

1. **File → Open File or Project**
   - Select `mycroft-gui-qt6/CMakeLists.txt`

2. **Configure**
   - Select build kit (GCC/Clang)
   - Select build directory

3. **Build**
   - Build → Build Project

4. **Debug**
   - Debug → Start Debugging
   - Or press F5

### Set Breakpoints

1. **Click in line number margin** to set breakpoint (red dot appears)
2. **Run with Debug** (F5)
3. **Execution stops** at breakpoint
4. **Inspect variables** in Variables pane
5. **Step** with F10 (line) or F11 (into function)

### Watch Variables

In Debugger → Locals & Expressions:
- Right-click variable
- "Add to Watch"
- Variable updates shown as you step

### Example Debugging Session

```
File: import/mycroftcontroller.cpp
Line: onWebSocketConnected()

1. Set breakpoint on m_connected = true;
2. Start mycroft-gui-app with debugger
3. When connection made, breakpoint hit
4. Inspect m_socket.peerName() in Variables pane
5. Step through connection code
6. Check m_isConnected value
```

---

## GDB Command-Line Debugging

For shell-based debugging (remote servers, etc.):

```bash
# Build with debug symbols
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make clean && make

# Start app under gdb
gdb ./mycroft-gui-app

# In gdb prompt:
(gdb) break MycroftController::onWebSocketConnected
(gdb) run
# ... app runs until breakpoint hits ...
(gdb) print m_socket.peerName()
(gdb) step
(gdb) next
(gdb) continue
(gdb) quit
```

**Common GDB commands:**
```
break <function>      # Set breakpoint
run                   # Start program
continue (c)          # Resume after breakpoint
step (s)              # Step into function
next (n)              # Execute line, don't step in
print <var>           # Print variable value
backtrace (bt)        # Show call stack
up/down               # Navigate call stack
quit (q)              # Exit debugger
```

---

## Memory Debugging

### Detect Memory Leaks

**Build with AddressSanitizer:**
```bash
cd build
cmake .. -DCMAKE_CXX_FLAGS="-fsanitize=address" -DCMAKE_BUILD_TYPE=Debug
make clean && make -j$(nproc)

# Run
./mycroft-gui-app

# Or run tests
make test VERBOSE=1
```

**Output shows:**
```
=================================================================
==12345==ERROR: LeakSanitizer: detected memory leaks

Direct leak of 256 byte(s) in 1 object(s) allocated from:
    #0 0x7ffff7a6d099 in operator new (/lib/libc.so.6+...)
    #1 0x5555557a1b3c in MycroftController::connect() ...
```

### Detect Use-After-Free

Same AddressSanitizer build catches:
```
==12345==ERROR: AddressSanitizer: heap-use-after-free on address
```

### Valgrind (detailed memory analysis)

```bash
# Install valgrind
sudo apt install valgrind  # Ubuntu/Debian

# Run with valgrind
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         ./mycroft-gui-app

# For tests
valgrind --leak-check=full ./autotests/message_routing_test
```

---

## Performance Debugging

### Profile with Qt Creator

1. Analyze → QML Profiler
2. Start program
3. Observe frame rate, memory usage
4. Identify hot spots

### Profile with perf (Linux)

```bash
# Install
sudo apt install linux-tools

# Record
perf record -g ./mycroft-gui-app

# Run your scenario, then Ctrl+C

# Analyze
perf report

# Or generate flamegraph
perf script | stackcollapse-perf.pl | flamegraph.pl > flamegraph.svg
```

### Check Memory Usage

```bash
# While running mycroft-gui-app in another terminal
watch -n 1 'ps aux | grep mycroft-gui'

# Check process details
cat /proc/$(pidof mycroft-gui-app)/status | grep VmRSS

# Real-time monitoring
htop
# Search for mycroft-gui-app (press f)
# Check VIRT and RES columns
```

---

## Network Debugging

### Monitor WebSocket Traffic

```bash
# Install Wireshark
sudo apt install wireshark

# Capture traffic
sudo wireshark &

# Or use tcpdump
sudo tcpdump -i lo -A 'tcp port 18181'
```

### Check Port Connectivity

```bash
# Is port 18181 listening?
netstat -tlnp | grep 18181

# Or with ss
ss -tlnp | grep 18181

# Try connecting
nc -zv localhost 18181

# Or with curl (if WebSocket endpoint supports HTTP)
curl -v ws://localhost:18181
```

### Mock WebSocket Server

For testing without ovos-gui:

```python
# test_websocket_server.py
import asyncio
import json
from websockets import serve

async def echo(websocket, path):
    async for message in websocket:
        data = json.loads(message)
        print(f"Received: {data}")
        # Send back session data
        response = {
            "type": "gui.page.show",
            "namespace": "test",
            "data": {"message": "Hello from test server"}
        }
        await websocket.send(json.dumps(response))

async def main():
    async with serve(echo, "localhost", 18181):
        print("Test server on ws://localhost:18181")
        await asyncio.Future()

asyncio.run(main())
```

Run with:
```bash
python3 test_websocket_server.py

# In another terminal
mycroft-gui-app
```

---

## QML Debugging

### QML Profiler

In Qt Creator:
1. Analyze → QML Profiler
2. Click Start button
3. Run mycroft-gui-app
4. Observe rendering, signal emission, memory allocation

### QML Language Server (Qt Creator Integration)

Automatic when opening .qml files:
- Syntax highlighting
- Auto-completion
- Go to definition (Ctrl+click)
- Find references (Ctrl+Shift+U)

### Enable QML Debugging

Run with QML debugging enabled:
```bash
mycroft-gui-app --qmljsdebugger=port:3768,block
```

Then in Qt Creator:
- Debug → Attach to QML Debugger
- Port: 3768

---

## Common Issues & Debugging

### Issue: "Cannot connect to ovos-gui"

**Debug steps:**
```bash
# 1. Check ovos-gui is running
systemctl status ovos-gui

# 2. Check port is listening
netstat -tlnp | grep 18181

# 3. Check logs
mycroft-gui-app --log-level debug 2>&1 | grep -i connect

# 4. Check firewall
sudo ufw status
sudo ufw allow 18181

# 5. Check mycroft.conf
cat ~/.config/mycroft/mycroft.conf | grep -A 3 gui_websocket
```

**Solution:**
- Start ovos-gui first
- Verify port 18181 is in mycroft.conf
- Check firewall isn't blocking

### Issue: "QML module not found"

**Debug steps:**
```bash
# 1. Check QML import path
echo $QML_IMPORT_PATH

# 2. Check library is installed
pkg-config --modversion mycroft-gui-qt6

# 3. Check library path
ldd /usr/local/lib/libmycroft-gui-qt6.so | grep -i kirigami

# 4. Verify QML plugin
ls -la /usr/local/lib/qt6/qml/Mycroft/
```

**Solution:**
```bash
export QML_IMPORT_PATH=/usr/local/lib/qt6/qml:$QML_IMPORT_PATH
mycroft-gui-app
```

### Issue: "Qt plugin error"

**Debug steps:**
```bash
# 1. Check Qt plugin path
echo $QT_PLUGIN_PATH

# 2. Check plugins exist
ls /usr/lib/qt6/plugins/platforms/

# 3. Try with explicit path
export QT_PLUGIN_PATH=/usr/lib/qt6/plugins:$QT_PLUGIN_PATH
mycroft-gui-app
```

**Solution:**
- Set QT_PLUGIN_PATH before running
- Add to ~/.bashrc for persistence

### Issue: Segmentation Fault

**Debug steps:**
```bash
# 1. Run with gdb
gdb ./mycroft-gui-app
(gdb) run
# ... crash happens ...
(gdb) backtrace
# Shows call stack

# 2. Or use AddressSanitizer
cmake .. -DCMAKE_CXX_FLAGS="-fsanitize=address"
make && ./mycroft-gui-app
```

**Solution:**
- Look at backtrace to find crash location
- Check NULL pointer dereference
- Check array bounds
- Use memory debugger

---

## Logging Best Practices

### For Users (Production)

Minimal logging:
```cpp
// Only log critical errors
qWarning() << "Failed to connect:" << errorString;
```

### For Developers (Development)

Detailed logging:
```cpp
qDebug() << "MycroftController::onWebSocketConnected()";
qDebug() << "  Server:" << m_socket.peerName();
qDebug() << "  Port:" << m_socket.peerPort();
qDebug() << "  Connected:" << m_connected;
```

### Conditional Logging

```cpp
#ifdef QT_DEBUG
    qDebug() << "Detailed debug info";
#endif

// Or at runtime
if (qEnvironmentVariableIsSet("DEBUG_MYCROFT")) {
    qDebug() << "Debug mode enabled";
}
```

Enable with:
```bash
DEBUG_MYCROFT=1 mycroft-gui-app
```

---

## Remote Debugging

### Debug on Different Machine

```bash
# On target machine
gdbserver :1234 ./mycroft-gui-app

# On development machine
gdb ./mycroft-gui-app
(gdb) target remote <target-ip>:1234
(gdb) break MycroftController::connect
(gdb) continue
```

### Debug in Container

```bash
# Build debug image
docker build -f Dockerfile.debug -t mycroft-gui-qt6-debug .

# Run with debugging enabled
docker run -it -p 3768:3768 mycroft-gui-qt6-debug

# Connect with Qt Creator
# Debug → Attach to QML Debugger
# Port: 3768
```

---

## Debugging Checklist

Before asking for help:

- [ ] Run with `--log-level debug`
- [ ] Check recent log file in `~/.cache/`
- [ ] Verify ovos-gui is running
- [ ] Verify port 18181 is listening
- [ ] Check mycroft.conf configuration
- [ ] Run test suite (`make test`)
- [ ] Check for memory leaks (AddressSanitizer)
- [ ] Try clean rebuild (`make clean && make`)
- [ ] Update environment variables (QML_IMPORT_PATH, etc.)

---

## Next Steps

- **Read TESTING.md**: How to write and run tests
- **Read CONTRIBUTING.md**: Guidelines for bug reports
- **Check CODE_GUIDE.md**: Understand the code structure

---

**Happy debugging!** 🐛
