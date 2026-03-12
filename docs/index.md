# mycroft-gui-qt6 Documentation

**Complete documentation for the modern Qt6 GUI client for OpenVoiceOS**

---

## 🚀 Getting Started

Start here if you're new to mycroft-gui-qt6:

### For Users

1. **[QUICK_START.md](QUICK_START.md)** — 5-minute setup guide
   - System requirements
   - Install for your distribution
   - Verify it works
   - Connect to OpenVoiceOS

2. **[getting-started/INSTALL.md](getting-started/INSTALL.md)** — Complete installation guide
   - Distribution-specific instructions (Ubuntu, Fedora, Arch, Alpine)
   - Binary packages (PPA, Copr, AUR)
   - Building from source
   - Custom installation locations
   - Verification and troubleshooting

3. **[getting-started/BUILD.md](getting-started/BUILD.md)** — Build from source
   - All dependencies for each distribution
   - CMake configuration options
   - Building with Make or Ninja
   - Testing your build
   - Performance optimization tips

4. **[INTEGRATION.md](INTEGRATION.md)** — Architecture and ecosystem overview
   - How mycroft-gui-qt6 fits in OpenVoiceOS
   - Links to related projects (ovos-gui, ovos-gui-api-client, ovos-legacy-mycroft-gui-plugin)
   - Deployment scenarios (single machine, remote, containers)
   - Troubleshooting integration issues

---

## 📖 Understanding the System

Read these to understand how mycroft-gui-qt6 works:

### Architecture & Design

1. **[ARCHITECTURE.md](ARCHITECTURE.md)** — System design and component interactions
   - Core components: `MycroftController`, `AbstractSkillView`, `AbstractDelegate`
   - WebSocket protocol on port 18181
   - Signal/slot system and property binding
   - Template rendering pipeline
   - Session data flow
   - Threading model (single-threaded event loop)
   - Error handling strategy
   - Complete ASCII architecture diagrams

2. **[INTEGRATION.md](INTEGRATION.md)** — GUI ecosystem architecture
   - Three-layer architecture: Skills → ovos-gui → GUI client
   - How the legacy adapter plugin bridges protocols
   - Deployment patterns
   - Event routing for voice and touch
   - Comparison with other GUI clients

3. **[PROTOCOL.md](PROTOCOL.md)** — WebSocket message protocol
   - Message format and types
   - Session management
   - Data transmission

### For Non-Qt Developers

**[CODE_GUIDE.md](CODE_GUIDE.md)** — Qt and C++ concepts explained
- Qt fundamentals without assuming Qt knowledge
- Signals and slots: how components communicate
- Property system: declarative UI binding
- Parent-child ownership: memory management
- QML syntax and features
- Common patterns: Singleton, Model-View
- Real code examples from mycroft-gui-qt6
- Debugging tips and tools
- How to read and modify the codebase
- Common mistakes and how to avoid them

### API Reference

**[COMPONENTS.md](COMPONENTS.md)** — Complete API documentation
- All C++ classes with method signatures
- All 25+ system templates (SYSTEM_text, SYSTEM_image, SYSTEM_weather, etc.)
- QML components (AudioPlayer, VideoPlayer, SlideShow, etc.)
- Property documentation with types
- Data flow for each component
- Usage examples
- Index of all template types

---

## 🛠️ For Developers & Contributors

### Extending mycroft-gui-qt6

**[CODE_GUIDE.md](CODE_GUIDE.md)** — Code concepts for developers
- How to read the C++ codebase
- QML architecture and structure
- Building custom extensions
- Memory management patterns
- Testing strategies
- Debugging Qt applications
- Common pitfalls

**[COMPONENTS.md](COMPONENTS.md)** — Component reference
- All exportable C++ classes with `QML_ELEMENT` macro
- File locations and responsibilities
- How to add new components
- How to add new templates

### Project History & Planning

**[planning/](planning/)** — Project planning and status documents
- **[QT6_PORT_PLAN.md](planning/QT6_PORT_PLAN.md)** — Original porting plan and strategy
  - Phase 1-6 breakdown (Build, C++, QML, Tests, Integration, Docs)
  - Risk assessment
  - Success criteria
  - File structure and dependencies

