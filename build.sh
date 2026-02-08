#!/bin/bash
as --32 src/boot/boot.s -o boot.o
gcc -m32 -ffreestanding -c src/kernel/kernel.c -o kernel.o
gcc -m32 -ffreestanding -c src/graphics/gfx.c -o gfx.o
ld -m elf_i386 -T linker.ld -o flux-kernel boot.o kernel.o gfx.o

# Check for Multiboot 1 (not 2)
grub-file --is-x86-multiboot flux-kernel

if [ $? -ne 0 ]; then
    echo "ERROR: Kernel is not Multiboot compliant."
    exit 1
fi

mkdir -p isodir/boot/grub
cp flux-kernel isodir/boot/flux-kernel

cat > isodir/boot/grub/grub.cfg << 'CFGEOF'
set timeout=0
set default=0

# Don't try graphics mode - let kernel handle it
terminal_output console

menuentry "Flux-OS" {
    multiboot /boot/flux-kernel
    boot
}
CFGEOF

grub-mkrescue -o flux-os.iso isodir
echo "Build Complete!"
