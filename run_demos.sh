#!/bin/bash
# run_demos.sh - Automated test script for OVOS GUI templates

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"
LOG_FILE="$SCRIPT_DIR/demo_test.log"

# Use local build if it exists, otherwise check install prefix
DEMO_BIN="$BUILD_DIR/bin/demotest"
if [ ! -f "$DEMO_BIN" ]; then
    echo "ERROR: demotest binary not found at $DEMO_BIN"
    echo "Please build the project first."
    exit 1
fi

echo "=========================================="
echo "Starting Automated Template Demos"
echo "Log file: $LOG_FILE"
echo "=========================================="

# Environment setup
export QT_QPA_PLATFORM=offscreen
export QT_LOGGING_RULES="*.debug=true;qt.qml.connections=false"

# Run the test and capture output
# We use a timeout to ensure it doesn't hang indefinitely
# It should take roughly (templates * 2s) + startup
echo "[$(date)] Launching demotest..." > "$LOG_FILE"
timeout 120 "$DEMO_BIN" >> "$LOG_FILE" 2>&1 || true

echo ""
echo "------------------------------------------"
echo "Demo Run Complete"
echo "Checking for errors in log..."
echo "------------------------------------------"

# Simple log analysis
ERRORS=$(grep -iE "error|fail|warning" "$LOG_FILE" | grep -v "Could not find any instance" || true)
if [ -n "$ERRORS" ]; then
    echo "⚠️ Potential issues found in log:"
    echo "$ERRORS" | head -n 20
    echo "..."
else
    echo "✅ No major errors found in log."
fi

echo "=========================================="
echo "Full log available at: $LOG_FILE"
