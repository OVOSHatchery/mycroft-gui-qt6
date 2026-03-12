# Contributing to mycroft-gui-qt6

**Guidelines for contributing code, documentation, and improvements**

---

## Welcome! 👋

mycroft-gui-qt6 is an AI-modernized project that benefits from community contributions. Whether it's code, documentation, bug reports, or enhancements, your help is valued.

---

## Before You Start

1. **Read the project status** in [../README.md](../../README.md)
   - This is AI-generated experimental code
   - It's well-tested but not yet in production on all systems
   - Community review is essential

2. **Understand the architecture** in [../ARCHITECTURE.md](../ARCHITECTURE.md)
   - System components
   - Signal/slot communication
   - WebSocket protocol

3. **Review existing code** using [../CODE_GUIDE.md](../CODE_GUIDE.md)
   - How to read Qt/C++ code without Qt expertise
   - Project code organization
   - Common patterns

---

## Setting Up Your Development Environment

### Clone and Build

```bash
# Clone the repository
git clone https://github.com/OpenVoiceOS/mycroft-gui-qt6.git
cd mycroft-gui-qt6

# Install dependencies (see ../getting-started/BUILD.md for your distro)

# Create development build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
make -j$(nproc)
```

### Run Tests

```bash
cd build
make test VERBOSE=1

# Or run individual tests
./autotests/message_routing_test
./autotests/qml_framework_components_test
```

### Enable Debug Symbols

For better debugging, build in Debug mode:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
make clean
make -j$(nproc)
```

---

## Code Organization

```
mycroft-gui-qt6/
├── CMakeLists.txt              # Build configuration
├── import/
│   ├── *.h / *.cpp            # C++ sources (13 files)
│   │   ├── mycroftcontroller.cpp         # WebSocket connection
│   │   ├── abstractskillview.cpp         # Skill UI container
│   │   ├── abstractdelegate.cpp          # Base delegate
│   │   ├── guibusmessages.h              # Message type enum
│   │   └── ...
│   ├── system-templates/       # 25 OVOS templates (QML)
│   │   ├── Text.qml, Image.qml, Weather.qml, ...
│   │   └── ...
│   ├── qml/                    # 17 framework components
│   │   ├── SkillView.qml, AudioPlayer.qml, ...
│   │   └── ...
│   ├── qmldir                  # QML module registration
│   └── mycroft.qrc             # Resource file (templates)
├── autotests/
│   ├── CMakeLists.txt          # Test configuration
│   ├── message_routing_test.cpp    # Protocol tests
│   ├── qml_framework_components_test.cpp
│   └── ...
└── docs/
    ├── index.md                # Documentation hub
    ├── ARCHITECTURE.md         # System design
    └── ...
```

---

## How to Contribute

### 1. Bug Reports

Found a bug? Report it on GitHub with:
- Clear title: "Bug: [component] fails when [condition]"
- Reproduction steps
- Expected behavior
- Actual behavior
- System info (distro, Qt version, compiler)
- Log output (with `--log-level debug`)

Example:
```
Title: Bug: MycroftController fails to reconnect after network loss

Steps:
1. Start mycroft-gui-app
2. Disconnect from network
3. Reconnect to network

Expected: GUI reconnects automatically
Actual: GUI remains disconnected, no error message

System: Ubuntu 22.04, Qt 6.5, GCC 11
Log: [paste full log with --log-level debug]
```

### 2. Feature Requests

Want a new feature? Create a GitHub issue with:
- Clear title: "Feature: [description]"
- Problem it solves
- Proposed solution (with pseudocode if helpful)
- Alternatives considered
- Impact on existing functionality

Example:
```
Title: Feature: Persist window position and size

Problem: Window position resets every time the app starts

Proposed Solution:
- Store window geometry in ~/.config/mycroft/gui-geometry.conf
- Restore on startup using QSettings

