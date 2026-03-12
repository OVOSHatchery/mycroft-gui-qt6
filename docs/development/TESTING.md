# Testing Guide: mycroft-gui-qt6

**How to test mycroft-gui-qt6 changes and contributions**

---

## Test Suite Overview

The project includes **5 automated test executables** covering:
- Message routing (protocol compliance)
- QML component loading
- Server functionality
- Model data handling
- Stress testing

All tests are built with CMake and run via `ctest`.

---

## Running Tests

### Quick Test (30 seconds)

```bash
cd build
make test

# Expected output:
# Test project /path/to/build
#     Start  1: servertest
# 1/5 Test #1: servertest ........................   PASSED    0.05 sec
#     Start  2: modeltest
# 2/5 Test #2: modeltest ........................   PASSED    0.03 sec
# ...
# 100% tests passed, 0 failures out of 5
```

### Verbose Test (with output)

```bash
make test VERBOSE=1

# Or
ctest --output-on-failure
```

### Run Individual Tests

```bash
# Test WebSocket protocol compliance
./autotests/message_routing_test

# Test QML can load all components
./autotests/qml_framework_components_test

# Test server functionality
./autotests/servertest

# Test model data handling
./autotests/modeltest

# Stress test under load
./autotests/stresstest
```

---

## Test Descriptions

### 1. message_routing_test
**Purpose**: Verify WebSocket protocol compliance

**Tests:**
- ✅ All 23 OVOS message types recognized
- ✅ Unknown messages handled gracefully
- ✅ Message categorization correct
- ✅ Protocol enum conversion working

**Code location**: `autotests/message_routing_test.cpp`
**Time**: ~0.02s

**When to run:**
- After changes to `GuiBusMessages::fromString()`
- After modifying message parsing logic
- When updating protocol

### 2. qml_framework_components_test
**Purpose**: Verify all QML components can be loaded

**Tests:**
- ✅ All 17 framework components loadable
- ✅ QML_ELEMENT registrations working
- ✅ Qt version validation passes
- ✅ No deprecated warnings

**Code location**: `autotests/qml_framework_components_test.cpp`
**Time**: ~0.04s

**When to run:**
- After modifying QML registration
- After updating component imports
- When adding new QML components

### 3. servertest
**Purpose**: Test core server functionality

**Tests:**
- ✅ Server can start and stop
- ✅ Connections accepted
- ✅ Messages routed correctly
- ✅ Session data managed properly

**Code location**: `autotests/servertest.cpp`
**Time**: ~0.05s

**When to run:**
- After changes to MycroftController
- After modifying WebSocket handling
- When refactoring message routing

### 4. modeltest
**Purpose**: Test model data handling

**Tests:**
- ✅ ActiveSkillsModel updates correctly
- ✅ SessionDataModel handles data
- ✅ Model signals emit properly
- ✅ Data bindings work

**Code location**: `autotests/modeltest.cpp`
**Time**: ~0.03s

**When to run:**
- After modifying model classes
- After updating data structures
- When changing property bindings

### 5. stresstest
**Purpose**: Test under load and stress conditions

**Tests:**
- ✅ Many connections handled
- ✅ High message throughput
- ✅ Memory stable under load
- ✅ No deadlocks or hangs

**Code location**: `autotests/stresstest.cpp`
**Time**: ~0.12s

**When to run:**
- Before release
- After performance-critical changes
- When debugging mysterious crashes

---

## Build with Testing

### Enable Tests

```bash
# Configure build with testing enabled
cmake .. -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Release

# Or Debug for better error messages
cmake .. -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug
```

### Build Tests

```bash
make -j$(nproc)

# Or just build tests without building main library
make servertest modeltest stresstest message_routing_test qml_framework_components_test
```

---

## Test-Driven Development

### When Adding a Feature

1. **Write the test first**
   ```cpp
   // autotests/myfeature_test.cpp
   void MyFeatureTest::testNewFeature() {
       QVERIFY(controller.newFeature() == expectedValue);
   }
   ```

2. **Run the test (it fails)**
   ```bash
   ./autotests/myfeature_test
   # FAIL: Feature not implemented
   ```

