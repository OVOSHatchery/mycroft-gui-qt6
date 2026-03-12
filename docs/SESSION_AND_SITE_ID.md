# Session and Site-ID Support — mycroft-gui-qt6

**Multi-screen and multi-device GUI deployment guide for Qt6 clients.**

---

## Overview

mycroft-gui-qt6 supports **multi-session and multi-site deployments** through session and site identifiers. This allows a single ovos-gui instance to manage multiple screens with isolated or grouped state.

See [ovos-gui: SESSION_AND_SITE_ID_DESIGN.md](../../ovos-gui/docs/SESSION_AND_SITE_ID_DESIGN.md) for comprehensive architecture.

---

## Quick Start

### Default Behavior (On-Device)

```bash
# Launch with default session (on-device single screen)
mycroft-gui-qt6
```

Automatically uses `session_id="default"` and `site_id="default"`.

### Custom Session (Desktop App / Remote GUI)

```bash
# Launch with custom session ID
mycroft-gui-qt6 --session-id="desktop_app"
```

Receives GUI state only for `session_id="desktop_app"`.

### Multi-Location Deployment

```bash
# Launch with site ID (for multi-location sync mode)
mycroft-gui-qt6 --session-id="kitchen_screen" --site-id="kitchen"
```

All GUIs with `site_id="kitchen"` receive identical state updates.

---

## Launch Parameters

### Command-Line Arguments

```bash
mycroft-gui-qt6 [OPTIONS]
```

| Argument | Type | Default | Description |
|----------|------|---------|-------------|
| `--session-id` | string | `"default"` | Session identifier for this GUI instance |
| `--site-id` | string | `"default"` | Site/location identifier for multi-location setups |
| `--gui-id` | string | Auto-generated UUID | Unique identifier for this client (for debugging/reconnection) |

**Examples:**

```bash
# On-device (default)
mycroft-gui-qt6

# Desktop app on same device
mycroft-gui-qt6 --session-id="desktop_app" --site-id="default"

# Living room tablet (multi-location)
mycroft-gui-qt6 --session-id="living_room_tablet" --site-id="living_room"

# Explicit all parameters
mycroft-gui-qt6 \
  --session-id="kitchen_screen" \
  --site-id="kitchen" \
  --gui-id="qt6-kitchen-primary"
```

### Environment Variables

Alternatively, set via environment:

```bash
export MYCROFT_SESSION_ID="living_room_tablet"
export MYCROFT_SITE_ID="living_room"
export MYCROFT_GUI_ID="qt6-living-room"

mycroft-gui-qt6
```

### Configuration File

Set in `~/.mycroft/mycroft.conf`:

```json
{
  "gui": {
    "session_id": "desktop_app",
    "site_id": "office",
    "gui_id": "qt6-office-desktop"
  }
}
```

**Priority order:**
1. Command-line arguments (highest)
2. Environment variables
3. Configuration file
4. Defaults (lowest)

---

## Connection Handshake (Session Established Once)

When mycroft-gui-qt6 connects to ovos-gui, it sends session information **once on startup**:

```javascript
{
    "type": "mycroft.gui.connected",
    "gui_id": "qt6-kitchen-primary-a1b2c3d4",
    "session_id": "kitchen_screen",
    "site_id": "kitchen"
}
```

This tells ovos-gui:
- This client exists (`gui_id`)
- Assign it `session_id="kitchen_screen"` for message routing
- Assign it `site_id="kitchen"` for multi-location grouping

**Key Point:** This information is sent **once at connection**. The session is then fixed for the lifetime of this connection. ovos-gui uses this registration to route all subsequent messages to this client.

---

## Message Filtering

mycroft-gui-qt6 **automatically filters messages** to only process those intended for its session:

```cpp
// In gui/messagebus.cpp

void WebsocketTransport::on_message(const QJsonObject& msg) {
    // Get this client's session info
    QString my_session = config.session_id;
    QString my_site = config.site_id;

    // Check message routing
    QString msg_session = msg["data"]["__session_id"].toString("");
    QString msg_site = msg["data"]["__site_id"].toString("");

    // Determine if message is for us
    bool is_for_me = false;

    if (!msg_session.isEmpty()) {
        // Message targets specific session
        is_for_me = (msg_session == my_session);
    } else if (!msg_site.isEmpty()) {
        // Message targets specific site
        is_for_me = (msg_site == my_site);
    } else {
        // Default routing: for session="default"
        is_for_me = (my_session == "default");
    }

    if (!is_for_me) {
        // Ignore this message
        return;
    }

    // Process message normally
    process_message(msg);
}
```

**Result:** Each GUI instance only sees messages for its session/site, preventing state conflicts.

---

## Common Scenarios

### Scenario 1: On-Device + Satellite

Both devices report the same session ID to share GUI state:

```bash
# On OVOS device (in /opt/mycroft/bin/start-mycroft.sh or service)
mycroft-gui-qt6
# → session_id="default"

# On satellite device (in satellite's startup script)
mycroft-gui-qt6 --session-id="default"
# → Also session_id="default"

# Result: Both devices show identical GUI state
```

### Scenario 2: Desktop App on Same Device

Dedicated window showing app launcher, separate from main screen:

