# FLUX-OS Installation & Setup Guide

## 👨‍💻 Developer

**Oliver Lebaigue** - Sole Developer  
*Age: 16 | Passionate about minimal OS design and retro computing*

---

## 🚀 Quick Start

```bash
# Clone the repository
git clone https://github.com/whiteo55/Flux-OS.git
cd Flux-OS

# Build the Operating System
bash build.sh

# Run in QEMU
DISPLAY=:1 qemu-system-i386 -m 512 -cdrom flux-os.iso -vga std
```

---

## 📋 Requirements

### Operating System

**Recommended**: Linux (Ubuntu 22.04 LTS or newer)  
**Also Supported**:  
- Debian 11+
- Fedora 36+
- Arch Linux
- macOS (with Homebrew toolchain)
- Windows (WSL2 recommended)

### Hardware

**Minimum**:
- CPU: x86 processor with SSE2 support
- RAM: 256 MB
- Storage: 50 MB free space
- Display: VGA-compatible monitor

**Recommended**:
- CPU: x86-64 processor
- RAM: 512 MB or more
- Storage: 1 GB free space (for development)
- Display: VESA-compatible monitor

### Software Dependencies

#### Ubuntu/Debian

```bash
# Core build tools
sudo apt-get update
sudo apt-get install -y build-essential binutils

# Cross-compiler toolchain (32-bit)
sudo apt-get install -y gcc-multilib g++-multilib

# Assembler and linker
sudo apt-get install -y gas

# Bootloader tools
sudo apt-get install -y grub-pc-bin grub-common

# ISO creation
sudo apt-get install -y xorriso

# Emulator
sudo apt-get install -y qemu-system-x86

# Additional tools
sudo apt-get install -y nasm make
```

#### Fedora

```bash
# Core build tools
sudo dnf install -y gcc binutils make

# Cross-compiler toolchain
sudo dnf install -y gcc-c++-multilib glibc-devel.i686

# Bootloader tools
sudo dnf install -y grub2-tools

# Emulator
sudo dnf install -y qemu-system-x86

# Additional tools
sudo dnf install -y nasm
```

#### Arch Linux

```bash
# Core build tools
sudo pacman -S --needed base-devel

# Cross-compiler (if using AUR)
yay -S gcc-multilib

# Bootloader tools
sudo pacman -S grub

# Emulator
sudo pacman -S qemu-full

# Additional tools
sudo pacman -S nasm
```

#### macOS (Homebrew)

```bash
# Install Homebrew if not installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install gcc binutils nasm qemu
brew install --cask xquartz
```

#### Windows (WSL2)

```bash
# Install WSL2 Ubuntu from Microsoft Store
# Then run:
sudo apt-get update
sudo apt-get install -y build-essential gcc-multilib g++-multilib nasm qemu-system-x86 grub-pc-bin xorriso
```

---

## 🔧 Detailed Installation

### Step 1: Install Build Tools

#### GCC Cross-Compiler (Recommended)

For better control over the build environment:

```bash
# Create toolchain directory
mkdir -p ~/toolchain
cd ~/toolchain

# Download binutils
curl -O https://ftp.gnu.org/gnu/binutils/binutils-2.40.tar.gz
tar xzf binutils-2.40.tar.gz
mkdir build-binutils
cd build-binutils
../binutils-2.40/configure --target=i386-elf --prefix=$HOME/toolchain --disable-nls
make -j$(nproc)
make install

# Download GCC
cd ~/toolchain
curl -O https://ftp.gnu.org/gnu/gcc/gcc-12.2.0/gcc-12.2.0.tar.gz
tar xzf gcc-12.2.0.tar.gz
mkdir build-gcc
cd build-gcc
../gcc-12.2.0/configure --target=i386-elf --prefix=$HOME/toolchain --disable-nls --enable-languages=c,c++ --without-headers
make -j$(nproc)
make install

# Add to PATH
echo 'export PATH=$HOME/toolchain/bin:$PATH' >> ~/.bashrc
source ~/.bashrc
```

#### System GCC (Alternative)

Using the system's GCC with multilib:

```bash
# Verify GCC installation
gcc --version
gcc -m32 -v

# If -m32 fails, install multilib
sudo apt-get install -y gcc-multilib g++-multilib
```

### Step 2: Install GRUB

#### Ubuntu/Debian

```bash
sudo apt-get install -y grub-pc-bin grub-common grub2-common

# Verify installation
grub-mkrescue --version
```

#### Manual GRUB Build

```bash
# Download GRUB source
cd ~/toolchain
curl -O https://ftp.gnu.org/gnu/grub/grub-2.06.tar.gz
tar xzf grub-2.06.tar.gz
cd grub-2.06

# Build (minimal configuration)
./configure --target=i386 --prefix=$HOME/toolchain --disable-werror
make -j$(nproc)
make install
```

### Step 3: Install QEMU

#### Ubuntu/Debian

```bash
sudo apt-get install -y qemu-system-x86

# Verify installation
qemu-system-i386 --version
```

#### macOS

```bash
brew install qemu
brew install --cask xquartz

# Add to PATH (if not already)
echo 'export PATH=/usr/local/opt/qemu/bin:$PATH' >> ~/.zshrc
source ~/.zshrc
```

#### Build from Source

```bash
cd ~/toolchain
curl -O https://download.qemu.org/qemu-8.0.0.tar.xz
tar xzf qemu-8.0.0.tar.xz
cd qemu-8.0.0
./configure --target-list=i386-softmmu --prefix=$HOME/toolchain
make -j$(nproc)
make install
```

### Step 4: Clone Repository

```bash
# Clone Flux-OS
git clone https://github.com/whiteo55/Flux-OS.git
cd Flux-OS

# Verify directory structure
ls -la
# Should show: build.sh, src/, isodir/, linker.ld, etc.
```

