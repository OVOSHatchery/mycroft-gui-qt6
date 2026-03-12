# Qt6 Port Plan — mycroft-gui-qt6

**Status**: Framework complete, implementation pending
**Date**: 2026-03-12
**Source**: mycroft-gui-qt5 (Phase A3 modernized version)
**Target Completion**: 2026-04-02 (3 weeks estimated)

---

## Project Overview

Port mycroft-gui-qt5 (Qt5-based GUI client for OpenVoiceOS) to modern Qt6 while maintaining **100% feature parity**.

### Why This Matters

1. **Qt5 End-of-Life**: Qt5 is entering maintenance mode; Qt6 is the future
2. **Modern C++**: Qt6 enforces C++17 (better code, performance)
3. **Security**: Qt6 receives active security updates
4. **Performance**: Faster QML compilation, better rendering
5. **Type Safety**: Stricter QML type checking

---

## Current Status

✅ **Completed**:
- CMakeLists.txt (Qt6-ready, tested structure)
- Project documentation (README, PORTING_GUIDE)
- Build strategy
- Architecture plan
- **Phase 2: C++ Source Porting** (libmycroft-gui-qt6.so builds, 6.1 MB)
  - All 13 C++ files ported with QML_ELEMENT macro
  - MycroftController, AbstractSkillView, AbstractDelegate updated
  - Plugin registration refactored for Qt6
  - 3 deprecation warnings identified for Phase 4 fixing

⏳ **In Progress**:
- Phase 3: QML file conversion (25 system templates + 17 components)
- Phase 4: Test suite adaptation + warning fixes
- Phase 5: Integration testing and app completion

---

## Implementation Plan

### Phase 1: Build Infrastructure (DONE)

**Deliverables**:
- [x] CMakeLists.txt (Qt6 6.5+ support)
- [x] README.md (overview, quick start)
- [x] docs/PORTING_GUIDE.md (detailed API changes)

**Files**: 3 new, 0 modified

---

### Phase 2: C++ Source Porting (✅ DONE - 2026-03-12)

**Files to Port** (13 source files):

1. **Core Headers** (3 files):
   - `import/mycroftcontroller.h` — Protocol handler, message routing
   - `import/abstractskillview.h` — Skill view management
   - `import/guibusmessages.h` — Message enum (23 types) - MINIMAL CHANGES

2. **Implementation** (10 files):
   - `import/mycroftcontroller.cpp` — WebSocket, state changes
   - `import/abstractskillview.cpp` — Session data, lifecycle
   - `import/activeskillsmodel.cpp` — Active skills tracking
   - `import/delegatesmodel.cpp` — Model management
   - `import/sessiondatamodel.cpp` — Data model
   - `import/sessiondatamap.cpp` — Data map
   - `import/abstractdelegate.cpp` — Base delegate
   - `import/filereader.cpp` — File utilities
   - `import/globalsettings.cpp` — Configuration
   - `application/main.cpp` — App entry point

**Changes Required**:

| File | Changes | Effort |
|------|---------|--------|
| Headers | Add QML_ELEMENT macro, update includes | Low |
| CPP files | Replace qmlRegisterType calls, update APIs | Medium |
| main.cpp | Update QML engine setup | Low |
| guibusmessages.h | No changes needed (enum is Qt-version agnostic) | None |

**Approach**:
1. Copy all .cpp/.h from Qt5 version
2. Update CMake registration (qmlRegisterType → QML_ELEMENT)
3. Replace deprecated APIs
4. Update signal/slot syntax (stricter type checking)
5. Test compilation

**Acceptance Criteria**:
- [x] All source files compile without errors
- [⚠️] 3 deprecation warnings identified (QWebSocket::error → errorOccurred, QFile::open nodiscard)
- [x] Object lifecycle correct (verified MOC output)
- [x] Library builds successfully as shared object (libmycroft-gui-qt6.so, 6.1 MB)

**Results**:
```
libmycroft-gui-qt6.so: ELF 64-bit LSB shared object, x86-64, dynamically linked
Compiled with: C++17, Qt6.5+, KDE Frameworks 6.23
Warnings: 3 deprecation (resolvable in Phase 4)
Build time: ~2 minutes
```

---

### Phase 3: QML Porting (3-4 hours)

**System Templates** (25 files):

