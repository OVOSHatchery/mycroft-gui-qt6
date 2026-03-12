# Building mycroft-gui-qt6 from Source

## System Requirements

### Minimum
- **OS**: Linux (Ubuntu 20.04+, Fedora 33+, Arch, Debian 11+)
- **CPU**: x86_64 or ARM64
- **RAM**: 2 GB (4 GB recommended for faster builds)
- **Disk**: 1 GB free space

### Required Software
- **C++ Compiler**: GCC 9+ or Clang 10+
- **CMake**: 3.24 or later
- **Qt6**: 6.5.0 or later
- **KDE Frameworks 6**: 6.0 or later
- **Git**: For version control

### Optional
- **OpenVoiceOS Core**: For full testing
- **Ninja**: For faster builds (alternative to Make)

## Dependency Installation

### Ubuntu 22.04 LTS / Debian 12

```bash
sudo apt-get update
sudo apt-get install -y \
  build-essential \
  cmake \
  git \
  qt6-base-dev \
  qt6-qml-dev \
  qt6-quick-dev \
  qt6-network-dev \
  libqt6websockets6-dev \
  libqt6multimedia6 \
  libqt6gui6 \
  libkf6kirigami2-5 \
  libkf6kirigami2-dev \
  libkf6coreaddons6 \
  libkf6coreaddons-dev \
  extra-cmake-modules
```

### Fedora 38+

```bash
sudo dnf install -y \
  gcc-c++ \
  cmake \
  git \
  qt6-qtbase-devel \
  qt6-qtdeclarative-devel \
  qt6-qtnetwork-devel \
  qt6-qtmultimedia-devel \
  kf6-kirigami-devel \
  kf6-kcoreaddons-devel \
  extra-cmake-modules \
  libwebsockets-devel
```

### Arch Linux

```bash
sudo pacman -S \
  base-devel \
  cmake \
  git \
  qt6-base \
  qt6-declarative \
  kf6-kirigami \
  kf6-coreaddons \
  extra-cmake-modules \
  libwebsockets
```

### Alpine Linux

```bash
apk add --no-cache \
  alpine-sdk \
  cmake \
  git \
  qt6-qtbase-dev \
  qt6-qtdeclarative-dev \
  libwebsockets-dev \
  kf6-kirigami-dev
```

## Build Steps

### 1. Clone Repository

```bash
git clone https://github.com/OpenVoiceOS/mycroft-gui-qt6.git
cd mycroft-gui-qt6
```

### 2. Create Build Directory

```bash
mkdir build
cd build
```

### 3. Configure with CMake

**Release Build** (optimized, smaller binary):
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

**Debug Build** (with symbols, useful for debugging):
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

**With Testing**:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
```

### 4. Build

**Using Make**:
```bash
make -j$(nproc)
```

**Using Ninja** (faster):
```bash
ninja
```

### 5. Verify Build Success

Check for:
- `libmycroft-gui-qt6.so` (library file, ~6 MB)
- `servertest`, `modeltest`, etc. (test executables if BUILD_TESTING=ON)
- No error messages (warnings are OK)

## Running Tests

After building with `-DBUILD_TESTING=ON`:

```bash
# Run all tests
ctest

# Or run individually
./autotests/servertest
./autotests/modeltest
./autotests/stresstest
./autotests/message_routing_test
./autotests/qml_framework_components_test
```

**Expected Output**:
```
Test project mycroft-gui-qt6/build
    Start 1: servertest
1/5 Test #1: servertest ...........   Passed   10.23 sec
2/5 Test #2: modeltest ...........   Passed    5.41 sec
3/5 Test #3: stresstest ..........   Passed   30.12 sec
4/5 Test #4: message_routing_test   Passed    0.85 sec
5/5 Test #5: qml_framework_components_test Passed 1.02 sec

100% tests passed, 0 tests failed out of 5
```

## Installation

### System-Wide Installation

```bash
sudo make install
```

This installs:
- Library: `/usr/local/lib/libmycroft-gui-qt6.so`
- QML modules: `/usr/local/lib/qt6/qml/Mycroft/`
- Headers: `/usr/local/include/mycroft-gui-qt6/`

### Custom Installation Path

```bash
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/mycroft-gui-qt6
make install
```

### Development Installation (no sudo needed)

```bash
# Just build, don't install globally
# Library available at: ./build/libmycroft-gui-qt6.so
# Use with: LD_LIBRARY_PATH=./build:$LD_LIBRARY_PATH
```

## Build Configuration Options

### CMake Variables

```bash
# Set build type
-DCMAKE_BUILD_TYPE=Release|Debug

# Enable testing
-DBUILD_TESTING=ON

# Installation prefix
-DCMAKE_INSTALL_PREFIX=/usr/local

# Use specific compiler
-DCMAKE_CXX_COMPILER=/usr/bin/clang++

# Enable optimizations
-DCMAKE_CXX_FLAGS="-O3 -march=native"
```

## Troubleshooting

### CMake Errors

**Error**: "Could not find a package configuration file for Qt6"

**Solution**: Install Qt6 development packages (see Dependency Installation above)

```bash
# Verify Qt6 installation
qmake6 --version
```

### Build Errors

**Error**: "fatal error: QQmlEngine: No such file or directory"

**Solution**: Qt6 QML development files not installed

```bash
# Ubuntu/Debian
sudo apt-get install qt6-qml-dev

# Fedora
sudo dnf install qt6-qtdeclarative-devel

# Arch
sudo pacman -S qt6-declarative
```

**Error**: "undefined reference to `qt_static_MetaObject'"

**Solution**: Qt6 was not found or is corrupted. Try:

```bash
rm -rf build
mkdir build && cd build
cmake ..
make clean
make -j4
```

### Test Failures

