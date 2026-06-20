#!/bin/bash
# Launch the mycroft-gui-qt6 Qt6 GUI application
# Usage: ./launch_gui.sh [install_prefix] [-- additional_args]
# Default install prefix: /tmp/mycroft-gui-install

set -e

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INSTALL_PREFIX="${1:-/tmp/mycroft-gui-install}"

# Extract any additional arguments after --
ADDITIONAL_ARGS=()
if [[ "${@}" == *" -- "* ]]; then
    shift
    ADDITIONAL_ARGS=("$@")
fi

# Verify installation
if [ ! -d "$INSTALL_PREFIX" ]; then
    echo "ERROR: Install prefix directory does not exist: $INSTALL_PREFIX"
    echo ""
    echo "Please compile the GUI first:"
    echo "  ./compile_gui.sh $INSTALL_PREFIX"
    exit 1
fi

if [ ! -f "$INSTALL_PREFIX/bin/ovos-gui-app" ]; then
    echo "ERROR: ovos-gui-app executable not found at $INSTALL_PREFIX/bin/ovos-gui-app"
    exit 1
fi

if [ ! -f "$INSTALL_PREFIX/lib/qml/OVOS/libovosplugin.so" ]; then
    echo "ERROR: libovosplugin.so not found at $INSTALL_PREFIX/lib/qml/OVOS/libovosplugin.so"
    exit 1
fi

# Set environment variables
export QML_IMPORT_PATH="$INSTALL_PREFIX/lib/qml"
export LD_LIBRARY_PATH="$INSTALL_PREFIX/lib/qml/OVOS:${LD_LIBRARY_PATH}"
export QML2_IMPORT_PATH="$INSTALL_PREFIX/lib/qml"

# Optional: enable Qt debug output (uncomment to debug)
# export QT_DEBUG_PLUGINS=1
# export QML_IMPORT_TRACE=1

echo "=========================================="
echo "Launching mycroft-gui-qt6"
echo "=========================================="
echo "Install prefix: $INSTALL_PREFIX"
echo "QML_IMPORT_PATH: $QML_IMPORT_PATH"
echo "LD_LIBRARY_PATH includes: $INSTALL_PREFIX/lib/qml/OVOS"
echo "=========================================="
echo ""

# Check if we have a display
if [ -z "$DISPLAY" ] && [ -z "$WAYLAND_DISPLAY" ]; then
    echo "WARNING: No X11 or Wayland display detected."
    echo "The GUI may not display properly without a graphical environment."
    echo ""
    read -p "Continue anyway? (y/n) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# Launch the application
echo "Starting application..."
exec "$INSTALL_PREFIX/bin/ovos-gui-app"
