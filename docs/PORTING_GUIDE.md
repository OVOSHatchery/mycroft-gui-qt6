# Qt5 → Qt6 Porting Guide

**Date**: 2026-03-12
**Source**: mycroft-gui-qt5 (modernized version)
**Target**: mycroft-gui-qt6 (full Qt6 implementation)
**Status**: Active porting in progress

---

## Overview

This guide documents the systematic porting of mycroft-gui-qt5 to Qt6, maintaining 100% feature parity while leveraging modern Qt6 APIs.

## Major Changes

### 1. CMake Configuration

**Qt5**:
```cmake
cmake_minimum_required(VERSION 3.16)
find_package(Qt5 5.15 REQUIRED COMPONENTS Core Qml Quick Network WebSockets Multimedia)
find_package(KF5 5.91 REQUIRED COMPONENTS Kirigami)
set(CMAKE_CXX_STANDARD 17)
qt5_add_resources(...)
```

**Qt6**:
```cmake
cmake_minimum_required(VERSION 3.24)
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Qml Quick Network WebSockets Multimedia)
find_package(KF6 6.0 REQUIRED COMPONENTS Kirigami)
set(CMAKE_CXX_STANDARD 17)
qt6_add_resources(...)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
```

### 2. C++ API Changes

#### Header Includes
```cpp
// Qt5
#include <QObject>
#include <QQmlComponent>

// Qt6 (mostly same, but some reorganization)
#include <QObject>
#include <QQmlComponent>
```

#### QML Registration
```cpp
// Qt5
class MycroftController : public QObject {
    Q_OBJECT
};

// In main.cpp
qmlRegisterType<MycroftController>("Mycroft", 1, 0, "MycroftController");

// Qt6 - Use declarative macro
class MycroftController : public QObject {
    Q_OBJECT
    QML_ELEMENT
};
```

#### Qt Namespace
```cpp
// Qt5 & Qt6 both work
connect(..., QOverload<>::of(&signal), ...);

// Qt6 preferred (cleaner with lambdas)
connect(..., &signal, receiver, [](auto value) { ... });
```

#### Signal/Slot Changes
```cpp
// Qt5
signals:
    void messageReceived(const QString &type, const QVariantMap &data);

slots:
    void onSocketConnected();

// Qt6 (same syntax, but stricter type checking)
signals:
    void messageReceived(const QString &type, const QVariantMap &data);

public slots:
    void onSocketConnected();
```

### 3. QML Syntax Changes

#### Import Statements
```qml
// Qt5
import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtMultimedia 5.12
import org.kde.kirigami 2.14 as Kirigami

// Qt6
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtMultimedia
import org.kde.kirigami as Kirigami
```

#### Property Declarations
```qml
// Qt5
Rectangle {
    property string title: "Default"
    property bool enabled: true
    property int count: 0
}

// Qt6 (with typing and required keyword)
Rectangle {
    required property string title
    property bool enabled: true
    property int count: 0
}
```

#### Property Bindings
```qml
// Qt5 - function bindings
Rectangle {
    color: {
        if (dark) return "#000000"
        else return "#FFFFFF"
    }
}

// Qt6 - cleaner with ternary and property types
Rectangle {
    required property bool dark
    color: dark ? "#000000" : "#FFFFFF"
}
```

#### Anchors and Layouts
```qml
// Qt5 & Qt6 (mostly compatible)
Rectangle {
    anchors.fill: parent
    anchors.margins: 10
}

// Qt6 also supports property bindings on layout properties
Row {
    spacing: isCompact ? 5 : 10  // Binding works directly
}
```

### 4. Component-Specific Changes

#### WebSocket
```cpp
// Qt5
#include <QWebSocket>
QWebSocket socket;
socket.open(QUrl("ws://localhost:18181"));

// Qt6 (same API)
#include <QWebSocket>
QWebSocket socket;
socket.open(QUrl("ws://localhost:18181"));
```

#### QML Components
```qml
// Qt5
Delegate {
    id: control
    ColumnLayout {
        anchors.fill: parent
        spacing: Kirigami.Units.largeSpacing
    }
}

// Qt6 (same, but can use simplified imports)
Delegate {
    id: control
    ColumnLayout {
        anchors.fill: parent
        spacing: Kirigami.Units.largeSpacing
    }
}
```

## Porting Checklist

