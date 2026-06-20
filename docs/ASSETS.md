# Assets & Resources

Documentation for all images, icons, and resources in mycroft-gui-qt6.

## Image Files Overview

```
application/
├── background.png          # OVOS logo background (skill mode)
├── banner.png             # Drawer header image
├── images/
│   └── microphone.svg     # Microphone icon
└── shell/
    └── icons/             # Shell mode icons (see below)

icons/
├── 16-apps-mycroft.svg   # App icon 16px
└── 32-apps-mycroft.svg   # App icon 32px
```

## Application Layer Assets

### `application/background.png`

**Location**: `application/background.png`

**Purpose**: OVOS logo background displayed when no skill is active

**Used In**:
- `application/main.qml:281` — Main skill GUI background
- `application/shell/qml/main.qml:175` — Shell mode background

**When Visible**: Shows when `mainView.currentItem` is null (no skill GUI displayed)

---

### `application/banner.png`

**Location**: `application/banner.png`

**Purpose**: Header image in the global drawer (side navigation menu)

**Used In**:
- `application/main.qml:142` — GlobalDrawer header

---

## Shell Mode Icons

All shell icons are located in `application/shell/icons/` and are used in **homescreen mode** (`--shell` flag).

### Brand Icons

| File | Purpose | Used In |
|------|---------|---------|
| `ovos-wave.svg` | OVOS logo wave animation | `SplashScreen.qml` |
| `ovos-egg.svg` | Fallback OVOS logo | `SplashScreen.qml` |

### Audio Icons

| File | Purpose | Used In |
|------|---------|---------|
| `mic.svg` | Microphone active | WeatherArea, homescreen |
| `mic-mute.svg` | Microphone muted | Status areas |
| `volume-high.svg` | Volume high | Quick settings |
| `volume-mute.svg` | Volume muted | Quick settings |

### UI Action Icons

| File | Purpose | Used In |
|------|---------|---------|
| `home.svg` | Home button | Navigation |
| `configure.svg` | Settings | Drawer actions |
| `delete.svg` | Delete action | Notifications |
| `clear.svg` | Clear action | idle.qml |
| `close.svg` | Close dialog | Notification popups |
| `dialog-close.svg` | Close dialog variant | idle.qml |

### System Icons

| File | Purpose | Used In |
|------|---------|---------|
| `system-shutdown.svg` | Power off | ShutdownOptions.qml |
| `system-shutdown-solid.svg` | Power off (solid) | ShutdownOptions.qml |
| `system-reboot.svg` | Restart | ShutdownOptions.qml |
| `wireless.svg` | WiFi status | StatusIndicator |
| `screen-rotate.svg` | Screen rotation | Quick settings |
| `notification-icon.svg` | Notification indicator | WidgetsArea |

### Media Icons

| File | Purpose | Used In |
|------|---------|---------|
| `scrub.png` | Scrubbing handle | ShutdownOptions.qml (power slider) |

## Quick Settings Panel Icons

Located in `application/shell/qml/panel/quicksettings/`:

| File | Purpose |
|------|---------|
| `brightness-increase.svg` | Brightness up |
| `brightness-decrease.svg` | Brightness down |

## How Icons Are Loaded

### Method 1: KDE System Icons (Preferred)

Most icons use KDE's built-in icon theme via `icon.name`:

```qml
Kirigami.Action {
    icon.name: "configure"    // Uses system KDE icon
    icon.name: "help-about"   // Uses system KDE icon
    icon.name: "go-previous"  // Back button
}
```

### Method 2: Custom SVG/PNG via Qt.resolvedUrl()

Custom OVOS icons are loaded from the resource system:

```qml
Image {
    source: Qt.resolvedUrl("icons/ovos-wave.svg")
}
```

### Method 3:.qrc Resource Path

Some components use Qt's resource compiler path:

```qml
source: "qrc://icons/mid-solid"
```

## Qt Resource Files (.qrc)

Images are compiled into the binary via Qt resource files:

| .qrc File | Contents |
|-----------|----------|
| `application/qml.qrc` | `background.png`, `banner.png` |
| `application/shell-qml.qrc` | Shell mode resources |

### Example qml.qrc

```xml
<RCC>
    <qresource prefix="/">
        <file alias="background.png">background.png</file>
        <file alias="banner.png">banner.png</file>
    </qresource>
</RCC>
```

## Icon Naming Conventions

1. **System icons**: Use KDE icon names (e.g., `configure`, `help-about`)
2. **Custom icons**: Use descriptive names (e.g., `ovos-wave.svg`)
3. **Size variants**: If needed, suffix with size (e.g., `icon-16.svg`)

## Adding New Icons

1. Add SVG/PNG to appropriate directory:
   - Shell icons → `application/shell/icons/`
   - Quick settings → `application/shell/qml/panel/quicksettings/`

2. Update `.qrc` file if not using KDE system icons

3. Use in QML:
   ```qml
   // System icon
   icon.name: "new-icon"
   
   // Custom icon
   Image {
       source: Qt.resolvedUrl("icons/new-icon.svg")
   }
   ```

## Troubleshooting

### Icon not showing?
1. Check file exists in correct location
2. Verify .qrc is included in CMake
3. Use correct path: `Qt.resolvedUrl("icons/name.svg")`

### Wrong icon size?
- KDE system icons auto-scale
- Custom icons: specify `sourceSize` property

### SVG not rendering?
- Ensure SVG is valid XML
- Check file encoding is UTF-8