Update imports from:
```qml
import QtQuick 2.12
import QtQuick.Controls 2.12
import org.kde.kirigami 2.14 as Kirigami
```

To:
```qml
import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami
```

**Framework Components** (17 files):

Same import updates + add property types where beneficial.

**Changes**:
- Import statement simplification (3 lines per file)
- Add optional property types (non-breaking)
- Update Kirigami references (2.14 → 6.x context)

**Acceptance Criteria**:
- [ ] All QML files parse without errors
- [ ] Templates render correctly
- [ ] No type warnings in QML engine
- [ ] Data binding works end-to-end

---

### Phase 4: Test Suite (2-3 hours)

**Port Existing Tests**:

1. `autotests/message_routing_test.cpp` (50+ assertions)
   - Enum conversion tests
   - Unknown message handling
   - Message categorization

2. `autotests/qml_framework_components_test.cpp`
   - Component loading tests
   - Qt version validation
   - Deprecation notice checks

**New Tests** (Qt6-specific):

1. Property binding tests
2. QML type system validation
3. WebSocket protocol compliance

**Acceptance Criteria**:
- [ ] All message routing tests pass (50+ assertions)
- [ ] All QML component tests pass (11+ tests)
- [ ] New Qt6-specific tests added
- [ ] 100% coverage of message types

---

### Phase 5: Integration Testing (2-3 hours)

**Manual Testing**:

1. **Build & Install**:
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j$(nproc)
   make install
   ```

2. **Run**:
   ```bash
   mycroft-gui-app
   ```

3. **Test Scenarios**:
   - [ ] Connect to OVOS instance (localhost:18181)
   - [ ] Display text template
   - [ ] Display weather template
   - [ ] Display list template
   - [ ] Trigger button click events
   - [ ] Verify session data updates
   - [ ] Test multiple skill activation
   - [ ] Test skill deactivation
   - [ ] Verify cleanup on disconnect

**Performance Baseline**:
- Startup time (target: <2 seconds)
- Memory usage (baseline: compare with Qt5)
- QML rendering FPS (target: 60 FPS)

---

### Phase 6: Documentation (1-2 hours)

**Documentation Updates**:

1. [x] README.md — Overview, quick start (DONE)
2. [x] PORTING_GUIDE.md — API changes (DONE)
3. [ ] QUICK_FACTS.md — Metadata, dependencies
4. [ ] FAQ.md — Build, deploy, troubleshoot
5. [ ] AUDIT.md — Known issues, status
6. [ ] MIGRATION.md — Qt5→Qt6 user guide

**Documentation Quality**:
- All code examples compile-tested
- All file paths accurate
- All commands tested on EndeavourOS

---

## Technical Details

### Message Routing (UNCHANGED)

The enum-based message routing from Phase A3 is **Qt-agnostic** and requires no changes:

```cpp
// Works identically in Qt5 and Qt6
auto msgType = GuiBusMessages::fromString(typeStr);
if (msgType == GUIBusMessageType::SESSION_SET) { ... }
```

**No porting needed** for `guibusmessages.h` or message handlers using the enum.

### Protocol (UNCHANGED)

WebSocket protocol remains the same:
- Connection: ws://localhost:18181
- Message format: JSON with `type`, `namespace`, `data` fields
- All 23 message types supported
- Session data model unchanged

### Template System (MOSTLY UNCHANGED)

25 system templates need import updates only:
- No logic changes
- No property binding changes (mostly compatible)
- Layout and styling unchanged

---

## Risk Assessment

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|-----------|
| Qt6 API incompatibilities | Low | Medium | Comprehensive porting guide, test suite |
| Performance regression | Low | Medium | Performance baseline testing |
| Memory leaks in Qt6 | Low | High | valgrind/asan, test suite |
| Breaking changes in KF6 | Low | Medium | Use KF6 6.0 LTS, not 6.5+ |
| QML syntax incompatibilities | Very Low | Low | Qt6 QML is backward compatible |

---

## Success Criteria

✅ **All of these must be met**:

1. **Compilation**
   - No compiler errors
   - No warnings with -Wall -Wextra -Werror=format
   - Clean compilation on EndeavourOS

2. **Functionality**
   - All 25 templates render correctly
   - All 23 message types handled
   - WebSocket protocol works
   - Session data synchronization correct
   - User interactions (button clicks, events) work

3. **Quality**
   - 60+ test assertions pass
   - No memory leaks (valgrind clean)
   - Performance baseline met (startup <2s)
   - Code follows Qt6 best practices

4. **Documentation**
   - README complete with build/run instructions
   - PORTING_GUIDE documents all changes
   - QUICK_FACTS with dependencies
   - AUDIT.md with known issues

5. **Compatibility**
   - 100% feature parity with Qt5 version
   - Protocol unchanged
   - Template behavior unchanged
   - API backward compatible where possible

---

## File Structure

```
mycroft-gui-qt6/
├── CMakeLists.txt              # Qt6 build config ✅
├── README.md                   # Overview ✅
├── QT6_PORT_PLAN.md           # This file
├── QUICK_FACTS.md             # (To create)
├── FAQ.md                     # (To create)
├── AUDIT.md                   # (To create)
├── LICENSE
├── docs/
│   ├── PORTING_GUIDE.md       # API changes ✅
│   ├── PROTOCOL.md            # (Copy from Qt5)
│   ├── TEMPLATE_VALIDATION.md # (Copy from Qt5)
│   └── QML_AUDIT.md          # (To create)
├── import/
│   ├── CMakeLists.txt
│   ├── mycroft.qrc
│   ├── *.h                    # (To port)
│   ├── *.cpp                  # (To port)
│   ├── system-templates/      # (To port QML)
│   │   ├── *.qml             # (25 templates)
│   └── qml/                   # (To port)
│       ├── *.qml             # (17 components)
│       └── private/
│           └── *.qml
├── application/
│   ├── CMakeLists.txt
│   └── main.cpp              # (To port)
├── autotests/
│   ├── CMakeLists.txt
│   ├── message_routing_test.cpp        # (To port)
│   ├── qml_framework_components_test.cpp # (To port)
│   └── ...
└── .git/
    └── (13 commits so far)