### Phase 1: Build System
- [ ] Update CMakeLists.txt (Qt6 syntax)
- [ ] Update find_package calls (Qt6, KF6)
- [ ] Set CMAKE_AUTOMOC, AUTORCC
- [ ] Test cmake configuration

### Phase 2: C++ Headers
- [ ] Update includes (new locations)
- [ ] Replace qmlRegisterType with QML_ELEMENT macro
- [ ] Update signal/slot declarations
- [ ] Replace deprecated Q_ENUMS with Q_ENUM
- [ ] Update connect() calls for stricter type checking

### Phase 3: C++ Implementation
- [ ] Update QML engine registration
- [ ] Replace deprecated APIs
- [ ] Update message handling (use existing enum)
- [ ] Verify all slots are public
- [ ] Update event handlers

### Phase 4: QML Files (System Templates)
- [ ] Update import statements (simplified paths)
- [ ] Update property declarations (add types)
- [ ] Update Kirigami references (2.14 → 6.x)
- [ ] Update color syntax (if needed)
- [ ] Test rendering

### Phase 5: QML Files (Framework Components)
- [ ] Update Delegate.qml and variants
- [ ] Update ScrollableDelegate, ProportionalDelegate
- [ ] Update AudioPlayer, VideoPlayer
- [ ] Update all utility components (AutoFitLabel, etc.)

### Phase 6: Tests
- [ ] Port message_routing_test.cpp
- [ ] Port qml_framework_components_test.cpp
- [ ] Add Qt6-specific tests
- [ ] Test WebSocket protocol

### Phase 7: Documentation
- [ ] Update PROTOCOL.md (if needed)
- [ ] Create PORTING_NOTES.md
- [ ] Update README
- [ ] Create migration guide for users

## Compatibility Notes

### What Changes
- CMake configuration (Qt6 syntax)
- QML import paths (simplified)
- QML property declarations (can add types)
- C++ registration (QML_ELEMENT macro preferred)
- Qt namespace usage (optional but recommended)

### What Stays the Same
- WebSocket protocol (port 18181)
- Message routing (enum-based, 23 types)
- Data models (mostly same C++ API)
- QML binding behavior
- Signal/slot mechanism

## Performance Improvements in Qt6

1. **QML Engine**: Faster compilation and execution
2. **Property Bindings**: Optimized binding system
3. **Memory**: Better memory management
4. **Rendering**: Improved graphics pipeline
5. **Networking**: Enhanced WebSocket handling

## Migration Timeline

| Phase | Duration | Status |
|-------|----------|--------|
| Build System | 1-2 hours | ✅ Complete |
| C++ Headers | 2-3 hours | ⏳ In progress |
| C++ Implementation | 4-6 hours | ⏳ In progress |
| QML System Templates | 2-3 hours | ⏳ Pending |
| QML Framework Components | 2-3 hours | ⏳ Pending |
| Tests | 3-4 hours | ⏳ Pending |
| Documentation | 1-2 hours | ⏳ Pending |

**Total Estimated**: 15-24 hours of development

## Testing Strategy

### Unit Tests (40+ assertions)
- Message routing (enum conversion)
- QML component loading
- Property bindings
- WebSocket protocol

### Integration Tests
- End-to-end skill activation
- Data synchronization
- User interactions
- Multi-skill scenarios

### Manual Testing
- Connect to OVOS instance
- Display various templates
- Test state changes
- Verify data updates

## Troubleshooting

### CMake Errors
```
Could not find Qt6...
```
**Solution**: Install Qt6 packages, set CMAKE_PREFIX_PATH

### QML Warnings
```
Type myClass not found
```
**Solution**: Ensure QML_ELEMENT macro is used, AUTOMOC is enabled

### Signal/Slot Mismatch
```
Could not connect...
```
**Solution**: Verify types match exactly, use lambdas for flexibility

## Resources

- [Qt6 Migration Guide](https://doc.qt.io/qt-6/porting-to-qt6.html)
- [KDE Frameworks 6 Documentation](https://api.kde.org/frameworks-static/)
- [Qt6 QML Improvements](https://doc.qt.io/qt-6/qtqml-index.html)

## See Also

- [PROTOCOL.md](PROTOCOL.md) — Message specification (unchanged)
- [TEMPLATE_VALIDATION.md](TEMPLATE_VALIDATION.md) — Template reference
- [../mycroft-gui-qt5](../mycroft-gui-qt5/) — Qt5 source
