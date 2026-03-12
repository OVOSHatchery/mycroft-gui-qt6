# mycroft-gui-qt6 Architecture

## Overview

`mycroft-gui-qt6` is a Qt6-based graphical user interface client for the OpenVoiceOS (OVOS) voice assistant platform. It provides real-time rendering of skill UIs using a template-based system combined with traditional Qt/QML components.

## Core Principles

1. **Protocol-Driven**: Communication with OVOS core happens via WebSocket protocol on port 18181
2. **Template-Based UI**: Skills don't send custom QML; they use predefined templates with data
3. **Real-Time Updates**: Session data is pushed from server to client via WebSocket messages
4. **Model-View Architecture**: C++ models expose skill data to QML views

## System Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                    OpenVoiceOS Core (Python)                    │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │ Legacy Plugin Adapter (mycroft-gui protocol translation) │  │
│  └───────────────────────────────────────────────────────────┘  │
│                           │                                      │
│                    WebSocket (port 18181)                        │
│                           │                                      │
└─────────────────────────────────────────────────────────────────┘
                            │
                            │
┌─────────────────────────────────────────────────────────────────┐
│                   mycroft-gui-qt6 (This Project)                │
│                                                                  │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │            MycroftController (Singleton)                │   │
│  │  • Manages WebSocket connection                         │   │
│  │  • Routes incoming messages to skill views              │   │
│  │  • Sends user interactions back to server               │   │
│  └──────────────────┬──────────────────────────────────────┘   │
│                     │                                            │
│        ┌────────────┼────────────┐                              │
│        │            │            │                              │
│  ┌─────▼──┐  ┌─────▼──┐  ┌─────▼──────────────┐               │
│  │Skill 1 │  │Skill 2 │  │System Templates    │               │
│  │View    │  │View    │  │(Text, List, etc)   │               │
│  └────────┘  └────────┘  └────────────────────┘               │
│        │            │            │                              │
│        └────────────┼────────────┘                              │
│                     │                                            │
│            Session Data Models                                  │
│     (ActiveSkillsModel, DelegatesModel)                         │
│                     │                                            │
│        QML Framework (Qt 2.12+ syntax)                          │
│        ✓ Qt/QtQuick primitives                                 │
│        ✓ Kirigami UI components                                │
│        ✓ Custom components (AudioPlayer, SlideShow, etc)       │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

## Key Components

### 1. MycroftController (`import/mycroftcontroller.h/cpp`)

**Role**: Central message router and connection manager

**Responsibilities**:
- Maintains WebSocket connection to OVOS core
- Parses incoming messages (json format)
- Routes messages to registered skill views via enum-based message types
- Handles connection lifecycle (connect, disconnect, reconnect)
- Exposes properties to QML (status, speaking, listening)

**Key Classes**:
```cpp
class MycroftController : public QObject {
    Q_OBJECT
    QML_ELEMENT  // Makes it available to QML
    
public:
    enum Status { Connecting, Open, Closing, Closed, Error };
    static MycroftController* instance();  // Singleton
    void registerView(AbstractSkillView *view);
    void sendRequest(const QString &type, const QVariantMap &data);
};
```

**Protocol Flow**:
1. Client connects to `ws://localhost:18181`
2. Server announces available templates
3. Client registers skill views
4. Server sends skill data when activated
5. Client renders appropriate template with server data
6. User interactions sent back to server via sendRequest()

### 2. AbstractSkillView (`import/abstractskillview.h/cpp`)

**Role**: Base class for skill UI rendering

**Responsibilities**:
- Manages per-skill WebSocket connection
- Translates server-sent skill data into QML-accessible models
- Handles skill session lifecycle (activation, deactivation)
- Coordinates between global MycroftController and skill-specific data

**Key Data Structures**:
```cpp
class AbstractSkillView : public QQuickItem {
    Q_OBJECT
    QML_ELEMENT
    
private:
    QHash<QString, SessionDataMap*> m_skillData;  // Per-skill session data
    ActiveSkillsModel* m_activeSkillsModel;       // Track active skills
    QWebSocket* m_guiWebSocket;                   // Skill-specific connection
};
```

**Session Data Management**:
- Each skill gets a `SessionDataMap` (QML property map)
- Server sends updates via `SessionDataModel` messages
- QML components bind to these properties for real-time updates

### 3. Message Routing (`import/guibusmessages.h`)

**Role**: Type-safe message type enumeration

**Why It Matters**:
- Qt5 code used string literals like "gui.page.show" (error-prone)
- Qt6 code uses `GUIBusMessageType::PageShow` (compile-time checked)
- 23 OVOS bus message types defined

**Example Message Types**:
```cpp
enum class GUIBusMessageType {
    PageShow,           // Display a skill template
    PageDelete,         // Remove a skill from UI
    SessionDataUpdate,  // Update session properties
    ClearNamespace,     // Clean up when skill exits
    // ... 19 more types
};
```

### 4. Model Classes (`import/*model*.h/cpp`)

