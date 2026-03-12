# Component Reference

Detailed documentation of every major component in mycroft-gui-qt6 for maintenance.

## C++ Classes

### MycroftController (import/mycroftcontroller.h/cpp)

**What It Does**: Singleton that manages the WebSocket connection to OVOS core

**Key Methods**:
```cpp
static MycroftController* instance();  // Get the singleton
void start();                          // Connect to server
void sendRequest(const QString &type, const QVariantMap &data);  // Send message
void registerView(AbstractSkillView *view);  // Register skill UI
```

**Key Properties**:
- `status`: CONNECTING, OPEN, CLOSING, CLOSED, ERROR
- `speaking`: Is the system currently speaking?
- `listening`: Is the system listening for speech?

**How It Works**:
1. App starts, calls `MycroftController::start()`
2. Attempts to connect to `ws://localhost:18181`
3. When message arrives from server:
   - Parse JSON
   - Look up message type (e.g., "gui.page.show")
   - Convert to enum (e.g., `GUIBusMessageType::PageShow`)
   - Route to registered skill view

**Common Issues**:
- Can't connect: Check port 18181, OVOS core running
- Messages not arriving: Check `registerView()` was called
- Reconnect loop: Network issues, check firewall

### AbstractSkillView (import/abstractskillview.h/cpp)

**What It Does**: Base class that represents one skill's UI

**Key Methods**:
```cpp
void registerView(const QString &skillId);  // Register skill
SessionDataMap* sessionDataForSkill(const QString &skillId);  // Get data
void triggerEvent(const QString &skillId, const QString &eventName, 
                  const QVariantMap &parameters);  // Send user action
```

**What To Know**:
- Each skill (weather, timer, etc.) gets its own AbstractSkillView instance
- Skills are NOT created manually; MycroftController creates them
- Session data is stored in QML-accessible models
- When skill exits, view is destroyed automatically

**Internal Data**:
```cpp
QHash<QString, SessionDataMap*> m_skillData;     // Per-skill data
QWebSocket* m_guiWebSocket;                      // Connection to server
ActiveSkillsModel* m_activeSkillsModel;          // Track active skills
```

### SessionDataModel (import/sessiondatamodel.h/cpp)

**What It Does**: Holds key-value pairs sent by skill server

**Example Data**:
```python
{
  "current_temp": 72,
  "conditions": "Sunny",
  "icon": "weather-sunny",
  "location": "San Francisco"
}
```

**How QML Accesses It**:
```qml
Text { text: "Temp: " + sessionData.current_temp }
Image { source: sessionData.icon }
```

**Important Note**: This is NOT editable by the client. Server owns all data. Client just displays it.

### ActiveSkillsModel (import/activeskillsmodel.h/cpp)

**What It Does**: Tracks which skills are currently active (displayed)

**Used By**: QML to show skill list, handle navigation

**Emits Signals**:
- `skillAdded()`: New skill appeared
- `skillRemoved()`: Skill closed
- `skillChanged()`: Skill data updated

### DelegatesModel (import/delegatesmodel.h/cpp)

**What It Does**: Maps skill IDs to QML templates

**Example**:
```
"weather.openvoiceos" → Weather.qml (system-templates)
"timer.openvoiceos"   → Timer.qml
"media.vlc"           → MediaPlayer.qml
```

**How It Works**:
1. Server sends: `{ "template": "Weather", ... }`
2. DelegatesModel looks up: Weather → Weather.qml path
3. QML engine loads and displays the template

### GUIBusMessageType (import/guibusmessages.h)

**What It Does**: Enum of all possible message types from server

**Why Important**: Type safety - catches typos at compile time