Code outline:
void MycroftController::saveWindowGeometry() { /* ... */ }
void MycroftController::restoreWindowGeometry() { /* ... */ }
```

### 3. Code Contributions

#### For Small Changes (< 50 lines)

1. **Fork and create a branch**
   ```bash
   git checkout -b fix/issue-name
   # or
   git checkout -b feature/feature-name
   ```

2. **Make your changes**
   - Follow existing code style (see CODE_GUIDE.md)
   - Update comments and docstrings
   - Add tests if applicable

3. **Test your changes**
   ```bash
   cd build
   cmake .. && make -j$(nproc)
   make test VERBOSE=1
   ```

4. **Commit with clear message**
   ```bash
   git commit -m "fix: MycroftController reconnects after network loss

   - Track connection state with Timer
   - Auto-reconnect on disconnection signal
   - Add debug logging for diagnostics

   Fixes #123"
   ```

5. **Push and create Pull Request**
   ```bash
   git push origin fix/issue-name
   ```

#### For Large Changes (> 50 lines)

1. **Discuss first** — Open an issue describing your changes
2. **Get feedback** — Get maintainer buy-in before investing time
3. **Create detailed PR** — Reference the issue, explain design decisions
4. **Be prepared to iterate** — Code review may require refinements

### 4. Documentation

Documentation is as important as code!

**To improve existing docs:**
- Edit markdown files in `docs/`
- Update internal links if you move files
- Run spell checker or use a grammar tool
- Test links with `grep "^\[" docs/*.md`

**To add new docs:**
1. Place in appropriate `docs/` subdirectory
2. Link from `docs/index.md` navigation section
3. Include clear examples and code references
4. Cross-reference related documents

**Example documentation contribution:**
```bash
# Add a debugging guide
cat > docs/development/DEBUGGING.md << 'EOF'
# Debugging mycroft-gui-qt6

## Using Qt Creator
...
EOF

# Update docs/index.md to link to it
```

---

## Code Style & Standards

### C++ Code

**Follow Qt conventions:**
- Member variables: `m_variableName` (m_ prefix)
- Private methods: `doSomethingPrivate()`
- Public methods: `doSomething()`
- Signals: emit with `Q_EMIT` macro
- Slots: prefix with `on` or `handle`: `onConnected()`, `handleError()`

**Example (correct style):**
```cpp
// mycroftcontroller.h
class MycroftController : public QObject {
    Q_OBJECT
    QML_ELEMENT

public:
    explicit MycroftController(QObject *parent = nullptr);
    void connect();
    void disconnect();

    bool isConnected() const { return m_connected; }

signals:
    void connectedChanged();
    void errorOccurred(const QString &message);

private slots:
    void onWebSocketConnected();
    void onWebSocketDisconnected();

private:
    void emitPageShow(const QString &namespace_);

    QWebSocket m_socket;
    bool m_connected = false;
};
```

**Comments:**
- Explain *why*, not *what*
- Document non-obvious logic
- Use `//` for single-line comments
- Use `/* */` for multi-line comments

```cpp
// Bad: just repeating the code
m_socket.close(); // Close the socket

// Good: explaining the reason
// Close socket without waiting for graceful close
// (we're shutting down and need to exit quickly)
m_socket.close();
```

### QML Code

**Follow Qt QML conventions:**
- Use 4-space indentation
- Use camelCase for properties
- Anchor or Layout for positioning
- Use property bindings instead of imperative updates

**Example (correct style):**
```qml
// SkillView.qml
import QtQuick
import Mycroft

Item {
    id: root

    required property var skillView
    property bool isActive: false

    width: parent.width
    height: parent.height

    onIsActiveChanged: {
        // React to state change
        console.log("Skill active:", isActive)
    }

    ListView {
        anchors.fill: parent
        model: root.skillView.sessionData
        // ...
    }
}
```

### Commit Messages

Use this format:
```
<type>: <short description>

<optional detailed explanation>

Related to #issue-number
```

**Types:**
- `feat:` New feature
- `fix:` Bug fix
- `docs:` Documentation change
- `test:` Test addition/modification
- `refactor:` Code refactoring (no behavior change)
- `perf:` Performance improvement
- `build:` Build system change
- `ci:` CI/CD change

**Examples:**
```
fix: MycroftController reconnects after network failure

- Monitor connection state with QTimer
- Automatically reconnect when socket disconnects
- Log reconnection attempts for debugging

Fixes #456

---

docs: Update ARCHITECTURE.md with signal flow diagram