#### ActiveSkillsModel
- Tracks which skills are active
- Emits signals when skills are added/removed
- Used by QML to populate skill lists

#### SessionDataModel
- Holds dynamic key-value data for each skill
- Implements QAbstractTableModel for SQL-like access
- Reflects server updates in real-time

#### DelegatesModel
- Maps skill IDs to QML delegate components
- Manages delegate lifecycle (creation, destruction)

### 5. QML Framework (`import/qml/` directory)

**System Templates** (`import/system-templates/`):
- 25 predefined templates for common skill patterns
- Examples: Text, List, Image, AudioPlayer, VideoPlayer
- Designed by OVOS team for consistency

**Framework Components** (`import/qml/`):
- 17 reusable UI components
- Examples: AutoFitLabel, SlideShow, CardDelegate
- Supporting components for template customization
- Some marked as deprecated (old Mycroft.Delegate pattern)

## Data Flow Example

### Scenario: User Speaks, Gets Weather Info

1. **OVOS Core** (Python)
   - Processes speech: "What's the weather?"
   - Activates weather skill
   - Sends to MycroftController:
     ```json
     {
       "type": "gui.page.show",
       "data": {
         "template": "Weather",
         "current_temp": 72,
         "conditions": "Sunny"
       }
     }
     ```

2. **MycroftController** (C++)
   - Receives message on WebSocket
   - Parses JSON
   - Routes to: `GUIBusMessageType::PageShow`
   - Calls `AbstractSkillView::onGuiSocketMessageReceived()`

3. **AbstractSkillView** (C++)
   - Updates `SessionDataMap` with weather data
   - Emits `sessionDataChanged()` signal
   - QML automatically re-renders

4. **QML** (Qt/Kirigami)
   - Detects `Weather` template in data
   - Loads `system-templates/Weather.qml`
   - Binds to SessionDataMap properties
   - Displays: "Current: 72°F, Sunny"

5. **User Interaction**
   - Taps button in Weather template
   - QML sends: `Mycroft.Controller.sendRequest("skill.weather.details", {})`
   - Back to step 1 (cycle repeats)

## Connection Lifecycle

```
DISCONNECTED
    ↓
[connect() called]
    ↓
CONNECTING (connecting websocket)
    ↓
[websocket connected]
    ↓
OPEN (ready for messages)
    ↑        ↓
    │    [error or disconnect]
    │        ↓
    │    CLOSING
    │        ↓
    │    CLOSED
    └────[reconnect timer triggered]
```

## Threading Model

- **Qt Event Loop**: Single-threaded (standard for Qt/QML apps)
- **WebSocket**: Handled by Qt's internal thread pool
- **QML Rendering**: Qt's scene graph renderer (GPU)
- **Signal/Slots**: Thread-safe, all in main thread

## Plugin Registration (Qt6 vs Qt5)

### Qt5 Approach (Old)
```cpp
// mycroft-plugin.cpp
qmlRegisterType<MycroftController>("Mycroft", 1, 0, "MycroftController");
```

### Qt6 Approach (Modern)
```cpp
// mycroftcontroller.h
class MycroftController : public QObject {
    Q_OBJECT
    QML_ELEMENT  // ← Automatic registration!
};
```

**Benefits of Qt6 approach**:
- Compile-time checking
- Less boilerplate code
- Better IDE support
- Centralized metadata

## Session Data Example

When a skill requests template, session data flows like this:

```
Skill sends to core: "I need Weather template"
         │
         ↓
Core formats for GUI:
{
  "type": "gui.page.show",
  "data": {
    "template": "Weather",
    "location": "San Francisco",
    "temp": 72,
    "icon": "sunny"
  }
}
         │
         ↓
MycroftController receives + parses
         │
         ↓
AbstractSkillView.m_skillData["weather_skill"] updates:
  location → "San Francisco"
  temp → 72
  icon → "sunny"
         │
         ↓
QML binds to these properties:
  Text { text: skillData.location }  // "San Francisco"
  Image { source: skillData.icon }   // Loads sunny.png
         │
         ↓
User sees weather in real-time (sub-100ms update)
```

## Error Handling Strategy

1. **Connection Errors**: 
   - Automatic reconnect with exponential backoff
   - Status property updated for QML to show error state

2. **Message Parsing Errors**:
   - Log to console
   - Skip malformed messages
   - Continue processing next message

3. **QML/Template Errors**:
   - Qt's declarative engine logs and continues
   - Broken template shows as gray box
   - Doesn't crash app

## Memory Management

- **Singleton Pattern**: MycroftController exists for app lifetime
- **Skill Views**: Created on demand, destroyed when skill exits
- **Models**: Owned by their parent views, cleaned up automatically
- **Signal/Slots**: Qt handles cleanup via parent/child hierarchy

## Performance Considerations

1. **Message Rate**: Server sends updates ~10-30 times/sec
2. **QML Rendering**: Efficient thanks to Qt's scene graph
3. **Memory**: ~50MB for app + library (typical)
4. **Latency**: <100ms from server update to screen render