**All 23 Message Types**:
```cpp
enum class GUIBusMessageType {
    GuiInit,                    // Server announces capabilities
    GuiConnected,               // Connection established
    GuiDisconnected,            // Connection lost
    PageShow,                   // Display template with data
    PageDelete,                 // Remove skill from display
    PageMove,                   // Reorder skills
    SkillClosed,                // Skill shut down
    SystemUserChanged,          // User switched
    SelectionChanged,           // UI selection changed
    ToggleFullscreen,           // Enter/exit fullscreen
    TriggerSkillEvent,          // Send event to skill
    SessionDataUpdate,          // Update skill data
    SessionDataDelete,          // Remove skill data
    SessionDataClear,           // Clear all skill data
    ClearNamespace,             // Cleanup on exit
    // ... more types
};
```

**When To Use This**:
- Adding new message type: Add to enum first
- Routing message: Use `messageTypeFromString()`
- Prevent typos: Always use enum, never strings

## QML Files

### System Templates (import/system-templates/*.qml)

**Location**: `import/system-templates/`
**Count**: 25 templates
**Purpose**: Pre-built UIs for common skill types

**Template Categories**:

1. **Simple Display** (no interaction):
   - `Text.qml` - Display text, like weather description
   - `Image.qml` - Show image
   - `Html.qml` - Web content

2. **Selection/Input**:
   - `Select.qml` - Dropdown list
   - `Confirm.qml` - Yes/No dialog
   - `List.qml` - Scrollable list

3. **Media**:
   - `AudioPlayer.qml` - Play audio with controls
   - `VideoPlayer.qml` - Play video with controls
   - `MediaPlayer.qml` - Generic audio/video player
   - `OCPPlaylist.qml` - Music playlist

4. **Data Display**:
   - `Table.qml` - Tabular data (like spreadsheet)
   - `Grid.qml` - Grid layout
   - `Status.qml` - Status indicator (on/off)

5. **Special Purpose**:
   - `Clock.qml` - Digital/analog clock
   - `Weather.qml` - Weather display
   - `Timer.qml` - Countdown timer
   - `Map.qml` - Location map
   - `Loading.qml` - Spinner/progress
   - `Error.qml` - Error message display
   - `Face.qml` - Avatar/face display
   - `Idle.qml` - Default resting state

**How Skill Chooses Template**:
1. Skill sends: `{ "template": "Weather" }`
2. Client looks up: `Weather.qml`
3. Client loads and renders it
4. Data binds to template properties

**Important**: Skills CANNOT send custom QML for security reasons

### Framework Components (import/qml/*.qml)

**Location**: `import/qml/`
**Count**: 17 components
**Purpose**: Reusable UI building blocks for templates

**Useful Components**:

1. **Text/Labels**:
   - `AutoFitLabel.qml` - Text that shrinks to fit space
   - `MarqueeText.qml` - Scrolling text
   - `PaginatedText.qml` - Multi-page text

2. **Layouts**:
   - `BoxLayout.qml` - Simple box arrangement
   - `CardDelegate.qml` - Card-style container

3. **Media**:
   - `AudioPlayer.qml` - (deprecated - use template)
   - `VideoPlayer.qml` - (deprecated - use template)

4. **Interactive**:
   - `SlideShow.qml` - Image carousel
   - `SlidingImage.qml` - Sliding image transition
   - `StatusIndicator.qml` - Status light (green/red)

5. **Utility**:
   - `Units.qml` - DPI-aware measurements (margins, spacing)
   - `SoundEffects.qml` - Sound effect player
   - `Delegate.qml` - (deprecated - old skill pattern)
   - `SkillView.qml` - (deprecated - use AbstractSkillView)

**Marked Deprecated**:
The old Mycroft.Delegate pattern (Delegate.qml, etc.) is deprecated because:
- Skills sending custom QML is a security risk
- Templates provide consistency
- Template system is easier to maintain

## Data Flow Walkthrough

### Example: User Says "Show Weather"

**Step 1: OVOS Core (Python)**
```python
# weather-skill activates and sends GUI data
bus.emit_message("gui.page.show", {
    "data": {
        "template": "Weather",
        "current_temp": 72,
        "conditions": "Sunny",
        "location": "San Francisco"
    }
})
```

