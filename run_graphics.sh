#!/bin/bash

# Build the kernel if not already built
echo "Building Flux-OS..."
bash build.sh

if [ ! -f flux-os.iso ]; then
    echo "ERROR: Failed to build flux-os.iso"
    exit 1
fi

echo "Launching QEMU with graphics support..."
echo "QEMU window should open showing the GUI"
echo ""

# Kill any existing QEMU processes
pkill -9 qemu 2>/dev/null || true

# Launch QEMU with explicit graphics configuration
# -vga std: Use standard VGA device
# -display gtk: Use GTK display (good for X11)
DISPLAY=:1 qemu-system-i386 \
    -m 512 \
    -cdrom flux-os.iso \
    -vga std \
    -display gtk,grab-on-hover=off \
    2>&1 &

QEMU_PID=$!
echo "QEMU PID: $QEMU_PID"

# Wait for QEMU to start
sleep 2

# Handle Ctrl+C
cleanup() {
    echo ""
    echo "Shutting down QEMU..."
    kill $QEMU_PID 2>/dev/null || true
    exit 0
}

trap cleanup SIGINT SIGTERM

# Wait for QEMU process
wait $QEMU_PID 2>/dev/null || true
