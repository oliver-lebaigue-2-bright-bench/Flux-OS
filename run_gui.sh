#!/bin/bash
# Simple script to build and run Flux-OS in QEMU with GUI display
cd /workspaces/Flux-OS

# Trap Ctrl+C to kill QEMU processes
cleanup() {
    echo ""
    echo "Shutting down QEMU..."
    pkill -f "qemu-system-x86_64.*flux-os.iso" 2>/dev/null
    pkill -f "qemu-system-i386.*flux-os.iso" 2>/dev/null
    sleep 1
    exit 0
}
trap cleanup SIGINT SIGTERM

echo "========================================"
echo "  FLUX-OS Build & Run Script"
echo "========================================"
echo ""

echo "Step 1: Building Flux-OS..."
echo "----------------------------------------"
bash build.sh
if [ $? -ne 0 ]; then
    echo "ERROR: Build failed!"
    exit 1
fi

echo ""
echo "Step 2: Launching QEMU with VBE graphics..."
echo "----------------------------------------"
echo "NOTE: If GUI doesn't appear, the OS is running in text mode."
echo "Check the QEMU window for boot messages."
echo ""
echo "Press Ctrl+C to quit"
echo ""

# Run QEMU with VBE support
# -vga std: Use standard VGA with VBE support
# -display sdl: Use SDL for display output
# These options enable the VESA BIOS Extensions that our kernel needs

# Try with VBE enabled (no serial conflicts)
echo "Starting QEMU with VBE support..."
qemu-system-i386 \
    -cdrom flux-os.iso \
    -boot d \
    -m 512 \
    -vga std \
    -display sdl \
    -monitor stdio \
    -no-reboot 2>&1 || {
    # Fallback to simpler options if above fails
    echo "Falling back to basic QEMU options..."
    qemu-system-i386 \
        -cdrom flux-os.iso \
        -boot d \
        -m 512 \
        -vga std \
        -display sdl \
        -no-reboot 2>&1
}

