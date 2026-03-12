# Integration Guide: mycroft-gui-qt6 in the OpenVoiceOS GUI Ecosystem

**How mycroft-gui-qt6 connects to the modern OpenVoiceOS GUI architecture**

---

## Overview: The New GUI Architecture

This is the **first modernized GUI client** in the new OpenVoiceOS architecture. The ecosystem consists of three layers:

```
┌─────────────────────────────────────────────────────────────────┐
│ Skills & Applications (ovos-core, skill plugins)                │
│ Send template data via: ovos-gui-api-client library              │
└────────────────────────┬────────────────────────────────────────┘
                         │
                         ▼
        ┌────────────────────────────────────┐
        │    ovos-gui (Messagebus Service)   │
        │                                    │
        │  - Manages GUI state              │
        │  - Implements GUI protocol        │
        │  - Coordinates between clients    │
        │                                    │
        │  Port: 18181 (base)               │
        └────────────────────────────────────┘
                         │
          ┌──────────────┼──────────────┐
          │              │              │
          ▼              ▼              ▼
     Legacy Qt5    Modern Web      New Qt6
     (Adapter)      (Browser)      (This)

     mycroft-gui-qt5  pyhtmx-gui-    mycroft-gui-qt6
     via legacy       client          (modernized)
     plugin
```

---

## Key Repositories

### 1. **ovos-gui** (Core Service)
- **Purpose**: Central GUI state manager and messagebus service
- **Repository**: https://github.com/OpenVoiceOS/ovos-gui
- **Config Port**: `gui_websocket.base_port` in `mycroft.conf` (default 18181)
- **Role**:
  - Receives template data from skills
  - Manages GUI client connections
  - Routes messages between skills and GUI clients
  - Handles homescreen and system templates (SYSTEM_idle, SYSTEM_loading, etc.)

**Configuration in mycroft.conf:**
```javascript
{
  "gui": {
    "idle_display_skill": "skill-ovos-homescreen.openvoiceos",
    "extension": "generic",
    "default_qt_version": 6  // Updated to 6 for mycroft-gui-qt6
  },
  "gui_websocket": {
    "host": "0.0.0.0",
    "base_port": 18181,
    "route": "/gui",
    "ssl": false
  }
}
```

---

### 2. **ovos-gui-api-client** (Skill Communication Library)
- **Purpose**: Python library for skills to send GUI data to ovos-gui
- **Repository**: https://github.com/OpenVoiceOS/ovos-gui-api-client
- **Entry Point**: `from ovos_gui_api_client import GUIClient`
- **Role**:
  - Skills use this to send template data
  - Implements template protocol (SYSTEM_text, SYSTEM_image, etc.)
  - Manages session data and event handling
  - Voice-first constraint enforcement

**Example Skill Usage:**
```python
from ovos_gui_api_client import GUIClient

gui = GUIClient()

# Display a text template
gui.show_text("Hello World",
              title="Greeting",
              sessionId="my_skill_session")

# Display a weather template
gui.show_weather_template({
    "current_temp": 72,
    "condition": "Sunny",
    "location": "New York"
})
```

**Supported Templates** (all 25+ system templates):
- Content: text, image, animated_image, list, grid, table, html, url
- Media: audio_player, video_player
- Utility: clock, timer, weather, map
- Status: idle, loading, success, error
- Input: select, confirm

---

### 3. **ovos-legacy-mycroft-gui-plugin** (Adapter Bridge)
- **Purpose**: Backward-compatible adapter for Qt-based GUI clients
- **Repository**: https://github.com/OpenVoiceOS/ovos-legacy-mycroft-gui-plugin
- **Entry Point**: OPM plugin `opm.gui_adapter`
- **Role**:
  - Implements the mycroft gui protocol (WebSocket port 18181) — the standard protocol used by ALL Qt GUI clients (both Qt5 and Qt6)
  - Translates between mycroft gui protocol messages and ovos-gui internal API
  - Manages Qt GUI client lifecycle (startup, connection, shutdown)
  - **Both mycroft-gui-qt5 AND mycroft-gui-qt6 connect through this same adapter**