**Tests won't compile**: Ensure BUILD_TESTING=ON was passed to cmake

```bash
cmake .. -DBUILD_TESTING=ON
```

**Tests fail with network errors**: Normal if OVOS Core not running

The network tests expect a server on localhost:18181

```bash
# Either start OVOS Core, or just run non-network tests
./autotests/message_routing_test
./autotests/qml_framework_components_test
```

## Performance Tips

### Faster Builds

1. **Use Ninja instead of Make**:
```bash
sudo apt-get install ninja-build  # Ubuntu/Debian
cmake .. -GNinja
ninja
```

2. **Use ccache for incremental builds**:
```bash
sudo apt-get install ccache
cmake .. -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
```

3. **Parallel compilation**:
```bash
make -j$(nproc)  # Use all CPU cores
```

4. **Release build (smaller, faster)**:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3"
```

### Smaller Binary

Strip debug symbols after build:
```bash
strip build/libmycroft-gui-qt6.so
# Before: 6.1 MB, After: 2.3 MB
```

## Cross-Compilation

### For ARM64 (Raspberry Pi 4, etc.)

```bash
# Install ARM toolchain
sudo apt-get install g++-aarch64-linux-gnu pkg-config-aarch64-linux-gnu

# Create toolchain file
cat > arm64-toolchain.cmake << 'EOF'
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
set(CMAKE_FIND_ROOT_PATH /usr/aarch64-linux-gnu)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
EOF

# Build
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../arm64-toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/opt/mycroft-gui-arm64

make -j$(nproc)
```

### For ARM 32-bit (Raspberry Pi Zero, etc.)

```bash
# Install 32-bit toolchain
sudo apt-get install g++-arm-linux-gnueabihf pkg-config-arm-linux-gnueabihf

# Create toolchain file
cat > arm32-toolchain.cmake << 'EOF'
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
set(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabihf)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
EOF

# Build
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../arm32-toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release

make -j$(nproc)
```

## Docker Build

For containerized builds without polluting your host system:

```dockerfile
# Build stage
FROM ubuntu:22.04 AS builder

WORKDIR /build

RUN apt-get update && apt-get install -y \
    build-essential cmake pkg-config git \
    qt6-base-dev qt6-declarative-dev qt6-multimedia-dev \
    libqt6websockets6-dev \
    extra-cmake-modules libkf6kirigami-dev libkf6coreaddons-dev

RUN git clone https://github.com/OpenVoiceOS/mycroft-gui-qt6.git
WORKDIR /build/mycroft-gui-qt6

RUN mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc) && \
    make install DESTDIR=/install

# Runtime stage
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libqt6core6 libqt6gui6 libqt6qml6 libqt6network6 \
    libqt6websockets6 libqt6multimedia6 \
    libkf6kirigami6 libkf6coreaddons6 && \
    rm -rf /var/lib/apt/lists/*

COPY --from=builder /install/usr/local/lib /usr/local/lib
COPY --from=builder /install/usr/local/lib/qt6/qml /usr/local/lib/qt6/qml

ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
ENV QML_IMPORT_PATH=/usr/local/lib/qt6/qml:$QML_IMPORT_PATH

RUN ldconfig
```

Build with:
```bash
docker build -t mycroft-gui-qt6:latest .
docker run -it mycroft-gui-qt6:latest bash
```

## Verification Steps

### After Build

Verify the library was built correctly:

```bash
# Check file type
file build/libmycroft-gui-qt6.so
# Should output: ELF 64-bit LSB shared object

# Check dependencies
ldd build/libmycroft-gui-qt6.so | grep -E "Qt6|KF6"

# Check library symbols
nm -D build/libmycroft-gui-qt6.so | head -20
```

### After Installation

Verify installation was successful:

```bash
# Check library installed
ls -lh /usr/local/lib/libmycroft-gui-qt6.so

# Check QML plugin installed
ls -la /usr/local/lib/qt6/qml/Mycroft/

# Verify library can be loaded
ldconfig -p | grep mycroft-gui

# Check QML can find module
qmlimportscanner -rootPath /usr/local/lib/qt6/qml | grep -i mycroft
```

## Advanced Build Options

### Enable Link-Time Optimization

For better performance at the cost of longer build time:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-flto=auto -O3" \
    -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON

make -j$(nproc)
```

### Build with Address Sanitizer

For debugging memory issues:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-fsanitize=address -fno-omit-frame-pointer"

make -j$(nproc)
./autotests/message_routing_test  # Will show any memory errors
```

### Build with GCC vs Clang

To explicitly use Clang instead of GCC:

```bash
cmake .. -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_BUILD_TYPE=Release

make -j$(nproc)
```

## Maintenance & Cleanup

### Clean Build Directory

```bash
cd build
make clean          # Removes built files, keeps configuration
rm -rf *            # Complete clean (removes configuration too)
```

### Remove Installed Files

```bash
sudo make uninstall
# Or manually
sudo rm /usr/local/lib/libmycroft-gui-qt6.so
sudo rm -rf /usr/local/lib/qt6/qml/Mycroft/
```

### Update Build

After pulling new changes:

```bash
cd build
cmake ..            # Reconfigure in case CMakeLists.txt changed
make -j$(nproc)    # Rebuild
```

## Documentation References

- **ARCHITECTURE.md** — System design, component interactions, protocol flow
- **CODE_GUIDE.md** — Qt concepts explained for non-Qt developers, debugging tips
- **COMPONENTS.md** — Detailed API reference, method documentation, templates
- **README.md** — Project overview and quick reference

## Getting Help

If you encounter issues:

1. Check this troubleshooting section above
2. Review ARCHITECTURE.md or CODE_GUIDE.md for context
3. Search existing GitHub issues
4. Report new issues with: CMake output, error messages, OS/distro info