```

---

## Timeline

| Week | Phase | Hours | Status |
|------|-------|-------|--------|
| W1 (3/12-3/18) | Build + C++ | 6-8 | ✅ Build done, C++ starting |
| W2 (3/19-3/25) | QML + Tests | 5-7 | ⏳ Pending |
| W3 (3/26-4/1) | Integration + Docs | 3-5 | ⏳ Pending |

**Total**: 14-20 hours development, 3 weeks calendar

---

## Dependencies

### Required
- Qt 6.5+ (LTS)
- KDE Frameworks 6.0+
- CMake 3.24+
- C++17 compiler (GCC 7+, Clang 5+)

### Optional
- KDE Plasma (for extra components)
- KDE KIO (for file dialogs)

### Build Tools
- cmake
- make or ninja
- qmake (for version info)
- qmllint (for QML validation)

---

## Next Steps

1. **Immediate** (Today):
   - [ ] Port C++ headers (update macros, includes)
   - [ ] Port C++ implementation (update APIs)
   - [ ] Get code compiling

2. **This Week**:
   - [ ] Port all QML files (import statements)
   - [ ] Get QML engine running
   - [ ] Adapt test suite

3. **Next Week**:
   - [ ] Integration testing (end-to-end)
   - [ ] Performance benchmarking
   - [ ] Complete documentation

---

## Commits Planned

```
[Phase 2] refactor: port C++ sources to Qt6 APIs
           - Update qmlRegisterType → QML_ELEMENT
           - Replace deprecated includes/functions
           - Update signal/slot syntax
           
[Phase 3] refactor: update QML for Qt6 imports
           - Simplify import statements
           - Add property types (optional)
           - Update Kirigami 2.14 → 6.x references
           
[Phase 4] test: port and update test suite for Qt6
           - Update test framework imports
           - Add Qt6-specific tests
           - Update assertions
           
[Phase 5] docs: add Qt6 documentation
           - Create QUICK_FACTS.md
           - Create FAQ.md
           - Update AUDIT.md
           - Create MIGRATION.md for users
```

---

## Success Metrics

**Upon Completion**:
- ✅ All C++ compiles (0 warnings)
- ✅ All QML valid (qmllint clean)
- ✅ 60+ tests pass
- ✅ No memory leaks (valgrind clean)
- ✅ 100% feature parity with Qt5
- ✅ Comprehensive documentation
- ✅ Ready for production use

---

**Author**: Claude Code (AI Assistant)
**Version**: 1.0 (2026-03-12)
**Status**: Active Development