**Key Integration Points:**
```
mycroft-gui-qt5 (Qt5 client) ─┐
                               ├─ Connect to port 18181 (mycroft gui protocol)
mycroft-gui-qt6 (Qt6 client) ─┘
    │
    └─ Protocol: WebSocket with JSON messages
       {
         "type": "gui.page.show",
         "namespace": "skill.weather",
         "data": { "temp": 72, ... }
       }
       │
       ▼
ovos-legacy-mycroft-gui-plugin (Adapter)
    │
    ├─ Receives protocol messages
    ├─ Converts to ovos-gui API calls
    │
    └─ Forwards to ovos-gui
       {
         "template": "SYSTEM_weather",
         "sessionId": "skill.weather",
         "sessionData": { "temp": 72, ... }
       }
       │
       ▼
ovos-gui (Core Service)
    │
    └─ Manages state and distributes to all connected clients
```

---

## mycroft-gui-qt6: The First Modern Qt Client

### What Is It?

`mycroft-gui-qt6` is a **modernized Qt6 port** of the abandoned `mycroft-gui-qt5`. It's the first GUI client updated to work with the new OpenVoiceOS architecture while maintaining backward compatibility via the legacy adapter plugin.

### Architecture

```
┌──────────────────────────────────────────────────────┐
│             mycroft-gui-qt6                          │
│                                                      │
│  C++ Core (QML_ELEMENT macro):                      │
│  - MycroftController (singleton)                    │
│    - Manages WebSocket connection to port 18181    │
│    - Receives skill templates & session data       │
│    - Emits signals for QML layer                   │
│                                                      │
│  - AbstractSkillView (per-skill UI container)      │
│    - Manages skill lifecycle                       │
│    - Session data synchronization                  │
│                                                      │
│  QML Layer (25 system templates + 17 components):   │
│  - Renders templates as native Qt UI               │
│  - No network access (data comes from C++)         │
│  - Touch and voice event handling                   │
│                                                      │
└──────────────────────────────────────────────────────┘
         │
         ▼
   WebSocket (port 18181)
   Legacy Mycroft Protocol
         │
         ▼
┌──────────────────────────────────────────────────────┐
│  ovos-legacy-mycroft-gui-plugin                      │
│                                                      │
│  Bridges old → new protocol                         │
│  (Adapter Pattern)                                   │
│                                                      │
└──────────────────────────────────────────────────────┘
         │
         ▼
    ovos-gui service
    (Core GUI state manager)
```

### Key Features

1. **Modern Qt6** — C++17, latest Qt APIs, better performance
2. **Full Template Support** — All 25+ system templates rendered natively
3. **Voice-First** — Every touch action has a voice equivalent
4. **Backward Compatible** — Works with legacy plugin bridge to ovos-gui
5. **Well-Documented** — ARCHITECTURE.md, CODE_GUIDE.md, COMPONENTS.md for maintenance

---

## Deployment Scenarios

### Scenario 1: Single Machine (Most Common)

**Setup:**
```
┌─────────────────────────────────────────┐
│  Single Computer / Raspberry Pi         │
│                                         │
│  ┌─────────────┐                       │
│  │ ovos-core   │                       │
│  └────────┬────┘                       │
│           │ (bus messages)             │
│  ┌────────▼──────────────────────────┐ │
│  │ ovos-gui                           │ │
│  │ (GUI messagebus service)           │ │
│  │ Port: 18181 (internally)           │ │
│  └────────┬──────────────────────────┘ │
│           │                             │
│  ┌────────▼──────────────────────────┐ │
│  │ ovos-legacy-mycroft-gui-plugin     │ │
│  │ (Adapter, loaded as OPM plugin)    │ │
│  └────────┬──────────────────────────┘ │
│           │                             │
│  ┌────────▼──────────────────────────┐ │
│  │ mycroft-gui-qt6 (Qt GUI Application)│ │
│  │ Connects to localhost:18181        │ │
│  └────────────────────────────────────┘ │
│                                         │
└─────────────────────────────────────────┘
```

**Installation Steps:**
```bash
# 1. Install ovos-core
pip install ovos-core

# 2. Install ovos-gui
pip install ovos-gui

# 3. Install legacy adapter plugin
pip install ovos-legacy-mycroft-gui-plugin

# 4. Install mycroft-gui-qt6
# See INSTALL.md for distribution-specific steps
sudo make install

# 5. Configure
# Edit ~/.config/mycroft/mycroft.conf:
{
  "gui": {
    "default_qt_version": 6,
    "idle_display_skill": "skill-ovos-homescreen.openvoiceos"
  },
  "gui_websocket": {
    "host": "0.0.0.0",
    "base_port": 18181
  }
}

# 6. Start
mycroft-core start
mycroft-gui-app  # In another terminal or systemd service
```

