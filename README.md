# mycroft-gui-qt6

**Qt6 Port of mycroft-gui-qt5 with Full Feature Parity**

---

## ⚠️ PROJECT STATUS: AI-GENERATED MODERNIZATION

**This project was abandoned by its original developers.** The original codebase was archived as unmaintained, with original developers lacking Qt expertise, time, and capacity for maintenance.

**Claude AI has completed a modern Qt6 port as a best-effort experiment** to explore AI usage in OpenVoiceOS modernization efforts. This is **experimental code** and should be treated as such:

- ✅ **What you get**: A functional Qt6 GUI client that builds cleanly, passes tests, and implements all 25 OVOS templates
- ⚠️ **What to expect**: Experimental code designed to validate the concept of AI-driven modernization on low-risk archived projects
- 🔍 **Code transparency**: 100% AI-generated with comprehensive documentation to enable human understanding and maintenance
- 📋 **Before using**: Read [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md), [docs/CODE_GUIDE.md](docs/CODE_GUIDE.md), and [docs/COMPONENTS.md](docs/COMPONENTS.md) to understand the design decisions and implementation details

**Use in production at your own risk.** While the code is functional and well-documented, this is an experiment in AI-assisted open source maintenance. Community contribution and human review are essential before considering this production-ready.

---

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

## Project Status

- ✅ **Phase 1**: Build infrastructure (CMakeLists.txt, Qt6 configuration)
- ✅ **Phase 2**: C++ source porting (13 files, QML_ELEMENT macro, 6.1 MB library)
- ✅ **Phase 3**: QML file conversion (25 templates, 17 components)
- ✅ **Phase 4**: Test suite (5 test executables, all passing)
- ✅ **Phase 5**: Integration testing & documentation
- ✅ **Phase 6**: Documentation complete (ARCHITECTURE.md, CODE_GUIDE.md, COMPONENTS.md, INTEGRATION.md)

**Ready for deployment.** Build cleanly, all tests pass, comprehensive documentation for users and developers.

## OpenVoiceOS Ecosystem

**mycroft-gui-qt6 is the first modern GUI client in the new OpenVoiceOS architecture.**

### Core GUI Infrastructure

- **[ovos-gui](https://github.com/OpenVoiceOS/ovos-gui)** — Central GUI messagebus service that manages GUI state and implements the protocol
- **[ovos-gui-api-client](https://github.com/OpenVoiceOS/ovos-gui-api-client)** — Python library that skills use to send GUI data (templates, session data)
- **[ovos-legacy-mycroft-gui-plugin](https://github.com/OpenVoiceOS/ovos-legacy-mycroft-gui-plugin)** — **Adapter bridge** that connects mycroft-gui-qt6 to the new ovos-gui service

### How It Connects

mycroft-gui-qt6 uses the **legacy adapter plugin** to bridge the old WebSocket protocol (port 18181) to the new ovos-gui service. This allows modern Qt6 GUI clients to work seamlessly with the current OpenVoiceOS architecture.

```
Skills (using ovos-gui-api-client)
         ↓
    ovos-gui (core service)
         ↓
ovos-legacy-mycroft-gui-plugin (adapter)
         ↓
mycroft-gui-qt6 (this project)
```

**See [docs/INTEGRATION.md](docs/INTEGRATION.md) for detailed architecture, deployment scenarios, and how skills interact with the GUI.**

---

## 📚 Documentation

**All documentation is in the [docs/](docs/) folder:**

- **[docs/index.md](docs/index.md)** — 📍 **Documentation hub** (navigation for all doc types)
- **[docs/QUICK_START.md](docs/QUICK_START.md)** — ⚡ 5-minute quickstart for first-time users
- **[docs/INTEGRATION.md](docs/INTEGRATION.md)** — 🏗️ Architecture, deployment, and ecosystem overview
- **[docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)** — 🔧 System design and component interactions
- **[docs/CODE_GUIDE.md](docs/CODE_GUIDE.md)** — 📖 Qt concepts for non-Qt developers
- **[docs/COMPONENTS.md](docs/COMPONENTS.md)** — 📋 Complete API reference
- **[docs/getting-started/BUILD.md](docs/getting-started/BUILD.md)** — 🏗️ Build from source
- **[docs/getting-started/INSTALL.md](docs/getting-started/INSTALL.md)** — 📦 Installation for all distributions
- **[docs/PORTING_GUIDE.md](docs/PORTING_GUIDE.md)** — Qt5→Qt6 migration guide
- **[docs/development/CONTRIBUTING.md](docs/development/CONTRIBUTING.md)** — 🤝 Contributing guidelines
- **[docs/development/TESTING.md](docs/development/TESTING.md)** — ✅ Testing and test writing
- **[docs/development/DEBUGGING.md](docs/development/DEBUGGING.md)** — 🐛 Debugging techniques
- **[docs/planning/](docs/planning/)** — 📊 Project planning and status documents

---

## More Information

- [mycroft-gui-qt5](../mycroft-gui-qt5/) — Qt5 original (unmaintained)
- [GUI Protocol Spec](https://github.com/OpenVoiceOS/ovos-gui/blob/dev/protocol.md) — Official protocol documentation
