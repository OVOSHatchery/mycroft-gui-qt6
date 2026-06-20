# Component Reference

Detailed documentation of every major component in `ovos-gui-qt6` for maintenance and development.

## C++ Classes

### GuiBusClient (`import/guibusclient.h/cpp`)

**Role**: Singleton that manages the WebSocket connection to the GUI bus and routes messages.

**Key Methods**:
```cpp
static GuiBusClient* instance();  // Get the singleton
void start();                     // Connect to bus
void sendRequest(const QString &type, const QVariantMap &data);  // Send protocol message
void registerNamespace(GuiNamespace *namespaceObj); // Register a UI namespace
```

**Key Properties**:
- `status`: `Connecting`, `Open`, `Closing`, `Closed`, `Error`
- `speaking`: Tracks the voice assistant's speaking state.
- `listening`: Tracks the voice assistant's listening state.
- `activeNamespaces`: Pointer to the `NamespaceModel`.

**Internal Logic**:
1. Upon startup, `GuiBusClient::start()` is called.
2. Connects to `ws://localhost:18181` (configurable via `GlobalSettings`).
3. Classified incoming messages via `GuiBusMessages::parseMessage()`.
4. Routes messages to the appropriate layer:
   - **Assistant events** (speaking/listening) handled internally.
   - **Protocol messages** (data/pages) forwarded to registered `GuiNamespace` instances.

---

### GuiNamespace (`import/guinamespace.h/cpp`)

**Role**: Container representing a specific UI namespace (e.g. `ovos.skill.weather`).

**Key Methods**:
```cpp
NamespaceModel *activeNamespaces() const;
Q_INVOKABLE NamespaceDataMap *namespaceDataForNamespace(const QString &namespace_id);
Q_INVOKABLE void triggerGuiEvent(const QString &eventName, const QVariantMap &parameters);
```

**What To Know**:
- Manages the `NamespaceModel` and its association with `PageModel`s.
- Resolves `SYSTEM:` template URIs to internal resources (`qrc:/system-templates/`).
- Owns the `NamespaceDataMap` instances that hold state for each active namespace.

---

### NamespaceDataMap (`import/namespacedatamap.h/cpp`)

**Role**: A reactive `QQmlPropertyMap` holding key-value pairs for a specific namespace.

**Usage**:
- When `mycroft.session.set` arrives, `GuiNamespace` updates the map.
- QML templates bind to these properties directly.
- Triggers `valueChanged` and `dataCleared` signals for UI reactivity.

---

### NamespaceModel (`import/namespacemodel.h/cpp`)

**Role**: `QAbstractListModel` tracking the stack of active namespaces and their priority.

**Roles exposed to QML**:
- `namespaceId`: The unique ID of the namespace.
- `namespaceData`: Reference to the `NamespaceDataMap`.
- `pages`: Reference to the `PageModel` containing UI pages.

---

### PageModel (`import/pagemodel.h/cpp`)

**Role**: Manages the ordered list of QML pages loaded for a specific namespace.

**Roles exposed to QML**:
- `pageUi`: The actual QML item (loaded via `PageLoader`).
- `namespaceId`: ID of the parent namespace.
- `pageUrl`: The resolved source URL of the page.

---

### MediaService (`import/mediaservice.h/cpp`)

**Role**: Centralized media playback manager using Qt6 Multimedia.

**Features**:
- Automatically handles `gui.player.*` events.
- Supports **Bundled GUI Backends**: allowing the GUI client to act as a native OCP rendering engine.
- Exposes `spectrum` data for visualizers.
- Manages `AudioProviderService` and `VideoProviderService` lifecycles.

---

## QML Templates (`qrc:/system-templates/`)

All core UI templates are bundled into the binary. They all inherit from `OVOS.Page` and access state via `namespaceData`.

### Common Templates

- **`HomeScreen.qml`**: The primary resting screen (Time, Date, Wallpaper).
- **`Weather.qml`**: Detailed weather information.
- **`Timer.qml`**: Visual countdown with progress.
- **`MediaPlayer.qml`**: Unified player for audio and video streams.
- **`OCPNowPlaying.qml`**: Native OCP interface with metadata and transport controls.
- **`Confirm.qml` / `Select.qml`**: Interactive user-input dialogs.

---

## Framework Components (`qrc:/qml/`)

Building blocks used inside templates:

- **`NamespaceView.qml`**: The core view component that renders namespaces and handles page transitions.
- **`AutoFitLabel.qml`**: Text component that scales to fit its container.
- **`MarqueeText.qml`**: Scrolling text for long titles.
- **`Units.qml`**: Provides DPI-aware measurements (`OVOS.Units.gridUnit`).

---

## Data Flow Example: Namespace Activation

1. **Server Sends**: `mycroft.session.list.insert` for `mycroft.system.active_skills`.
2. **`GuiBusClient`**: Classifies as WIRE message, forwards to `GuiNamespace`.
3. **`GuiNamespace`**: Calls `NamespaceModel::insertNamespaces()`.
4. **Server Sends**: `mycroft.session.set` with data for the namespace.
5. **`GuiNamespace`**: Updates the specific `NamespaceDataMap`.
6. **Server Sends**: `mycroft.gui.list.insert` with template URL `SYSTEM:Weather.qml`.
7. **`GuiNamespace`**: Resolves to `qrc:/system-templates/Weather.qml`, creates a `PageLoader`.
8. **`PageLoader`**: Instantiates the QML component.
9. **QML UI**: Page is displayed in the `NamespaceView`, properties bind to `namespaceData`.

---

## Build Targets

- **`ovos-gui-app`**: The main monolithic executable.
- **`OvosGuiCommon`**: Static logic library used by the app and tests.
- **`demotest`**: Headless tool to verify all templates load correctly with mock data.