```bash
# Main on-device screen
mycroft-gui-qt6 &

# Desktop app window
mycroft-gui-qt6 --session-id="desktop_app" &

# Result: Each window gets its own GUI state
# Main screen shows current skill
# Desktop app shows launcher, unaffected by skill activity
```

### Scenario 3: Multi-Location Deployment

Multiple rooms, each with multiple screens sharing state:

```bash
# Kitchen (site_id="kitchen")
# Launch on primary screen
mycroft-gui-qt6 --session-id="kitchen_primary" --site-id="kitchen" &

# Launch on tablet
mycroft-gui-qt6 --session-id="kitchen_tablet" --site-id="kitchen" &

# Bedroom (site_id="bedroom")
# Launch on screen
mycroft-gui-qt6 --session-id="bedroom_screen" --site-id="bedroom" &

# Result (if ovos-gui has site_id_sync_mode=true):
# Kitchen primary and tablet show identical state
# Bedroom screen shows different state
# Music command targets __site_id="kitchen" → both kitchen GUIs update
```

---

## Configuration

### mycroft.conf

```json
{
  "gui": {
    // Session and site identifiers
    "session_id": "default",
    "site_id": "default",
    "gui_id": null,  // Auto-generate if not specified

    // Connection
    "websocket": {
      "host": "127.0.0.1",
      "port": 18181
    }
  }
}
```

### Command-Line Override

Command-line arguments override all other sources:

```bash
# Command-line overrides all config/env
mycroft-gui-qt6 --session-id="cli_session" --site-id="cli_site"
```

---

## Debugging Multi-Session Setup

### Check Active Sessions

View connected GUI clients in ovos-gui logs:

```bash
# In ovos-gui output, look for connection messages
tail -f /var/log/mycroft/gui.log | grep "gui_id\|session_id\|site_id"
```

Example output:
```
2026-03-12 12:34:56 GUI Client Connected:
  gui_id: qt6-kitchen-primary-a1b2c3d4
  session_id: kitchen_screen
  site_id: kitchen
```

### Verify Message Routing

Enable debug logging in mycroft-gui-qt6:

```bash
# Set log level
export MYCROFT_LOG_LEVEL=DEBUG

# Launch
mycroft-gui-qt6 --session-id="test"
```

Look for filtering debug output:
```
[DEBUG] Message received: gui.page.show
[DEBUG] Target session: other_session
[DEBUG] My session: test
[DEBUG] Ignoring message (not for this session)
```

### Test Message Delivery

Manually send messages via ovos-messagebus:

```bash
# From ovos device terminal
mycroft-cli-client

# Send to specific session
>>> gui.page.show {"page_names": ["test"], "__session_id": "test"}
```

Qt GUI should only respond if `session_id="test"`.

---

## Troubleshooting

### GUI Shows Irrelevant Content

**Problem:** Multiple GUI instances are showing content for each other's sessions.

**Solution:** Ensure each instance has unique `session_id`:

```bash
# Wrong (both default)
mycroft-gui-qt6 &
mycroft-gui-qt6 &

# Correct (different session IDs)
mycroft-gui-qt6 --session-id="primary" &
mycroft-gui-qt6 --session-id="secondary" &
```

### Remote GUI Not Responding

**Problem:** Tablet GUI not showing updates from core.

**Solution:** Verify session ID and connection:

1. Check connection handshake:
   ```bash
   tcpdump -i lo -n 'port 18181' -A | grep session_id
   ```

2. Verify session ID on command line:
   ```bash
   ps aux | grep mycroft-gui-qt6
   ```

3. Check ovos-gui is routing correctly:
   ```bash
   tail -f /var/log/mycroft/gui.log | grep "session_id"
   ```

### Multi-Site Not Syncing

**Problem:** Different sites are showing same content.

**Solution:** Ensure ovos-gui has `site_id_sync_mode=true` in config:

```json
{
  "gui": {
    "site_id_sync_mode": true
  }
}
```

And verify GUIs have correct `site_id`:

```bash
# Kitchen
mycroft-gui-qt6 --site-id="kitchen" &

# Bedroom
mycroft-gui-qt6 --site-id="bedroom" &
```

---

## Implementation Notes

### Message Filtering Location

Message filtering happens in:
- File: `gui/messagebus.cpp` (or equivalent)
- Function: `on_gui_message_received()` or similar
- Stage: **Before** message is dispatched to handlers

This ensures filtered messages don't affect any internal state.

### Session Identifier Format

Session IDs are **case-sensitive strings** with these constraints:
- Alphanumeric characters, underscores, hyphens
- Max length: 64 characters
- Examples: `default`, `desktop_app`, `kitchen_screen`, `tablet_123`

Avoid spaces, special characters, or reserved names.

### Auto-Generation

If `--gui-id` is not specified, mycroft-gui-qt6 auto-generates a UUID:

```
qt6-{hostname}-{uuid}
Example: qt6-kitchen-device-a1b2c3d4-e5f6-7890-abcd-ef1234567890
```

This helps identify individual instances in logs.

---

## See Also

- `PROTOCOL.md` — Wire protocol and message routing specification
- [ovos-gui: SESSION_AND_SITE_ID_DESIGN.md](../../ovos-gui/docs/SESSION_AND_SITE_ID_DESIGN.md) — Multi-session architecture
- `mycroft-gui-qt5` — Qt5 equivalent implementation