- Add ASCII diagram showing MycroftController signals
- Document session data flow for new contributors
- Link to related code sections

Related to #789
```

---

## Testing Your Changes

### Run the Test Suite

```bash
cd build
cmake .. -DBUILD_TESTING=ON
make -j$(nproc)
make test VERBOSE=1

# Expected output:
# 100% tests passed, 0 failures out of 5
```

### Add Tests for New Features

If adding a new feature, add tests to `autotests/`:

```cpp
// autotests/myfeature_test.cpp
#include <QtTest>
#include "../import/mycroftcontroller.h"

class MyFeatureTest : public QObject {
    Q_OBJECT

private slots:
    void testNewFeature() {
        MycroftController controller;
        // Test the feature
        QVERIFY(controller.newFeature());
    }
};

QTEST_MAIN(MyFeatureTest)
#include "myfeature_test.moc"
```

Then add to `autotests/CMakeLists.txt`:
```cmake
ecm_add_test(
    myfeature_test.cpp
    ...
    TEST_NAME myfeature_test
)
```

### Memory Testing

Check for memory leaks:
```bash
cd build
cmake .. -DCMAKE_CXX_FLAGS="-fsanitize=address"
make -j$(nproc)
./autotests/message_routing_test

# Look for "SUMMARY: AddressSanitizer: 0 bytes leaked"
```

---

## Pull Request Process

### Before Opening a PR

1. **Rebase on latest dev**
   ```bash
   git fetch origin
   git rebase origin/dev
   ```

2. **Test your changes**
   ```bash
   cd build && make clean && make -j$(nproc) && make test VERBOSE=1
   ```

3. **Review your own changes**
   ```bash
   git log --oneline -3
   git diff origin/dev...HEAD
   ```

### In the PR

1. **Link to related issues**
   ```
   Fixes #123
   Related to #456
   ```

2. **Describe what changed and why**
   - What problem does this solve?
   - How did you solve it?
   - Did you consider alternatives?

3. **Reference code locations**
   - Point to changed files with line numbers
   - Link to design decisions in issues

### During Code Review

- **Be open to feedback** — Reviewers want to help
- **Explain your reasoning** — If a suggestion doesn't make sense, explain why
- **Update and re-request review** — After making changes, click "Re-request review"

---

## Documentation for Your Code

When adding code, document it:

### C++ Classes and Methods

```cpp
/// MycroftController manages the WebSocket connection to ovos-gui
///
/// This singleton handles:
/// - Connecting to ovos-gui service on port 18181
/// - Routing incoming template messages to SkillViews
/// - Forwarding user events (touch, voice) back to skills
///
/// Usage:
/// \code
/// auto controller = MycroftController::instance();
/// connect(controller, &MycroftController::pageShowRequested,
///         this, &MySkillView::onPageShow);
/// \endcode
///
/// \sa AbstractSkillView, GuiBusMessages
class MycroftController : public QObject {
    // ...
};
```

### QML Components

```qml
/// SkillView renders a skill's UI templates
///
/// This component is responsible for:
/// - Loading the appropriate template for the current skill
/// - Binding session data to the template
/// - Handling user interactions (button clicks, etc.)
///
/// Example:
/// \qml
/// SkillView {
///     skillView: MycroftController.currentSkillView
///     isActive: true
/// }
/// \endqml
Item {
    id: root

    /// The AbstractSkillView model managing this skill's data
    required property var skillView

    /// Whether this skill view is currently active
    property bool isActive: false
}
```

---

## Reporting Security Issues

If you find a security vulnerability:

1. **Do NOT open a public GitHub issue**
2. **Email security@openvoceos.com** with:
   - Description of the vulnerability
   - Steps to reproduce
   - Potential impact
   - Suggested fix (if any)

---

## Questions?

- **Read the docs**: [docs/index.md](index.md)
- **Check CODE_GUIDE.md**: [../CODE_GUIDE.md](../CODE_GUIDE.md)
- **Open a discussion issue** on GitHub

---

## License

By contributing, you agree that your code is licensed under Apache 2.0, the same as the project.

---

**Thank you for contributing to mycroft-gui-qt6!** 🚀

Your improvements help the OpenVoiceOS community.
