#!/bin/bash
# Simple script to build and run Flux-OS in QEMU with GUI display
cd /workspaces/Flux-OS

# Trap Ctrl+C to kill QEMU processes
cleanup() {
    echo ""
    echo "Shutting down QEMU..."
    pkill -f "qemu-system-x86_64.*flux-os.iso"
    sleep 1
    exit 0
}
trap cleanup SIGINT SIGTERM

echo "Building Flux-OS..."
bash build.sh > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "Build failed!"
    bash build.sh
    exit 1
fi
echo "Launching QEMU with graphics..."
echo "Press Ctrl+C to quit"
DISPLAY=:1 qemu-system-x86_64 -cdrom flux-os.iso -boot d -m 512 -display sdl