- **[MODERNIZATION_PLAN.md](planning/MODERNIZATION_PLAN.md)** — Modernization roadmap
  - Code quality improvements
  - Memory leak fixes
  - Dead code removal
  - Feature completion
  - QML refactoring

- **[planning/COMPLETION_REPORT.md](planning/COMPLETION_REPORT.md)** — Final project status
  - All phases completed
  - Metrics: lines of code, documentation, tests
  - Build results and artifact sizes
  - Quality metrics
  - Deployment readiness assessment

- **[planning/PROJECT_AUDIT.md](planning/PROJECT_AUDIT.md)** — Known issues and technical debt
  - Current status of all work items
  - Technical debt tracking
  - Issues and limitations

### Porting from Qt5

**[PORTING_GUIDE.md](PORTING_GUIDE.md)** — Qt5→Qt6 migration guide
- API changes (signals, properties, QML imports)
- Deprecated patterns and replacements
- Build system changes (CMake)
- QML syntax updates
- Common migration issues
- Testing after migration

---

## 📚 Key Files & Classes Reference

### C++ Core

| Class | File | Purpose |
|-------|------|---------|
| `MycroftController` | `import/mycroftcontroller.cpp:29` | WebSocket connection manager, message routing singleton |
| `AbstractSkillView` | `import/abstractskillview.cpp:76` | Per-skill UI container, session data management |
| `AbstractDelegate` | `import/abstractdelegate.cpp:42` | Base delegate class for custom skill UI |
| `ActiveSkillsModel` | `import/activeskillsmodel.cpp:15` | Model tracking active skills |
| `SessionDataModel` | `import/sessiondatamodel.cpp:29` | Session data dictionary model |
| `GuiBusMessages` | `import/guibusmessages.h:15` | 23 OVOS message types enum |

### QML Components & Templates

**System Templates** (in `import/system-templates/`):
- `Text.qml` — Long-form text display
- `Image.qml` — Static image rendering
- `AnimatedImage.qml` — GIF/WebP playback
- `List.qml` — Scrollable lists
- `Grid.qml` — 2D grid layout
- `Weather.qml` — Weather summary card
- `AudioPlayer.qml` — Now-playing display
- `VideoPlayer.qml` — Video playback surface
- And 17 more (Clock, Timer, Map, Table, etc.)

**Framework Components** (in `import/qml/`):
- `SkillView.qml` — Skill container
- `Delegate.qml` — Base delegate
- `AudioPlayer.qml` — Audio playback UI
- `SlideShow.qml` — Image slideshow
- `StatusIndicator.qml` — Status display
- And 12 more

### Build Configuration

| File | Purpose |
|------|---------|
| `CMakeLists.txt:1-10` | Qt6 6.5+ configuration, C++17 requirement |
| `CMakeLists.txt:13-22` | Component discovery (Qt, KDE Frameworks) |
| `import/qmldir` | QML module registration |
| `autotests/CMakeLists.txt` | Test targets (5 executables) |

### Tests

| Test | File | Purpose |
|------|------|---------|
| `servertest` | `autotests/servertest.cpp` | Core server functionality |
| `modeltest` | `autotests/modeltest.cpp` | Model data handling |
| `stresstest` | `autotests/stresstest.cpp` | Load and stress testing |
| `message_routing_test` | `autotests/message_routing_test.cpp` | Protocol compliance |
| `qml_framework_components_test` | `autotests/qml_framework_components_test.cpp` | QML component loading |

---

## 🔗 Related Projects

