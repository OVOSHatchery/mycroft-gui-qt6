# Code Maintenance Guide for Non-Qt Developers

This guide explains how to understand and maintain mycroft-gui-qt6 code without deep Qt expertise.

## Quick Mental Model

Think of Qt applications like this:

```
┌─────────────────────┐
│   Application       │
│  (main event loop)  │
└──────────┬──────────┘
           │
    ┌──────▼──────┐
    │   Objects   │  (C++ classes)
    │   & Data    │
    └──────┬──────┘
           │
    ┌──────▼──────┐
    │     QML     │  (UI markup, like HTML)
    │  Templates  │
    └─────────────┘
```

**Key Concept**: Qt constantly monitors objects for changes and updates the UI automatically.

## Common File Types

### 1. Header Files (`.h`)

Declare what classes do. Example structure:

```cpp
#pragma once  // Prevent double-inclusion

#include <QObject>  // Base class from Qt
#include <QString>  // Qt's string type (like Python's str)

class MyClass : public QObject {  // Inherit from Qt base class
    Q_OBJECT  // Qt magic macro - makes introspection work
    
public:
    MyClass();  // Constructor
    ~MyClass(); // Destructor
    
    void doSomething();  // Public method (call from anywhere)
    
private:
    QString m_data;  // Member variable (starts with m_)
    
Q_SIGNALS:  // Qt's observer pattern
    void dataChanged();  // Emit when something happens
};
```

