#!/bin/bash
# Compile the mycroft-gui-qt6 Qt6 GUI application
# Usage: ./compile_gui.sh [install_prefix]
# Default install prefix: /tmp/mycroft-gui-install

set -e

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INSTALL_PREFIX="${1:-/tmp/mycroft-gui-install}"
BUILD_DIR="${SCRIPT_DIR}/build"
JOBS=$(nproc)

echo "=========================================="
echo "mycroft-gui-qt6 Compilation Script"
echo "=========================================="
echo "Project directory: $SCRIPT_DIR"
echo "Build directory: $BUILD_DIR"
echo "Install prefix: $INSTALL_PREFIX"
echo "Parallel jobs: $JOBS"
echo "=========================================="
echo ""

# Clean previous build if requested
if [ -d "$BUILD_DIR" ]; then
    read -p "Remove existing build directory? (y/n) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "Cleaning previous build..."
        rm -rf "$BUILD_DIR"
    fi
fi

# Configure CMake
echo "Step 1: Configuring CMake..."
if ! cmake -B "$BUILD_DIR" -S "$SCRIPT_DIR" \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O2 -g"; then
    echo "ERROR: CMake configuration failed"
    exit 1
fi
echo "✓ CMake configuration complete"
echo ""

# Build
echo "Step 2: Building project..."
if ! cmake --build "$BUILD_DIR" --parallel "$JOBS"; then
    echo "ERROR: Build failed"
    exit 1
fi
echo "✓ Build complete"
echo ""

# Install
echo "Step 3: Installing to $INSTALL_PREFIX..."
if ! cmake --build "$BUILD_DIR" --target install; then
    echo "ERROR: Installation failed"
    exit 1
fi
echo "✓ Installation complete"
echo ""

# Verify installation
echo "Step 4: Verifying installation..."
if [ ! -f "$INSTALL_PREFIX/bin/ovos-gui-app" ]; then
    echo "ERROR: ovos-gui-app executable not found at $INSTALL_PREFIX/bin/ovos-gui-app"
    exit 1
fi

echo "✓ All required files verified"
echo ""

# Summary
echo "=========================================="
echo "Compilation Successful!"
echo "=========================================="
echo "Install prefix: $INSTALL_PREFIX"
echo "Executable: $INSTALL_PREFIX/bin/ovos-gui-app"
echo ""
echo "To launch the GUI, run:"
echo "  ./launch_gui.sh $INSTALL_PREFIX"
echo "=========================================="