3. **Implement the feature**
   ```cpp
   // import/mycroftcontroller.cpp
   bool MycroftController::newFeature() {
       return true;
   }
   ```

4. **Run the test (it passes)**
   ```bash
   ./autotests/myfeature_test
   # PASS
   ```

5. **Run full test suite**
   ```bash
   cd build && make test
   # All 5+ tests pass
   ```

### When Fixing a Bug

1. **Create a test that reproduces the bug**
   ```cpp
   void BugFixTest::testBugReproduced() {
       // This test should fail with the bug present
       QVERIFY(controller.buggyBehavior() == false);
   }
   ```

2. **Run the test (it fails with the bug)**
   ```bash
   ./autotests/bugfix_test
   # FAIL: Shows the bug is present
   ```

3. **Fix the bug**
   ```cpp
   // Fix the implementation
   ```

4. **Run the test (it passes)**
   ```bash
   ./autotests/bugfix_test
   # PASS: Bug is fixed
   ```

5. **Keep the test**
   - Prevents regression
   - Documents the bug fix

---

## Memory Testing

### Detect Memory Leaks

**Build with AddressSanitizer:**
```bash
cd build
cmake .. -DCMAKE_CXX_FLAGS="-fsanitize=address" -DBUILD_TESTING=ON
make clean && make -j$(nproc)
make test VERBOSE=1
```

**Look for:**
```
SUMMARY: AddressSanitizer: 0 bytes leaked
✅ Pass: No memory leaks detected
❌ Fail: SUMMARY shows bytes leaked
```

### Detect Use-After-Free

The same AddressSanitizer build catches use-after-free bugs:
```
ERROR: AddressSanitizer: heap-use-after-free
```

### Detect Stack Overflows

Build with Stack Protector:
```bash
cmake .. -DCMAKE_CXX_FLAGS="-fstack-protector-all"
make test
```

---

## Performance Testing

### Measure Build Time

```bash
time make -j$(nproc)

# Expected: ~2-5 minutes depending on CPU cores
```

### Measure Runtime Performance

```bash
# Run stresstest to completion
time ./autotests/stresstest

# Check QML rendering FPS (if display available)
mycroft-gui-app --profile-qml
```

### Profile with perf

If `linux-tools` is installed:
```bash
perf record ./autotests/stresstest
perf report
```

---

## Continuous Integration (CI)

### Before Pushing

Always run this locally:
```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
make clean
make -j$(nproc)
make test VERBOSE=1

# All tests should pass
```

### GitHub Actions

Tests run automatically on:
- ✅ Every push to dev branch
- ✅ Every pull request
- ✅ Once per day (schedule)

Check CI status:
```bash
# Show latest CI runs
gh run list

# View specific run
gh run view <run-id>

# View logs
gh run view <run-id> --log
```

---

## Writing New Tests

### Test File Template

Create `autotests/myfeature_test.cpp`:

```cpp
#include <QtTest/QtTest>
#include <QObject>

// Include the class you're testing
#include "../import/mycroftcontroller.h"

class MyFeatureTest : public QObject {
    Q_OBJECT

private slots:
    // Each test_ method is a separate test

    void initTestCase() {
        // Run once before all tests
        // Set up shared resources
    }

    void cleanupTestCase() {
        // Run once after all tests
        // Clean up shared resources
    }

    void testFeatureBasic() {
        MycroftController controller;

        // Test basic functionality
        QVERIFY(controller.feature() == true);
    }

    void testFeatureWithData() {
        MycroftController controller;

        // Test with specific data
        QCOMPARE(controller.getValue(), 42);
    }

    void testFeatureFailsGracefully() {
        MycroftController controller;

        // Test error handling
        QVERIFY_EXCEPTION_THROWN(
            controller.badCall(),
            std::runtime_error
        );
    }
};

// Required to generate test executable
QTEST_MAIN(MyFeatureTest)
#include "myfeature_test.moc"
```

### Add to CMakeLists.txt

In `autotests/CMakeLists.txt`:

```cmake
ecm_add_test(
  myfeature_test.cpp
  ${import_SRCS}

  TEST_NAME myfeature_test

  LINK_LIBRARIES
    Qt6::Test
    Qt6::Qml
    Qt6::Quick
    Qt6::Network
    Qt6::WebSockets
)
```

### Common Test Assertions

```cpp
QVERIFY(condition)                    // Assert true
QVERIFY2(condition, message)          // With message
QCOMPARE(actual, expected)            // Equality
QCOMPARE(actual, expected, ...)       // Multiple assertions
QTEST_COMPARE(a, b)                   // Using == operator
QTEST_COMPARE_LT(a, b)               // Using < operator
QFAIL(message)                        // Force failure
QWARN(message)                        // Print warning
QSKIP(reason)                         // Skip this test
QVERIFY_EXCEPTION_THROWN(code, exception) // Exception testing
```

### Signal Testing

```cpp
void TestMySignals::testSignalEmitted() {
    MycroftController controller;

    // Verify signal is emitted
    QSignalSpy spy(&controller, &MycroftController::connectedChanged);

    controller.connect();

    // Check signal was emitted once
    QCOMPARE(spy.count(), 1);
}
```

---

## Debugging Test Failures

### Print Debug Information

```cpp
void MyFeatureTest::testDebug() {
    int value = controller.getValue();

    // Print for debugging
    qDebug() << "Value is:" << value;

    QCOMPARE(value, 42);
}
```

Run with output:
```bash
./autotests/myfeature_test -v2
```

### Use Qt Creator's Test Debugger

```bash
# If using Qt Creator
# 1. Open project
# 2. Run → Run Tests
# 3. Click on failed test
# 4. Debug
```

### Run Single Test

```bash
# Run only one test method
./autotests/myfeature_test MyFeatureTest::testSpecific

# Run with extra verbosity
./autotests/myfeature_test -v2
```

---

## Test Coverage

### Check Coverage

```bash
cd build
cmake .. -DCMAKE_CXX_FLAGS="--coverage" -DBUILD_TESTING=ON
make clean && make -j$(nproc)
make test

# Generate coverage report
lcov --directory . --capture --output-file coverage.info
lcov --remove coverage.info '*/test*' --output-file coverage.info
genhtml coverage.info --output-directory coverage_report

# View report
open coverage_report/index.html
```

### Coverage Goals

- ✅ Core classes (MycroftController, AbstractSkillView): 80%+
- ✅ Message routing: 90%+ (critical path)
- ⚠️ QML: Testing framework constraints limit coverage
- ✅ Error paths: 100% (catch all failures)

---

## Common Issues & Solutions

### Test Fails: "Cannot find Qt6"

**Solution:**
```bash
# Ensure Qt6 is installed
qmake6 -v

# Update CMake cache
cd build
rm CMakeCache.txt
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
make test
```

### Test Fails: "Symbol not found"

**Solution:**
```bash
# Rebuild everything
cd build
make clean
cmake ..
make -j$(nproc)
make test
```

### Test Timeout

**Solution:**
```bash
# Increase timeout (some systems are slow)
ctest --timeout 120 --output-on-failure
```

### Flaky Test (passes sometimes)

**Problem**: Test has timing-dependent behavior

**Solution**:
1. Add explicit waits:
   ```cpp
   QTest::qWait(100); // Wait 100ms
   ```

2. Use QSignalSpy instead of busy-waiting:
   ```cpp
   QSignalSpy spy(&obj, &MyObject::ready);
   spy.wait(5000); // Wait for signal
   ```

---

## Best Practices

✅ **Do:**
- Run tests before committing
- Write tests for new features
- Keep tests fast (< 1s each)
- Test both success and failure paths
- Use meaningful assertion messages

❌ **Don't:**
- Use `sleep()` in tests (use QTest::qWait or QSignalSpy)
- Create external files in tests (use temp directories)
- Rely on specific timing (add small delays)
- Skip tests without a good reason
- Test implementation details (test behavior instead)

---

## Next Steps

- **Read CODE_GUIDE.md**: Understand how to write Qt/C++ code
- **Check CONTRIBUTING.md**: Guidelines for pull requests
- **Look at COMPONENTS.md**: API reference for what to test

---

**Happy testing!** 🧪