### Scenario 2: Remote GUI (Network Deployment)

For GUI on a different machine than core:

```
┌──────────────────────┐       ┌──────────────────────┐
│  Machine A (Core)    │       │  Machine B (Display) │
│                      │       │                      │
│  ovos-core     │     │       │                      │
│  ovos-gui ─────┼─────┼───────┼─ ovos-legacy-... ────│
│  (Port 18181)  │     │       │ mycroft-gui-qt6      │
│                │     │       │ (localhost:18181)    │
│                │     │       │                      │
└──────────────────────┘       └──────────────────────┘
```

**Configuration:**
```javascript
// On Machine A (ovos-core)
{
  "gui_websocket": {
    "host": "192.168.1.100",  // Listen on network interface
    "base_port": 18181
  }
}

// On Machine B (mycroft-gui-qt6)
// Set environment variable or config:
MYCROFT_MESSAGEBUS_HOST=192.168.1.100
MYCROFT_MESSAGEBUS_PORT=18181
```

### Scenario 3: Container Deployment

```dockerfile
# Dockerfile example
FROM ubuntu:22.04

# Install ovos-core, ovos-gui, legacy plugin
RUN apt-get install -y \
    ovos-core \
    python3-ovos-gui \
    python3-ovos-legacy-mycroft-gui-plugin \
    mycroft-gui-qt6  # Or build from source

# Configure
COPY mycroft.conf /home/ovos/.config/mycroft/

# Start
CMD ["sh", "-c", "mycroft-core start & mycroft-gui-app"]
```

---

## Integration with Skills

### How Skills Discover and Use the GUI

**Skills use `ovos-gui-api-client`:**
```python
from ovos_gui_api_client import GUIClient

class MyWeatherSkill:
    def __init__(self):
        self.gui = GUIClient()

    def handle_weather(self, message):
        # Send data to GUI
        self.gui.show_weather_template({
            "location": "San Francisco",
            "current_temp": 72,
            "condition": "Sunny",
            "icon": "weather-sunny.png"
        })

        # This is received by:
        # 1. ovos-gui (registers session data)
        # 2. ovos-legacy-mycroft-gui-plugin (adapts to port 18181)
        # 3. mycroft-gui-qt6 (renders via SYSTEM_weather template)
```

### Session Data Flow

```
Skill creates session data:
{
  "template": "SYSTEM_weather",
  "namespace": "skill-weather",
  "sessionId": "skill-weather.openvoiceos:weather",
  "sessionData": {
    "location": "San Francisco",
    "current_temp": 72,
    "condition": "Sunny"
  }
}
         │
         ▼
ovos-gui-api-client:
  Calls GUI.show_weather_template()
         │
         ▼
ovos-gui (messagebus):
  Stores in session registry
  Broadcasts "gui.page.show" event
         │
         ▼
ovos-legacy-mycroft-gui-plugin:
  Receives "gui.page.show"
  Translates to WebSocket protocol
  Sends: { "type": "gui.page.show", "namespace": "...", "data": {...} }
         │
         ▼
mycroft-gui-qt6:
  Receives on port 18181
  AbstractSkillView loads SYSTEM_weather template
  QML renders with received data
         │
         ▼
User sees weather display on screen
```

---

## Event Routing: Voice and Touch

### Touch Events in mycroft-gui-qt6

When user taps a button in the GUI:

```
mycroft-gui-qt6 (QML Button click)
    │
    └─ MycroftController::onGuiEvent()
         │
         ├─ Translates to ovos-gui event format
         └─ Sends WebSocket message to port 18181
                │
                ▼
ovos-legacy-mycroft-gui-plugin
    │
    └─ Receives WebSocket message
         │
         └─ Forwards to ovos-gui as bus message
                │
                ▼
ovos-gui
    │
    └─ Routes to appropriate skill:
         gui.user.interaction event
                │
                ▼
Skill receives touch event
    │
    └─ Can trigger same action as voice command
       (ensures touch is a shortcut, never the only path)
```

**Example: Weather Skill Button Click**

```python
# In skill
def handle_message(self, message):
    if message.type == "gui.user.interaction":
        action = message.data.get("action")
        if action == "view_forecast":
            self.show_forecast()

# Same action triggered by:
# 1. Voice: "Show forecast" → STT → intent → handle_message()
# 2. Touch: Button click → mycroft-gui-qt6 event → handle_message()
```

