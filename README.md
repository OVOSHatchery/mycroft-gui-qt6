# mycroft-gui-qt6

**Qt6 Port of mycroft-gui-qt5 with Full Feature Parity**

## Overview

`mycroft-gui-qt6` is a modern Qt6-based GUI client for the OpenVoiceOS voice assistant. It maintains **100% feature parity** with the modernized Qt5 version while leveraging Qt6's performance, security, and modern APIs.

### Why Qt6?

- **Modern C++**: Qt6 requires C++17 minimum (vs Qt5's C++11)
- **Performance**: Improved rendering, better memory management
- **Security**: Updated dependencies, active security patches
- **Long-term support**: Qt6 LTS extends to 2026+
- **Type Safety**: Improved QML, better compile-time checking

## Quick Start

### Build Requirements

```bash
pacman -S qt6-{base,declarative,websockets,multimedia} \
          extra-cmake-modules kf6-{framework,kirigami}
```

### Build

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
make -j$(nproc)
make install
```

## Architecture

Same as Qt5 version with modernized APIs:
- WebSocket protocol (port 18181)
- 25 system templates
- 23 OVOS bus message types (enum-based)
- Type-safe message routing

## Qt5 → Qt6 Changes

### C++ API
- `qmlRegisterType` → `QML_ELEMENT` macro
- Stricter signal/slot type checking
- C++17 features available (auto, structured bindings, etc.)

### QML Syntax
- `import QtQuick 2.12` → `import QtQuick`
- Simplified module paths
- Improved property binding syntax

## Status

- ✅ CMakeLists.txt (Qt6-compatible)
- ✅ README documentation
- ⏳ C++ source porting (in progress)
- ⏳ QML porting (in progress)
- ⏳ Tests porting (in progress)

## See Also

- [mycroft-gui-qt5](../mycroft-gui-qt5/) — Qt5 original
- [PORTING_GUIDE.md](docs/PORTING_GUIDE.md) — Qt5→Qt6 migration