---

## 🛠️ Build Process

### Standard Build

```bash
# Make build script executable
chmod +x build.sh

# Run build
./build.sh

# Expected output:
# - flux-kernel binary
# - flux-os.iso bootable image
```

### Build with Debug Symbols

```bash
# Edit build.sh to add debug flags
CFLAGS="-g -O0 -DDEBUG"

# Rebuild
./build.sh
```

### Verbose Build

```bash
# Show all commands
bash -x build.sh
```

### Clean Build

```bash
# Remove all build artifacts
make clean

# Or manually:
rm -f *.o
rm -f flux-kernel
rm -f flux-os.iso
rm -rf isodir/boot/flux-kernel

# Rebuild
./build.sh
```

---

## 🖥️ Running Flux-OS

### QEMU Basic

```bash
# Basic run
qemu-system-i386 -m 512 -cdrom flux-os.iso -vga std

# With more memory
qemu-system-i386 -m 1024 -cdrom flux-os.iso -vga std

# With networking (if implemented)
qemu-system-i386 -m 512 -cdrom flux-os.iso -vga std -net nic -net user
```

### QEMU with Graphics Acceleration

```bash
# With OpenGL support
qemu-system-i386 -m 512 -cdrom flux-os.iso -vga virtio -display sdl,gl=on

# With remote display
qemu-system-i386 -m 512 -cdrom flux-os.iso -vga std -display vnc=:1
```

### QEMU with Debugging

```bash
# With GDB stub
qemu-system-i386 -m 512 -cdrom flux-os.iso -vga std -s -S

# Then connect with GDB in another terminal
i386-elf-gdb flux-kernel
(gdb) target remote localhost:1234
```

### Running Scripts

```bash
# Use provided run scripts
./run_qemu.sh      # Basic QEMU
./run_graphics.sh  # With VBE graphics
./run_gui.sh       # With GUI (when implemented)
./run_gui_sdl.sh   # With SDL rendering
```

### Display Issues?

```bash
# Set display manually
DISPLAY=:0 qemu-system-i386 -m 512 -cdrom flux-os.iso -vga std

# Or use virtual display (Xvfb)
sudo apt-get install -y xvfb
Xvfb :99 -screen 0 1024x768x24 &
DISPLAY=:99 qemu-system-i386 -m 512 -cdrom flux-os.iso -vga std
```

---

## 🔍 Testing

### Automated Testing

```bash
# Run tests (when implemented)
make test
```

### Manual Testing Checklist

- [ ] Boot sequence completes
- [ ] GRUB menu appears
- [ ] Kernel loads successfully
- [ ] Text mode displays correctly
- [ ] VBE mode switches (when implemented)
- [ ] GUI renders (when implemented)
- [ ] Keyboard input works
- [ ] Mouse input works

### Bug Reporting

When reporting bugs, include:

1. **Environment**:
   ```bash
   # System info
   uname -a
   gcc --version
   qemu-system-i386 --version
   ```

2. **Build Output**:
   ```bash
   ./build.sh 2>&1 | tee build.log
   ```

3. **QEMU Output**:
   ```bash
   qemu-system-i386 -m 512 -cdrom flux-os.iso -vga std -d int,cpu_reset 2>&1 | tee qemu.log
   ```

4. **Screenshots**: If visual issues

---

## 🐛 Troubleshooting

### Build Errors

#### "gcc: error: unrecognized command line option '-m32'"

**Solution**: Install multilib
```bash
sudo apt-get install -y gcc-multilib g++-multilib
```

#### "grub-mkrescue: error: cannot find a device for"

**Solution**: Install xorriso
```bash
sudo apt-get install -y xorriso
```

#### "i386-elf-gcc: command not found"

**Solution**: Add to PATH or use system GCC
```bash
export PATH=$HOME/toolchain/bin:$PATH
# Or use system GCC instead
gcc -m32 -ffreestanding -c kernel.c -o kernel.o
```

### Runtime Errors

#### Black Screen in QEMU

**Solution**: Try different VGA modes
```bash
# Try std VGA
qemu-system-i386 -m 512 -cdrom flux-os.iso -vga std

# Try Cirrus (legacy)
qemu-system-i386 -m 512 -cdrom flux-os.iso -vga cirrus

# Try no VGA (text mode only)
qemu-system-i386 -m 512 -cdrom flux-os.iso -nographic
```

#### Slow Performance

**Solution**: Increase memory or use KVM
```bash
# With KVM (Linux)
qemu-system-i386 -m 512 -cdrom flux-os.iso -vga std -enable-kvm

# More memory
qemu-system-i386 -m 2048 -cdrom flux-os.iso -vga std
```

---

## 📚 Additional Resources

### Related Documentation

- [README.md](README.md) - Main project overview
- [VISION.md](VISION.md) - Design philosophy and vision
- [DESIGN.md](UI/UX specifications)
- [TYPOGRAPHY.md](Font system documentation)
- [GLYPH.md](Glyph interface specifications)
- [ARCHITECTURE.md](System architecture)
- [CONTRIBUTING.md](Developer guidelines)

### External Resources

- [OSDev Wiki](https://wiki.osdev.org) - x86 operating system development
- [OSDev Forum](https://forum.osdev.org) - Community support
- [QEMU Documentation](https://www.qemu.org/docs/) - Emulator documentation
- [GRUB Manual](https://www.gnu.org/software/grub/manual/) - Bootloader documentation
- [Intel Manuals](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html) - CPU documentation

---

## 📝 Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2024 | Initial documentation |

---

*FLUX-OS Installation & Setup Guide v1.0*  
*Created by Oliver Lebaigue (Age 16)*  
*Nothing OS Inspired Desktop Experience*