---

## Comparing With Other GUI Clients

### Qt5 GUI (mycroft-gui-qt5)
- **Status**: Deprecated but functional — modernized in Q1 2026 (C++17, TLS/SSL, auth tokens, memory fixes)
- **Connection**: Through the SAME legacy adapter plugin as Qt6 (WebSocket port 18181)
- **Modern Features**: Yes — C++17, Qt5.15+, TLS/SSL, bearer token auth
- **Maintenance**: One-time modernization complete; deprecated, no further updates planned. Use for systems where Qt6 is unavailable.

### Qt6 Modern GUI (mycroft-gui-qt6) ← **You are here**
- **Status**: AI-modernized, actively maintained
- **Connection**: Through the same legacy adapter plugin as Qt5 (mycroft gui protocol, port 18181)
- **Modern Features**: Yes — C++17, Qt6.5+, clean architecture
- **Maintenance**: Well-documented, non-Qt developers can contribute

### Browser-Based GUI (pyhtmx-gui-client)
- **Status**: Active development, FastAPI + HTMX
- **Connection**: WebSocket (JSON) + SSE (events)
- **Platform**: Works on any system with a web browser
- **Use Case**: Cloud-hosted, remote access, mobile-friendly

### Shell GUI (ovos-shell)
- **Status**: Active, production-ready
- **Type**: Kirigami desktop shell (not a standalone app)
- **Connection**: Uses Mycroft.SkillView QML component
- **Use Case**: Full desktop environment with homescreen integration

---

## Migration Path: Qt5 to Qt6

### For End Users

If you're currently using **mycroft-gui-qt5**:

1. **Uninstall Qt5 version**
   ```bash
   sudo apt remove mycroft-gui-qt5
   # Or manually remove if built from source
   ```

2. **Install Qt6 version** (see [INSTALL.md](INSTALL.md))
   ```bash
   sudo apt install mycroft-gui-qt6
   # Or build from source per BUILD.md
   ```

3. **Verify connection**
   ```bash
   # Should see "Connected to ovos-gui" in logs
   mycroft-gui-app --log-level debug
   ```

4. **Update configuration** (if needed)
   ```javascript
   // ~/.config/mycroft/mycroft.conf
   {
     "gui": {
       "default_qt_version": 6  // Add this
     }
   }
   ```

### For Developers

If you're **extending the Qt GUI**:

1. **Qt5 → Qt6 API changes**: See [PORTING_GUIDE.md](docs/PORTING_GUIDE.md)
2. **Custom QML components**: Review [COMPONENTS.md](docs/COMPONENTS.md)
3. **Custom C++ extensions**: See [CODE_GUIDE.md](docs/CODE_GUIDE.md)
4. **Example**: Adding a custom template

   ```cpp
   // C++ registration (Qt6 modern)
   #include <QQmlEngine>

   class MyCustomView : public AbstractSkillView {
       Q_OBJECT
       QML_ELEMENT  // No manual qmlRegisterType needed!
   };
   ```

   ```qml
   // QML usage (Qt6 import syntax)
   import QtQuick
   import Mycroft

   MyCustomView {
       // Your UI code
   }
   ```

---

## Troubleshooting Integration

### Q: mycroft-gui-qt6 won't connect

**Diagnosis:**
```bash
# Check ovos-gui is running
systemctl status ovos-gui
# or
ps aux | grep ovos-gui

# Check port 18181 is listening
netstat -tlnp | grep 18181

# Check logs
journalctl -u ovos-gui -n 50
```

**Solutions:**
1. Ensure ovos-gui is installed and running
2. Ensure legacy adapter plugin is installed:
   ```bash
   pip install ovos-legacy-mycroft-gui-plugin
   ```
3. Check firewall isn't blocking port 18181
4. Check mycroft.conf has correct host/port settings

### Q: Skills don't appear in mycroft-gui-qt6

**Diagnosis:**
```bash
# Verify ovos-gui is receiving skill data
# Check ovos-gui logs for "gui.page.show" events
journalctl -u ovos-gui -f | grep "page.show"
```

**Solutions:**
1. Ensure skills are using `ovos-gui-api-client`:
   ```bash
   pip install ovos-gui-api-client
   ```
2. Verify skill is sending template data (not old protocol)
3. Check skill namespace matches session ID in templates

### Q: mycroft-gui-qt6 isn't rendering templates