**Step 2: MycroftController Receives**
```cpp
void MycroftController::onMainSocketMessageReceived(const QString &message) {
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    
    // Parse the message
    QString type = doc["type"].toString();  // "gui.page.show"
    
    // Convert to enum (type-safe!)
    GUIBusMessageType msgType = GuiBusMessages::fromString(type);
    
    // Route based on type
    if (msgType == GUIBusMessageType::PageShow) {
        onPageShow(doc["data"].toObject());
    }
}
```

**Step 3: Prepare Skill View**
```cpp
void MycroftController::onPageShow(const QJsonObject &data) {
    QString skillId = data["skill_id"].toString();  // "weather.openvoiceos"
    QString templateName = data["template"].toString();  // "Weather"
    
    // Get or create view for this skill
    AbstractSkillView* view = getOrCreateView(skillId);
    
    // Update view's session data with server data
    view->sessionDataForSkill(skillId)->updateFromJson(data);
}
```

**Step 4: QML Renders Template**
```qml
// Weather.qml (template) receives data
Rectangle {
    Text { text: skillData.location }         // "San Francisco"
    Text { text: skillData.current_temp }     // "72"
    Text { text: skillData.conditions }       // "Sunny"
}
```

**Step 5: User Interacts**
```qml
// User taps "Details" button
Button {
    onClicked: {
        MycroftController.sendRequest(
            "skill.weather.details",
            {}
        )
    }
}
```

**Step 6: Back to Step 1** (cycle repeats)

## Connection States

```
[Disconnected] 
     ↓ start()
[Connecting] ← Attempting WebSocket connection
     ↓ success
[Open] ← Ready! Can send/receive messages
     ↓ server sends skill data
[Open] ← Rendering skills...
     ↓ disconnectSocket() or network error
[Closing] ← Waiting for socket to close
     ↓
[Closed] 
     ↓ reconnect() or start() again
[Connecting]
```

## Building and Testing

**To build**:
```bash
cd build
cmake .. -DBUILD_TESTING=ON
make
```

**Test executables**:
- `servertest` - Server communication
- `modeltest` - Data model functionality
- `message_routing_test` - Message type routing
- `qml_framework_components_test` - QML component loading

**Run tests**:
```bash
./build/autotests/servertest
./build/autotests/modeltest
# etc.
```

## Debugging Techniques

### 1. Add Debug Logging

In `mycroftcontroller.cpp`:
```cpp
void MycroftController::onMainSocketMessageReceived(const QString &message) {
    qDebug() << "Received message:" << message;  // Add this line
    // ... rest of code
}
```

### 2. Check Property Values in QML

```qml
Rectangle {
    Component.onCompleted: {
        console.log("MycroftController status:", MycroftController.status);
        console.log("Speaking?", MycroftController.speaking);
    }
}
```

### 3. Verify Connections

```cpp
bool ok = connect(sender, &SenderClass::signal,
                  receiver, &ReceiverClass::slot);
if (!ok) {
    qWarning() << "Connection failed!";
} else {
    qDebug() << "Connection successful!";
}
```

## Common Maintenance Tasks

### Adding a New Message Type

1. Add to `guibusmessages.h` enum:
   ```cpp
   MyNewMessage
   ```

2. Update `fromString()` function:
   ```cpp
   if (typeStr == "my.new.message") return MyNewMessage;
   ```

3. Add handler in MycroftController:
   ```cpp
   case GUIBusMessageType::MyNewMessage:
       handleMyNewMessage(data);
       break;
   ```

4. Implement handler method

### Modifying a Template

1. Edit template file: `import/system-templates/MyTemplate.qml`
2. Add/change properties: `property var skillData`
3. Test with sample data
4. Rebuild: `make`

### Adding a Framework Component

1. Create: `import/qml/MyComponent.qml`
2. Add to: `import/mycroft.qrc` if should be embedded
3. Import in using templates:
   ```qml
   import Mycroft  // Provides all framework components
   ```
4. Use in template:
   ```qml
   MyComponent { /* ... */ }
   ```