**Key Pattern**: Methods starting with `m_` are private members (don't touch from outside)

### 2. Implementation Files (`.cpp`)

Contains actual code. Example:

```cpp
#include "myclass.h"

MyClass::MyClass() {
    // Constructor - runs once at creation
    m_data = "initial";
}

void MyClass::doSomething() {
    m_data = "changed";
    Q_EMIT dataChanged();  // Tell observers something changed
}
```

### 3. QML Files (`.qml`)

UI markup (like HTML). Example:

```qml
import QtQuick
import QtQuick.Controls

Rectangle {  // Base shape (like a div)
    width: 200
    height: 100
    color: "lightblue"
    
    Text {  // Display text (like <p>)
        text: "Hello"  // Static text
        anchors.centerIn: parent  // Center in parent
    }
    
    Button {  // Clickable button
        text: "Click me"
        onClicked: {
            // C++ signal handler
            MyObject.doSomething()
        }
    }
}
```

**Key Idea**: QML is declarative (you describe what it should look like, not how to draw it)

## Signal/Slot System (Qt's Observer Pattern)

This is how different parts notify each other:

```cpp
// In header
class MyObject : public QObject {
    Q_OBJECT
    
Q_SIGNALS:  // These can be emitted to notify listeners
    void valueChanged(int newValue);
    
public Q_SLOTS:  // These respond to signals
    void handleValueChange(int val);
};

// In implementation
void MyObject::handleValueChange(int val) {
    qDebug() << "Value changed to" << val;
}
```

**Connecting signals to slots**:
```cpp
MyObject obj1, obj2;
// When obj1 emits valueChanged, obj2's handleValueChange runs
connect(&obj1, &MyObject::valueChanged, 
        &obj2, &MyObject::handleValueChange);
```

**In QML**:
```qml
Button {
    onClicked: {
        // This is a signal handler
        MyObject.sendMessageToServer()
    }
}
```

## Property System (Bindings)

Qt properties automatically update the UI:

```cpp
// Header
class MyClass : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    
public:
    QString getName() { return m_name; }
    void setName(const QString& n) { 
        m_name = n; 
        Q_EMIT nameChanged();  // Notify UI
    }
    
Q_SIGNALS:
    void nameChanged();
    
private:
    QString m_name;
};
```

**In QML**:
```qml
Text {
    text: MyObject.name  // Automatically updates when C++ changes it
}
```

## Common Qt Patterns in This Project

### 1. Singleton Pattern (OVOSController)

```cpp
class OVOSController : public QObject {
public:
    static OVOSController* instance() {
        static OVOSController* s_instance = nullptr;
        if (!s_instance) s_instance = new OVOSController();
        return s_instance;
    }
};
```

**Why**: Only one controller per app (represents "the server connection")

**Usage**: 
```cpp
OVOSController::instance()->sendMessage("hello");
```

### 2. Parent-Child Ownership

```cpp
class Parent : public QObject {
public:
    Parent() {
        // Child automatically deleted when parent is deleted
        new Child(this);  // 'this' is the parent
    }
};
```

**Why**: Prevents memory leaks - Qt cleans up automatically

### 3. Q_ENUM and Message Types

```cpp
// Header
class MyClass : public QObject {
    Q_OBJECT
    
public:
    enum MessageType {
        PageShow,
        PageDelete,
        DataUpdate
    };
    Q_ENUM(MessageType)
};

// Usage - Compile time checked!
MyClass::MessageType type = MyClass::PageShow;
```

**Why**: Type-safe instead of string literals ("page.show")

## Threading Notes

**Important**: Qt is single-threaded by design

```cpp
// All of this runs on "main thread"
void MyClass::sendMessage() {
    // ✓ OK - runs on main thread
    MyObject.doSomething();
    
    // ✗ DON'T - would crash, runs on different thread!
    // thread.start(lambda { MyObject.doSomething(); });
}
```

**Exception**: Network operations (WebSocket) are handled internally by Qt

## Memory Management Checklist

When writing code:

```cpp
// ✓ GOOD - parent handles deletion
new Child(parentPointer);  // parentPointer must be valid QObject

// ✓ GOOD - explicit parent set later
Child* c = new Child();
c->setParent(parent);

// ✗ BAD - no parent, will leak!
new Child();

// ✓ GOOD - stack variable, auto-deleted
MyClass obj;

// ✓ GOOD - smart pointer (modern C++)
std::unique_ptr<MyClass> obj(new MyClass());
```

## Debugging Tips

### 1. Print Debug Messages

```cpp
#include <QDebug>

qDebug() << "Value is:" << m_value;
// Output: Value is: 42

qWarning() << "Something wrong";  // Yellow warning
qCritical() << "Error!";          // Red error
```

### 2. Check Object Connection Status

```cpp
bool connected = connect(obj1, &MyClass::signal1,
                        obj2, &MyClass::slot1);
if (!connected) {
    qWarning() << "Connection failed!";
}
```

### 3. QML Debugging

```qml
import QtQuick

Rectangle {
    Component.onCompleted: {
        console.log("Rectangle created");  // Log when component loads
        console.warn("This is a warning");
    }
}
```

## Common Mistakes

### Mistake 1: Modifying objects from wrong context
```cpp
// ✗ DON'T - this crashes if called from different thread
void setValueFromNetwork(int val) {
    m_value = val;  // ← Might crash!
}

// ✓ DO - use Qt's signal/slot mechanism
void NetworkHandler::onDataReceived(int val) {
    Q_EMIT valueReceived(val);  // Signal on any thread is safe
}
// Connect to: MyObject::setValue(int) which updates on main thread
```

### Mistake 2: Calling C++ methods directly from QML
```qml
// ✗ DON'T
Button {
    onClicked: MyObject.internalPrivateMethod()  // Might break encapsulation
}

// ✓ DO - use public slots
Button {
    onClicked: MyObject.userDidClickButton()  // Clear intent
}
```

### Mistake 3: Memory in lambdas
```cpp
// ✗ DANGEROUS - if obj is deleted, this crashes!
connect(button, &Button::clicked, [this]() {
    someObject->doSomething();  // 'someObject' might be invalid
});

// ✓ SAFE - use context object
connect(button, &Button::clicked, someObject, [this]() {
    someObject->doSomething();  // Qt disconnects if someObject deleted
});
```

## Reading Existing Code

### Step 1: Find the Entry Point
- Look in `application/main.cpp`
- Understand what gets created first

### Step 2: Trace Key Classes
- `OVOSController` - handles server connection
- `AbstractSkillView` - displays individual skill UIs
- `SessionDataModel` - holds skill data

### Step 3: Look for Q_PROPERTY

```cpp
Q_PROPERTY(bool speaking READ isSpeaking NOTIFY isSpeakingChanged)
//         ^type  ^name   ^getter      ^signal when changes
```

This tells you:
- Variable name: `speaking`
- How to read it: `isSpeaking()`
- When it changes: `isSpeakingChanged()` signal

### Step 4: Follow Signal/Slot Connections

Find `connect()` calls - these show how objects talk to each other:
```cpp
connect(sender, &SenderClass::signal, 
        receiver, &ReceiverClass::slot);
//      ↑ This happens     ↓ Triggers this
```

## Testing Strategy

```cpp
// Test files in autotests/ directory

#include <QtTest>

class MyTest : public QObject {
    Q_OBJECT
    
private Q_SLOTS:
    void test_something() {
        MyClass obj;
        obj.doSomething();
        QCOMPARE(obj.getValue(), expectedValue);  // Assert
    }
};

QTEST_MAIN(MyTest)  // Run this test
```

**Run tests**:
```bash
./servertest           # Test server communication
./modeltest           # Test data models
./message_routing_test  # Test message types
```

## Documentation Standard

Every class should have:

```cpp
/**
 * @class MyClass
 * @brief What does this class do in one sentence?
 * 
 * Longer description of purpose and responsibilities.
 * 
 * Example:
 * @code
 * MyClass obj;
 * obj.doSomething();
 * @endcode
 * 
 * @note Thread safety: This class is not thread-safe.
 * @see RelatedClass
 */
class MyClass : public QObject {
    // ...
};
```

## Getting Help

1. **Qt Documentation**: https://doc.qt.io/qt-6/ (official reference)
2. **Q_PROPERTY System**: Search for "Qt properties" in docs
3. **Signal/Slots**: Search "Qt signals and slots"
4. **QML Guide**: https://doc.qt.io/qt-6/qtqml-index.html

## Before Making Changes

1. ✓ Understand which class you're modifying
2. ✓ Look at existing examples in the file
3. ✓ Check if similar code exists elsewhere
4. ✓ Run tests before and after
5. ✓ Write what changed in commit message