### Core GUI Infrastructure
- **[ovos-gui](https://github.com/OpenVoiceOS/ovos-gui)** — Central GUI messagebus service
  - Manages GUI state
  - Routes messages between skills and clients
  - Configuration: `gui_websocket.base_port` (default 18181)

- **[ovos-gui-api-client](https://github.com/OpenVoiceOS/ovos-gui-api-client)** — Python library for skills
  - Skills use this to send templates
  - Implements all 25+ template types
  - Documentation: Template philosophy and design

- **[ovos-legacy-mycroft-gui-plugin](https://github.com/OpenVoiceOS/ovos-legacy-mycroft-gui-plugin)** — Adapter bridge
  - Bridges old protocol (port 18181) to new ovos-gui
  - **This is what mycroft-gui-qt6 uses to connect**
  - Entry point: `opm.gui_adapter`

### Other GUI Clients
- **[pyhtmx-gui-client](https://github.com/OpenVoiceOS/pyhtmx-gui-client)** — Browser-based GUI (FastAPI + HTMX)
- **[ovos-shell](https://github.com/OpenVoiceOS/ovos-shell)** — Full desktop shell with homescreen

### Specifications
- **[GUI Protocol Spec](https://github.com/OpenVoiceOS/ovos-gui/blob/dev/protocol.md)** — Official protocol documentation
- **[Qt6 Compatibility Assessment](https://github.com/OpenVoiceOS/ovos-gui/blob/dev/ADAPTER_COMPATIBILITY_ASSESSMENT.md)** — Qt6 adapter compatibility (in ovos-gui)

---

## 📋 Documentation Index by Audience

### Users Installing & Using mycroft-gui-qt6
1. **[QUICK_START.md](QUICK_START.md)** ← Start here
2. **[getting-started/INSTALL.md](getting-started/INSTALL.md)** — Your distribution
3. **[INTEGRATION.md](INTEGRATION.md)** — How it works in OpenVoiceOS
4. **[getting-started/BUILD.md](getting-started/BUILD.md)** — Only if building from source

### Developers Extending mycroft-gui-qt6
1. **[ARCHITECTURE.md](ARCHITECTURE.md)** — Understand the system
2. **[CODE_GUIDE.md](CODE_GUIDE.md)** — Learn Qt without prior Qt knowledge
3. **[COMPONENTS.md](COMPONENTS.md)** — API reference for all classes
4. **[PORTING_GUIDE.md](PORTING_GUIDE.md)** — If porting custom Qt5 code

### Project Managers & Decision Makers
1. **[planning/COMPLETION_REPORT.md](planning/COMPLETION_REPORT.md)** — Project status and metrics
2. **[planning/QT6_PORT_PLAN.md](planning/QT6_PORT_PLAN.md)** — Original plan and scope
3. **[INTEGRATION.md](INTEGRATION.md)** — Ecosystem context
4. **[planning/PROJECT_AUDIT.md](planning/PROJECT_AUDIT.md)** — Known issues

### Maintainers & Contributors
1. **[CODE_GUIDE.md](CODE_GUIDE.md)** — How the code is organized
2. **[COMPONENTS.md](COMPONENTS.md)** — What each class does
3. **[ARCHITECTURE.md](ARCHITECTURE.md)** — System design
4. **[PORTING_GUIDE.md](PORTING_GUIDE.md)** — Qt5→Qt6 patterns
5. **[planning/](planning/)** — Historical context and decisions

---

## 🎯 Quick Reference

### Most Important Files

**For Users:**
- `docs/index.md` (you are here)
- `../README.md` (root project overview)
- `QUICK_START.md` (get running fast)
- `INTEGRATION.md` (understand the ecosystem)

**For Developers:**
- `ARCHITECTURE.md` (system design)
- `CODE_GUIDE.md` (learn the codebase)
- `COMPONENTS.md` (API reference)

**For Contributors:**
- `COMPONENTS.md` (what classes do)
- `CODE_GUIDE.md` (how to read code)
- `PORTING_GUIDE.md` (Qt patterns)

### Key Configuration Files (in project root)

- `CMakeLists.txt` — Build configuration
- `import/qmldir` — QML module registration
- `import/mycroft.qrc` — Resource file (templates, QML, assets)

### Test Execution

```bash
# Build with tests
cd build
cmake .. -DBUILD_TESTING=ON
make -j$(nproc)

# Run all tests
make test VERBOSE=1

# Run individual test
./autotests/message_routing_test
```

---

## 📞 Getting Help

### If You Can't Find What You Need

1. **Search this documentation** — Use browser Find (Ctrl+F) to search all docs
2. **Check COMPONENTS.md** — Class and method reference
3. **Check CODE_GUIDE.md** — Qt concepts explained
4. **Check ARCHITECTURE.md** — System design and data flow
5. **Check planning/PROJECT_AUDIT.md** — Known issues

### Common Questions

**Q: How do I install this?**
→ See [getting-started/INSTALL.md](getting-started/INSTALL.md)

**Q: How does mycroft-gui-qt6 connect to OpenVoiceOS?**
→ See [INTEGRATION.md](INTEGRATION.md) - Ecosystem Architecture section

**Q: How do I build from source?**
→ See [getting-started/BUILD.md](getting-started/BUILD.md)

**Q: What's a QML_ELEMENT macro?**
→ See [CODE_GUIDE.md](CODE_GUIDE.md) - Qt Basics section

**Q: How do I add a custom template?**
→ See [COMPONENTS.md](COMPONENTS.md) - System Templates section

**Q: What happened to mycroft-gui-qt5?**
→ See [planning/MODERNIZATION_PLAN.md](planning/MODERNIZATION_PLAN.md) and root [README.md](../README.md)

---

## 📊 Documentation Statistics

| Category | Document | Lines | Focus |
|----------|----------|-------|-------|
| **Getting Started** | QUICK_START.md | ~150 | First-time users |
| | getting-started/INSTALL.md | 600+ | Installation for all distros |
| | getting-started/BUILD.md | 500+ | Building from source |
| **Architecture** | ARCHITECTURE.md | 500+ | System design |
| | CODE_GUIDE.md | 400+ | Qt concepts |
| | COMPONENTS.md | 350+ | API reference |
| | INTEGRATION.md | 645 | Ecosystem |
| **Planning** | planning/QT6_PORT_PLAN.md | 440 | Original plan |
| | planning/MODERNIZATION_PLAN.md | 150 | Improvements |
| | planning/COMPLETION_REPORT.md | 200+ | Status |
| | planning/PROJECT_AUDIT.md | 100+ | Known issues |
| **Migration** | PORTING_GUIDE.md | 300+ | Qt5→Qt6 |
| | PROTOCOL.md | 180 | Protocol spec |
| **Total** | (10 documents) | 4,000+ | Comprehensive |

---

## 🔄 Document Relationships

```
README.md (root)
    ├─ QUICK_START.md (first read)
    │
    ├─ INTEGRATION.md (ecosystem overview)
    │   ├─ Links to ovos-gui
    │   ├─ Links to ovos-gui-api-client
    │   └─ Links to ovos-legacy-mycroft-gui-plugin
    │
    ├─ getting-started/INSTALL.md (distribution-specific)
    │   └─ getting-started/BUILD.md (from source)
    │
    ├─ ARCHITECTURE.md (system design)
    │   ├─ CODE_GUIDE.md (learn the code)
    │   ├─ COMPONENTS.md (API reference)
    │   └─ PORTING_GUIDE.md (Qt5→Qt6)
    │
    └─ planning/ (project context)
        ├─ QT6_PORT_PLAN.md (original scope)
        ├─ MODERNIZATION_PLAN.md (improvements)
        ├─ COMPLETION_REPORT.md (final status)
        └─ PROJECT_AUDIT.md (known issues)
```

---

## 🎓 Learning Path

### For Users (1-2 hours)
1. Read [QUICK_START.md](QUICK_START.md) (10 min)
2. Install from [getting-started/INSTALL.md](getting-started/INSTALL.md) (30 min)
3. Read [INTEGRATION.md](INTEGRATION.md) - "Deployment Scenarios" section (20 min)
4. Verify it works with OpenVoiceOS (30 min)

### For Developers (4-6 hours)
1. Read [ARCHITECTURE.md](ARCHITECTURE.md) (60 min)
2. Skim [CODE_GUIDE.md](CODE_GUIDE.md) sections relevant to your task (30 min)
3. Study [COMPONENTS.md](COMPONENTS.md) for the classes you'll modify (60 min)
4. Read [CODE_GUIDE.md](CODE_GUIDE.md) - "Code Reading Strategy" to navigate the source (30 min)
5. Build and run tests from [getting-started/BUILD.md](getting-started/BUILD.md) (30 min)

### For Contributors (2-3 hours)
1. Read [CODE_GUIDE.md](CODE_GUIDE.md) entire (90 min)
2. Explore source code using patterns from "Code Reading Strategy" (60 min)
3. Review [COMPONENTS.md](COMPONENTS.md) for your feature area (30 min)
4. Check [PORTING_GUIDE.md](PORTING_GUIDE.md) for Qt patterns (30 min)

---

**Last Updated**: 2026-03-12
**Project Status**: Complete, ready for deployment
**Maintained By**: Claude AI (best-effort, see [README.md](../README.md) for details)
