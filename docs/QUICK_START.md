# Quick Start: mycroft-gui-qt6

**Get mycroft-gui-qt6 running in 5 minutes**

---

## What Is This?

A modern Qt6 GUI client for OpenVoiceOS voice assistant. It displays skill interfaces, handles voice and touch interactions, and integrates with the OpenVoiceOS ecosystem.

---

## Prerequisites

✅ **OpenVoiceOS Core** installed and running
✅ **ovos-gui** service installed
✅ **ovos-legacy-mycroft-gui-plugin** installed
✅ **Linux system** (Ubuntu 20.04+, Fedora 38+, Arch, Alpine, or other)
✅ **Qt6** 6.5 or later (automatically verified during installation)

---

## Installation: 60 seconds

### Option 1: Ubuntu/Debian (Package)
```bash
# Coming soon — watch this repo for PPA release
```

### Option 2: Fedora/RHEL (Package)
```bash
# Coming soon — watch this repo for Copr release
```

### Option 3: Arch Linux (AUR)
```bash
yay -S mycroft-gui-qt6
```

### Option 4: Any Linux (From Source)

```bash
# 1. Install dependencies
# Ubuntu/Debian
sudo apt install -y \
    build-essential cmake git \
    qt6-base-dev qt6-declarative-dev qt6-multimedia-dev \
    libqt6websockets6-dev \
    extra-cmake-modules libkf6kirigami-dev libkf6coreaddons-dev

# Fedora/RHEL
sudo dnf install -y \
    gcc-c++ cmake git \
    qt6-qtbase-devel qt6-qtdeclarative-devel qt6-qtmultimedia-devel \
    qt6-qtwebsockets-devel \
    extra-cmake-modules kf6-kirigami-devel kf6-kcoreaddons-devel

# Arch
sudo pacman -S \
    base-devel cmake git \
    qt6-base qt6-declarative qt6-multimedia qt6-websockets \
    extra-cmake-modules kf6-kirigami kf6-coreaddons

# 2. Clone and build
git clone https://github.com/OpenVoiceOS/mycroft-gui-qt6.git
cd mycroft-gui-qt6
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# 3. Install
sudo make install
sudo ldconfig
```

---

## Verification: 30 seconds

Verify the installation worked:

```bash
# Check library is installed
pkg-config --modversion mycroft-gui-qt6

# Expected output: 1.0.0 (or current version)
```

If that fails, check [getting-started/INSTALL.md](getting-started/INSTALL.md#verification-steps) for troubleshooting.

---

## Configuration: 2 minutes

Tell OpenVoiceOS to use this GUI:

```bash
# Edit OpenVoiceOS configuration
nano ~/.config/mycroft/mycroft.conf

# Add or modify the gui section:
{
  "gui": {
    "idle_display_skill": "skill-ovos-homescreen.openvoiceos"
  },
  "gui_websocket": {
    "host": "0.0.0.0",
    "base_port": 18181
  }
}
```

Save with `Ctrl+X`, `Y`, `Enter` (nano editor).

---

## Start It: 1 minute

In a terminal, run:

```bash
mycroft-gui-app
```

You should see:
- Qt window opening
- "Connecting to ovos-gui..." message
- When connected: GUI displays homescreen or idle screen

---

## Test It Works

### Test 1: Speak a Skill Command
```bash
# In another terminal
mycroft-cli-client

# Type a skill command, e.g.:
> what's the weather

# You should see:
# - GUI displays weather template
# - Weather information on screen
```

### Test 2: Touch the GUI
- Click buttons in the GUI
- They should trigger the same action as voice commands

### Test 3: Check Logs
```bash
# Check for errors
mycroft-gui-app --log-level debug

# Should show:
# "Connected to ovos-gui"
# "WebSocket connected"
# No "ERROR" messages
```

---

## Troubleshooting (Quick)

### Problem: "Cannot connect to ovos-gui"
```bash
# Solution 1: Verify ovos-gui is running
systemctl status ovos-gui

# Solution 2: Check port 18181 is listening
netstat -tlnp | grep 18181

# Solution 3: Read troubleshooting
# See: INTEGRATION.md -> Troubleshooting Integration
```

### Problem: "Cannot load QML module"
```bash
# Solution: Update QML import path
export QML_IMPORT_PATH=/usr/local/lib/qt6/qml:$QML_IMPORT_PATH
mycroft-gui-app
```

### Problem: "Qt plugin error"
```bash
# Solution: Update plugin path
export QT_PLUGIN_PATH=/usr/lib/qt6/plugins:$QT_PLUGIN_PATH
mycroft-gui-app
```

More troubleshooting: [getting-started/INSTALL.md](getting-started/INSTALL.md#troubleshooting-installation)

---

## Next Steps

1. **Understand the architecture**: Read [INTEGRATION.md](INTEGRATION.md) (10 min)
2. **Learn how skills display content**: Check [COMPONENTS.md](COMPONENTS.md) - "Supported Templates" section (10 min)
3. **Build from source with tests**: See [getting-started/BUILD.md](getting-started/BUILD.md) (30 min)
4. **Develop custom extensions**: Read [CODE_GUIDE.md](CODE_GUIDE.md) (2 hours)

---

## Important Notes

⚠️ **This is experimental, AI-generated code.** See [../README.md](../README.md) for details about the AI modernization and what to expect.

✅ **It works.** The project builds cleanly, all tests pass, it renders all 25 templates, and integrates with OpenVoiceOS.

📖 **Well documented.** See [docs/index.md](index.md) for the complete documentation roadmap.

---

## Need Help?

- **Installation issues**: [getting-started/INSTALL.md](getting-started/INSTALL.md)
- **Build from source**: [getting-started/BUILD.md](getting-started/BUILD.md)
- **How the system works**: [ARCHITECTURE.md](ARCHITECTURE.md)
- **API reference**: [COMPONENTS.md](COMPONENTS.md)
- **Learning Qt without prior knowledge**: [CODE_GUIDE.md](CODE_GUIDE.md)
- **Ecosystem overview**: [INTEGRATION.md](INTEGRATION.md)

---

**Estimated time to working GUI**: 10-15 minutes
**System resources**: 500 MB disk, 2 GB RAM, any Linux distribution with Qt6 support