**Diagnosis:**
```bash
# Check QML can load
qmlimportscanner -rootPath /usr/local/lib/qt6/qml | grep Mycroft

# Check library dependencies
ldd /usr/local/lib/libmycroft-gui-qt6.so | grep Qt6
```

**Solutions:**
1. Verify QML_IMPORT_PATH includes plugin directory:
   ```bash
   export QML_IMPORT_PATH=/usr/local/lib/qt6/qml:$QML_IMPORT_PATH
   ```
2. Check template is supported (see COMPONENTS.md)
3. Verify session data format matches template spec

---

## Links to Related Projects

### Core GUI Infrastructure
- [ovos-gui](https://github.com/OpenVoiceOS/ovos-gui) — GUI messagebus service
- [ovos-gui-api-client](https://github.com/OpenVoiceOS/ovos-gui-api-client) — Skill API library
- [ovos-legacy-mycroft-gui-plugin](https://github.com/OpenVoiceOS/ovos-legacy-mycroft-gui-plugin) — Qt adapter bridge

### Other GUI Clients
- [pyhtmx-gui-client](https://github.com/OpenVoiceOS/pyhtmx-gui-client) — Browser-based GUI
- [ovos-shell](https://github.com/OpenVoiceOS/ovos-shell) — Full desktop shell
- [ovos-skill-homescreen](https://github.com/OpenVoiceOS/ovos-skill-homescreen) — Default homescreen

### References & Planning Docs
- [ovos-gui PR #100](https://github.com/OpenVoiceOS/ovos-gui/pull/100) — Qt6 compatibility strategy (in ovos-gui)
- [GUI Protocol](https://github.com/OpenVoiceOS/ovos-gui/blob/dev/protocol.md) — Official protocol spec
- [Template Design Philosophy](https://github.com/OpenVoiceOS/ovos-gui-api-client/blob/dev/README.md) — Why templates matter

---

## Next Steps

1. **For Users**: Follow [INSTALL.md](INSTALL.md) to install mycroft-gui-qt6
2. **For Developers**:
   - Read [ARCHITECTURE.md](docs/ARCHITECTURE.md) for system design
   - Review [CODE_GUIDE.md](docs/CODE_GUIDE.md) for Qt concepts
   - Check [COMPONENTS.md](docs/COMPONENTS.md) for API reference
3. **For Integration**:
   - Configure ovos-core with ovos-gui
   - Verify legacy adapter plugin is loaded
   - Test with existing OVOS skills
4. **For Contribution**:
   - Report bugs on GitHub
   - Submit enhancements with tests
   - Help with documentation improvements

---

**This is the bridge between legacy and modern.** mycroft-gui-qt6 is the first GUI client updated for the new OpenVoiceOS architecture. Use the legacy adapter plugin to connect seamlessly to ovos-gui, and enjoy a modern Qt6-based GUI experience.

---

## GUI History: Mycroft AI → OpenVoiceOS

Understanding the GUI ecosystem requires knowing its history:

### The Original Mycroft AI GUI (Pre-OVOS)
- Skills shipped **arbitrary QML files** that were sent over the wire at runtime
- The `mycroft-gui` client (created by KDE/Mycroft AI) received and rendered this QML dynamically
- This was fragile: any QML change in a skill could break rendering on the client

### The OVOS Modernization
- OVOS replaced arbitrary QML with a **bundled template system** (SYSTEM_text, SYSTEM_weather, SYSTEM_media_player, etc.)
- Templates are pre-installed in the Qt client — skills just send data, not UI code
- The `ovos-legacy-mycroft-gui-plugin` implements the **mycroft gui protocol** (WebSocket port 18181) — this is the current standard for ALL Qt clients

### Important: Incompatibility Warning
- **Pre-OVOS `mycroft-gui` binaries will NOT work** with modern OVOS. The legacy adapter is not backwards-compatible with the original Mycroft AI GUI binaries.
- You MUST recompile from the current `mycroft-gui-qt5` or `mycroft-gui-qt6` source code and use the latest `ovos-gui` service.

### The Word "Legacy"
- The word "legacy" in `ovos-legacy-mycroft-gui-plugin` refers to the **protocol's Mycroft AI origins**, not its current status
- This protocol is the CURRENT, ACTIVE standard used by all Qt GUI clients
- Both mycroft-gui-qt5 and mycroft-gui-qt6 connect through this same adapter

