# Flux-OS ARM Migration Plan

## Executive Summary

This document outlines a comprehensive plan to migrate Flux-OS from its current x86/i386 architecture to ARM architecture. The migration is substantial but achievable, requiring changes to boot code, kernel internals, drivers, build system, and bootloader.

---

## Table of Contents

1. [Current Architecture Analysis](#current-architecture-analysis)
2. [Target ARM Architecture](#target-arm-architecture)
3. [Technical Changes Required](#technical-changes-required)
4. [Migration Phases](#migration-phases)
5. [File-by-File Changes](#file-by-file-changes)
6. [Build System Updates](#build-system-updates)
7. [Testing Strategy](#testing-strategy)
8. [Risks and Mitigation](#risks-and-mitigation)
9. [Estimated Effort](#estimated-effort)

---

## Current Architecture Analysis

### Current Technology Stack

| Component | Current Implementation |
|-----------|----------------------|
| **CPU Architecture** | x86/i386 (32-bit Intel/AMD) |
| **Bootloader** | GRUB 2 (Multiboot v1) |
| **Assembly Syntax** | AT&T syntax (GNU Assembler) |
| **Compiler** | GCC with `-m32 -ffreestanding` |
| **Linker** | GNU LD (`ld -m elf_i386`) |
| **Kernel Entry** | Multiboot compliant |
| **Memory Layout** | Kernel at 1MB (0x100000) |
| **Graphics** | VGA Text Mode + VBE Framebuffer |
| **Build System** | Bash scripts + GNU toolchain |

### Key Files and Their Architecture Dependencies

```
src/boot/boot.s          ← x86 assembly, Multiboot header, GDT setup
src/kernel/kernel.c      ← x86 I/O ports, BIOS calls, VGA memory
src/libc_compat.c        ← Mostly portable
src/graphics/gfx.c       ← Mostly portable (framebuffer operations)
src/gui/*.c              ← Mostly portable
linker.ld                ← x86 memory layout, ELF32-i386
build.sh                 ← i386 compilation flags
isodir/boot/grub/grub.cfg ← GRUB Multiboot config
```

---

## Target ARM Architecture

### Recommended Target: ARMv7-A (32-bit) or ARMv8-A (64-bit)

For simplicity and broad hardware availability, **ARMv7-A with hardware floating-point (VFPv3)** is recommended as the initial target.

### Hardware Targets (Desktop ARM)

| Priority | Device | Notes |
|----------|--------|-------|
| 1 | QEMU ARM Virtual Machine | Fastest development/testing cycle |
| 2 | **Raspberry Pi 500** | User's test device, Pi 4 with keyboard, Cortex-A72 |
| 3 | Raspberry Pi 4 | Alternative, same BCM2711 SoC |
| 4 | ARM Laptops (Snapdragon X Elite, Apple M-series) | Desktop-class ARM |

### Why ARMv8-A (AArch64) as Primary Target?

For a desktop OS targeting ARM laptops and Raspberry Pi 500, **ARMv8-A in 64-bit mode (AArch64)** is recommended:

- **Raspberry Pi 500/4:** Broadcom BCM2711, 4x Cortex-A72 @ 1.5GHz, 64-bit capable
- **ARM Laptops:** Snapdragon X Elite, Apple M-series - all 64-bit
- **64-bit OS:** Better performance, larger address space
- **Future-proof:** Industry standard moving forward

### Secondary Target: ARMv7-A (32-bit)

For legacy Raspberry Pi 2/3 support:
- 32-bit compatibility mode
- Slightly simpler boot process
- Large address space (32-bit)
- Hardware FPU (VFPv3) for floating-point
- MMU with standard page tables
- Well-supported by QEMU
- Good cross-compiler support

---

### Raspberry Pi 500/4 Technical Specifications

| Specification | Details |
|---------------|---------|
| **SoC** | Broadcom BCM2711 (Pi 500/4) / BCM2712 (Pi 5) |
| **CPU** | 4x ARM Cortex-A72 @ 1.5 GHz (Pi 500/4) / Cortex-A76 @ 2.4 GHz (Pi 5) |
| **GPU** | VideoCore VI |
| **RAM** | 2GB/4GB/8GB LPDDR4 (Pi 500: typically 4GB or 8GB) |
| **USB** | 2x USB 3.0, 2x USB 2.0 |
| **Storage** | micro-SD card, USB 3.0 storage |
| **Video** | 2x micro-HDMI (4K60) |
| **Networking** | Gigabit Ethernet, Wi-Fi 5, Bluetooth 5.0 |
| **GPIO** | 40-pin header |

**Key Pi 500-specific notes:**
- Pi 500 = Pi 4 Model B with integrated keyboard and case
- Same BCM2711 SoC as Pi 4 Model B
- Fixed keyboard and trackpad (USB HID)
- 5V USB-C power (same as Pi 4)
- Excellent for desktop OS testing

---



## Technical Changes Required

### 1. Bootloader (Major Changes)

**Current:** GRUB Multiboot
**Required:** ARM-specific bootloader

#### Options for Desktop ARM:

| Bootloader | Pros | Cons |
|------------|------|------|
| **Raspberry Pi 5 Bootloader** | Native, well-documented | Raspberry-specific |
| **U-Boot** | Industry standard, multi-platform | Complex configuration |
| **Limine** | Modern, cross-platform | ARM64 support evolving |
| **EDK II/UEFI** | Standard for ARM laptops | Heavy, complex |

#### Raspberry Pi 500 Boot (Primary Target)

The Raspberry Pi 500 (Pi 4 with keyboard) has a built-in bootloader that loads `kernel8.img` (64-bit) from the SD card:

```
SD Card Layout:
├── bootcode.bin    (pre-installed, loads start*.elf)
├── start*.elf      (pre-installed, GPU firmware)
├── fixup*.dat      (pre-installed)
├── config.txt      (boot configuration)
├── kernel8.img     (Flux-OS 64-bit kernel)
├── device_tree.dtb (Device Tree blob)
└── cmdline.txt     (kernel command line)
```

**config.txt for Pi 500:**
```
arm_64bit=1
kernel=kernel8.img
device_tree_address=0x1f0000
gpu_mem=128
enable_uart=1
dtparam=audio=on
dtparam=spi=on
```

**cmdline.txt:**
```
console=serial0,115200 console=tty1 root=/dev/mmcblk0p2 rootwait
```

Note: Pi 500 uses the same boot process as Pi 4 Model B - the bootloader is in ROM and loads boot files from the SD card.

#### ARM Laptop Boot (UEFI)

Modern ARM laptops use UEFI firmware:
- Kernel loaded as EFI application or via boot manager
- Device Tree or ACPI tables provided
- Standard EFI boot services available

### 2. Boot Assembly (Complete Rewrite)

**Current boot.s (x86):**
```assembly
.set MAGIC, 0x1BADB002  # Multiboot magic
# ... Multiboot header ...
_start:
    cli
    mov $stack_top, %esp
    push %ebx
    call kernel_main
```

**Required ARM Entry (new boot.s):**
```assembly
.section .text
.global _start
.type _start, %function

_start:
    # Disable interrupts
    cpsid if
    
    # Set up stack (grow downward)
    ldr sp, =stack_top
    
    # Enable FPU if present
    mrc p15, 0, r0, c1, c0, 2  @ Read CPACR
    orr r0, r0, #(0x3 << 20)   @ Enable full access
    mcr p15, 0, r0, c1, c0, 2  @ Write CPACR
    vmrs r0, fpexc
    orr r0, r0, #0x40000000    @ Enable FP exception
    vmsr fpexc, r0
    
    # Enable MMU and caches
    bl __mmu_setup
    
    # Call C kernel
    bl kernel_main
```

### 3. Kernel Architecture Dependencies

#### 3.1 Port I/O (x86) → Memory-Mapped I/O (ARM)

**Current (x86):**
```c
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
```

**Required (ARM):**
ARM uses memory-mapped I/O exclusively. The equivalent would be:

```c
// Define hardware register addresses
#define SERIAL_BASE      0x3F201000  @ Raspberry Pi UART
#define UART_DR          (SERIAL_BASE + 0x00)
#define UART_FR          (SERIAL_BASE + 0x18)

static inline uint8_t inb(uint32_t port) {
    return *(volatile uint8_t*)port;
}

static inline void outb(uint32_t port, uint8_t val) {
    *(volatile uint8_t*)port = val;
}
```

#### 3.2 VGA Text Mode → Framebuffer

**Current:** Direct memory access to `0xB8000` (VGA text buffer)
**Required:** Memory-mapped framebuffer at device-specific address

The framebuffer address on ARM is typically provided by the bootloader via Device Tree or ATAGs:

```c
void kernel_main(uint32_t r0, uint32_t r1, uint32_t r2) {
    // r0: ARM machine type (not typically used)
    // r1: ATAG pointer (older ARM) or 0 for Device Tree
    // r2: Physical address of Device Tree blob
    
    #if USE_DEVICE_TREE
        void* fdt = (void*)r2;
        parse_fdt(fdt);
    #elif USE_ATAGS
        void* atag = (void*)r1;
        parse_atags(atag);
    #endif
}
```

#### 3.3 BIOS Interrupts → Direct Hardware

**Current:** `int $0x10` for VGA mode setting
**Required:** Direct GPU initialization or bootloader-provided framebuffer

The ARM architecture has no BIOS equivalent. All hardware access must be direct or via trusted firmware.

#### 3.4 Inline Assembly Changes

| x86 (AT&T) | ARM (ARMASM/GAS) |
|------------|------------------|
| `mov %eax, %ebx` | `mov r0, r1` |
| `push %ebx` | `push {r0-r3, lr}` |
| `pop %eax` | `pop {r0-r3, pc}` |
| `inb %1, %0` | `ldr r0, [r1]` |
| `outb %0, %1` | `str r0, [r1]` |
| `hlt` | `wfi` or infinite loop |
| `cli` | `cpsid i` |
| `sti` | `cpsie i` |

### 4. Memory Layout Changes

**Current linker.ld (x86):**
```ld
ENTRY(_start)
SECTIONS
{
    . = 1M;
    .text : { *(.multiboot) *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : { *(.bss) }
}
```

**Required linker.ld (ARM):**
```ld
ENTRY(_start)

/* ARM expects different load addresses for different SoCs */
/* QEMU Virt:   Load at 0x40000000, Execute at 0x40000000 */
/* Raspberry Pi: Load at 0x80000, Execute at 0x80000 */

phys = 0x40000000;  @ Default for QEMU Virt

SECTIONS
{
    . = phys;
    
    .text.boot : { *(.text.boot) }
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : { *(.bss) }
    
    /* Device Tree placeholder */
    _dtb_phys = .;
    .dtb : { *(.dtb) }
}
```

### 5. Data Structure Alignment

ARM has stricter alignment requirements:

| Data Type | x86 Behavior | ARM Behavior |
|-----------|--------------|--------------|
| `uint32_t` on 0x3 | Usually works | Unaligned access fault |
| `uint32_t` on 0x1 | May work | Fault |
| `uint64_t` on 0x3 | Works | Fault |

**Mitigation:** Use `__attribute__((aligned(4)))` or `__packed` appropriately.

---

## Migration Phases

### Phase 1: Foundation (2-3 weeks)

**Goals:** Set up ARM toolchain, create ARM entry point, verify QEMU boots

#### Tasks:
1. [ ] Install ARM64 (AArch64) cross-compiler toolchain

For Raspberry Pi 5 and ARM laptops (64-bit ARM):
```bash
# Ubuntu/Debian
sudo apt-get install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu binutils-aarch64-linux-gnu

# Verify installation
aarch64-linux-gnu-gcc --version
```

For ARM32 (ARMv7) legacy support (Pi 2/3):
```bash
sudo apt-get install gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf binutils-arm-linux-gnueabihf
```

**Note:** Raspberry Pi 500/4 uses 64-bit ARM (AArch64), so `aarch64-linux-gnu` toolchain is required for testing on real hardware. The Pi 500 runs in 64-bit mode by default.

2. [ ] Create ARM entry assembly (`src/boot/arm_entry.s`)
   - Vector table
   - Stack setup
   - MMU/caches initialization
   - Jump to C kernel

3. [ ] Create MMU setup code
   - Page table creation
   - MMU enable
   - Caches enable

4. [ ] Update linker script for ARM (`linker.ld`)
   - Correct memory regions
   - Device Tree placement

5. [ ] Create Device Tree source for QEMU (`board.dts`)
   - UART configuration
   - Framebuffer info
   - Memory layout

6. [ ] Update build script for ARM (`build.sh`)
   - Cross-compiler flags
   - ARM assembler flags

7. [ ] Verify boot in QEMU
   ```bash
   qemu-system-arm -M virt -cpu cortex-a15 -m 1G \
       -kernel build/flux-kernel.bin -dtb board.dtb \
       -serial stdio -display none
   ```

#### Deliverables:
- ARM cross-compilation works
- Kernel boots to basic UART output
- MMU and caches enabled

### Phase 2: Core Kernel (2-3 weeks)

**Goals:** Port kernel functionality to ARM, maintain API compatibility

#### Tasks:
1. [ ] Create architecture abstraction layer (`src/arch/arm/`)
   ```
   src/arch/arm/
   ├── arm_asm.s          @ Low-level assembly (context switch, interrupts)
   ├── cp15.c             @ CP15 register operations
   ├── interrupts.c       @ GIC setup, exception handlers
   ├── timer.c            @ System timer (arch timer or generic timer)
   ├── uart.c             @ PL011 UART driver
   └── mmu.c              @ Page table operations
   ```

2. [ ] Replace x86-specific code in kernel.c
   - Remove port I/O functions
   - Replace with memory-mapped I/O
   - Remove BIOS calls
   - Adapt Multiboot parsing to Device Tree/ATAGs

3. [ ] Create serial console driver for ARM
   - PL011 UART (common on QEMU, Raspberry Pi)
   - Mini UART (Raspberry Pi specific)

4. [ ] Implement interrupt handling
   - GIC (Generic Interrupt Controller) for ARMv7
   - Exception vectors
   - IRQ/FIQ handling

5. [ ] Implement system timer
   - ARM Generic Timer
   - PIT replacement

6. [ ] Update libc_compat for ARM
   - Heap allocator (already portable)
   - String functions (already portable)

#### Deliverables:
- UART serial console working
- Interrupt system functional
- Timer working (for future scheduling)

### Phase 3: Graphics System (2-3 weeks)

**Goals:** Port graphics to ARM framebuffer, maintain API compatibility

#### Tasks:
1. [ ] Detect framebuffer from Device Tree
   ```c
   struct framebuffer {
       uint64_t base;
       uint32_t width;
       uint32_t height;
       uint32_t pitch;
       uint32_t format;
   };
   
   int get_framebuffer(struct framebuffer* fb);
   ```

2. [ ] Update gfx.h for ARM
   ```c
   // Add ARM-specific optimizations
   #ifdef __arm__
   #define USE_ARM_NEON 1
   #define FB_FOURCC uint32_t
   #endif
   ```

3. [ ] Implement ARM NEON optimizations (optional)
   ```c
   void fill_rect_neon(int x, int y, int w, int h, uint32_t color) {
       uint32x4_t color_vec = vdupq_n_u32(color);
       // ... NEON fill loop
   }
   ```

4. [ ] Handle ARM big-endian vs little-endian framebuffer
   - VGA was always little-endian
   - ARM can be either (though usually little-endian)

5. [ ] Add Device Tree parser for framebuffer info

#### Deliverables:
- Framebuffer detected at boot
- All gfx_* functions work
- Desktop renders correctly

### Phase 4: Input System (1-2 weeks)

**Goals:** Port keyboard and mouse drivers to ARM

#### Tasks:
1. [ ] USB host controller support (EHCI/OHCI)
   - USB HID devices for keyboard/mouse
   - Or use PS/2 over USB emulation

2. [ ] USB HID driver
   - Keyboard protocol
   - Mouse protocol

3. [ ] Integrate with existing GUI input handling
   - Event system remains the same
   - Just replace low-level drivers

#### Deliverables:
- USB keyboard works
- USB mouse works
- GUI input functional

### Phase 5: Build and Testing (1 week)

**Goals:** Finalize build system, create test suite

#### Tasks:
1. [ ] Update package.json
   ```json
   {
     "cpu": ["arm", "armv7l", "aarch64"],
     "scripts": {
       "build:arm": "cross-compile for ARM",
       "build:x86": "existing build",
       "test:qemu": "test in QEMU ARM",
       "test:pi": "test on Raspberry Pi"
     }
   }
   ```

2. [ ] Create ARM-specific run scripts
   ```bash
   # run_qemu_arm.sh
   qemu-system-arm -M virt -cpu cortex-a15 -m 1G \
       -kernel build/flux-kernel.bin -dtb board.dtb \
       -serial stdio -display sdl
   ```

3. [ ] Add CI/CD for ARM builds
   - GitHub Actions with ARM runners
   - Or cross-compilation testing

#### Deliverables:
- Build works for both architectures
- QEMU test script functional
- Documentation updated

---

## File-by-File Changes

### 1. src/boot/boot.s → Complete Rewrite

| Original | New File | Action |
|----------|----------|--------|
| `src/boot/boot.s` | `src/boot/arm_entry.s` | Rewrite completely |
| `src/boot/boot.s` | `src/arch/arm/vectors.s` | ARM exception vectors |

**New file structure:**
```
src/boot/
├── arm_entry.s        @ Kernel entry point (ARM)
└── ...
src/arch/arm/
├── vectors.s          @ Exception vectors
├── cp15.s             @ CP15/MMU operations
└── ...
```

### 2. src/kernel/kernel.c → Heavy Modification

| Section | Changes |
|---------|---------|
| Includes | Add ARM-specific headers |
| Port I/O | Replace with memory-mapped I/O |
| VGA code | Remove completely |
| Multiboot | Replace with Device Tree/ATAGs |
| Serial | Rewrite for ARM UART |
| Main loop | Mostly unchanged |

**Key changes:**
```c
// Remove:
#include <stdint.h>  // Keep, but use arch-specific
#include <stddef.h>

// Add:
#include <arch/arm/arm.h>
#include <arch/arm/pl011.h>
#include <device_tree.h>

// Replace kernel_main signature:
#ifdef __x86_64__
void kernel_main(uint32_t mb_magic, uint32_t mb_info);
#else
// ARM: r0=machine type, r1=ATAG ptr or 0, r2=DTB ptr
void kernel_main(uint32_t r0, uint32_t r1, uint32_t r2);
#endif
```

### 3. src/graphics/gfx.c → Minor Changes

| Section | Changes |
|---------|---------|
| Framebuffer access | Works, but verify endianness |
| Pixel operations | Mostly unchanged |
| Font rendering | Mostly unchanged |

**Key changes:**
```c
// Add at top:
#ifdef __arm__
#include <arm/neon.h>
#endif

// Update set_pixel for ARM optimizations:
void set_pixel(int x, int y, uint32_t color) {
    // ... existing code ...
    
    #ifdef __arm__
    // ARM-specific fast path
    #endif
}
```

### 4. src/gui/*.c → Mostly Unchanged

The GUI code is already architecture-independent and should require minimal changes.

### 5. src/libc_compat.c/h → Mostly Unchanged

Memory allocation and string functions are already portable.

### 6. linker.ld → Complete Rewrite

```ld
/* ARM Linker Script */
OUTPUT_FORMAT("elf32-arm")
ENTRY(_start)

phys = 0x40000000;  @ Adjust per target

SECTIONS
{
    . = phys;
    
    .text.boot : { *(.text.boot) }
    .text : { *(.text*) }
    .rodata : { *(.rodata*) }
    .data : { *(.data*) }
    .bss : { *(.bss*) }
    
    _end = .;
}
```

### 7. build.sh → Heavy Modification

```bash
#!/bin/bash

# Build for ARM
ARCH=arm
PREFIX=arm-none-eabi-

# OR build for x86 (keep existing)
# ARCH=x86
# PREFIX=

if [ "$ARCH" = "arm" ]; then
    echo "Building for ARM (ARMv7-A)..."
    
    # ARM assembler flags
    ASFLAGS="-march=armv7-a -mfpu=vfpv3"
    
    # ARM C flags
    CFLAGS="-march=armv7-a -mfpu=vfpv3 -ffreestanding -fno-pie \
            -O2 -Wall -nostdinc"
    
    # ARM linker flags
    LDFLAGS="-T linker.ld -Bstatic"
    LDMACHINE="-m armelf_linux_eabi"
    
    $PREFIXas $ASFLAGS src/boot/arm_entry.s -o boot.o
    $PREFIXgcc $CFLAGS -c src/kernel/kernel.c -o kernel.o
    # ... compile other sources ...
    
    $PREFIXld $LDMACHINE $LDFLAGS -o flux-kernel boot.o kernel.o ...
    
else
    # Existing x86 build code
    ...
fi
```

### 8. isodir/boot/grub/grub.cfg → Remove

GRUB is x86-only. For ARM, the bootloader is handled by:
- U-Boot
- Raspberry Pi bootcode
- QEMU built-in boot

**Create new boot script for ARM:**
```bash
# boot_emu_arm.sh - For QEMU
qemu-system-arm \
    -M virt \
    -cpu cortex-a15 \
    -m 1G \
    -kernel flux-kernel.bin \
    -dtb board.dtb \
    -serial stdio \
    -display sdl \
    -device VGA,driver=stdvga
```

---

## Testing Strategy

### Development Testing

| Phase | Testing Method |
|-------|----------------|
| Phase 1 | QEMU ARM virt machine, UART output only |
| Phase 2 | QEMU with serial console debugging |
| Phase 3 | QEMU with framebuffer (stdvga or virtio-gpu) |
| Phase 4 | USB keyboard/mouse testing |
| Phase 5 | Full GUI testing |

### QEMU Commands

For testing Pi 500/4 (AArch64):
```bash
# Using aarch64-softmmu (recommended for Pi 500/4)
qemu-system-aarch64 -M virt -cpu cortex-a72 -m 1G \
    -kernel build/flux-kernel.bin \
    -serial stdio \
    -display sdl

# With Device Tree
qemu-system-aarch64 -M virt -cpu cortex-a72 -m 1G \
    -kernel build/flux-kernel.bin \
    -dtb board.dtb \
    -serial stdio \
    -display sdl

# Pi 3 emulation (32-bit)
qemu-system-arm -M raspi3 -kernel kernel8.img \
    -serial stdio \
    -display sdl
```

### Hardware Testing

| Device | Boot Method |
|--------|-------------|
| **Raspberry Pi 500** | Copy kernel8.img to SD card, boot from micro-SD |
| Raspberry Pi 4 | Copy kernel8.img to SD card |
| ARM Laptops | UEFI boot, EFI application |
| QEMU | Direct kernel boot |

### Raspberry Pi 500 SD Card Setup

For testing on Raspberry Pi 500:

1. **Prepare micro-SD card** (minimum 8GB)
2. **Copy Raspberry Pi boot files** to SD card:
   ```bash
   # Download Raspberry Pi OS boot files
   # or copy from existing Raspberry Pi OS installation
   ```
3. **Configure config.txt:**
   ```
   arm_64bit=1
   kernel=kernel8.img
   device_tree_address=0x1f0000
   gpu_mem=128
   enable_uart=1
   dtoverlay=disable-bt
   dtoverlay=disable-wifi
   ```
4. **Copy Flux-OS kernel:**
   ```bash
   cp build/flux-kernel.elf kernel8.img
   ```
5. **Insert SD card and power on Pi 500**
6. **Connect serial console** (UART pins) for debugging:
   ```
   TX  (GPIO 14) → UART TX
   RX  (GPIO 15) → UART RX
   GND → GND
   ```
   ```bash
   # Connect with picocom or minicom
   sudo apt-get install picocom
   sudo picocom -b 115200 /dev/ttyUSB0
   ```

### Pi 500 Specific Considerations

| Component | Pi 500 Implementation |
|-----------|----------------------|
| UART | PL011 on `/dev/ttyAMA0` (GPIO 14/15) |
| Framebuffer | VideoCore VI, Device Tree provides address |
| USB | XHCI host controller (USB 3.0) |
| Power | 5V USB-C, recommend 3A+ adapter |
| Video Output | 2x micro-HDMI |
| Keyboard | Integrated USB keyboard (acts as USB HID) |
| Trackpad | Integrated USB trackpad (acts as USB HID) |

### ARM Laptop Considerations

| Component | Laptop Implementation |
|-----------|----------------------|
| Boot | UEFI (standard) |
| Framebuffer | GOP (Graphics Output Protocol) |
| Input | USB HID (built-in keyboard/touchpad) |
| ACPI | Full ACPI tables available |

---

## Risks and Mitigation

| Risk | Impact | Probability | Mitigation |
|------|--------|-------------|------------|
| ARM hardware variability | High | High | Use QEMU virt as reference, support Device Tree |
| Performance regression | Medium | Low | ARMv7 is comparable to x86 for this workload |
| Driver complexity | High | Medium | Start with QEMU virt (emulated devices) |
| Build complexity | Low | Low | Use separate build scripts per architecture |
| Testing difficulty | Medium | High | Heavy use of QEMU ARM emulation |

### Key Challenges

1. **No BIOS/UEFI on ARM** - Must use bootloader-provided services
2. **Device heterogeneity** - Each ARM board is different
3. **Boot protocols** - Multiple standards (U-Boot, UEFI, vendor-specific)
4. **USB complexity** - Requires USB host stack for modern input devices

---

## Estimated Effort

| Phase | Effort | Difficulty |
|-------|--------|------------|
| Phase 1: Foundation | 2-3 weeks | Medium |
| Phase 2: Core Kernel | 2-3 weeks | High |
| Phase 3: Graphics | 2-3 weeks | Medium |
| Phase 4: Input | 1-2 weeks | High |
| Phase 5: Build/Testing | 1 week | Low |
| **Total** | **8-14 weeks** | **Medium-High** |

### Factors Affecting Timeline

- **Prior ARM experience:** -2 to -4 weeks if experienced
- **Target hardware:** +1 week for Raspberry Pi support
- **USB stack:** +2 weeks if full USB support needed
- **Contributors:** Scales roughly linearly

---

## Rollback Plan

If ARM migration proves too challenging:

1. **Maintain x86 as primary architecture**
2. **Add ARM support as secondary (experimental)**
3. **Use architecture detection at build time**
4. **Keep code paths relatively common**

```bash
# Conditional build
if [ "$1" = "--arm" ]; then
    build_arm
else
    build_x86
fi
```

---

## References

### ARM Documentation

- [ARM Architecture Reference Manual (ARMv7-A)](https://developer.arm.com/documentation/ddi0406/latest/)
- [ARM System Developer's Guide](https://developer.arm.com/documentation/ddi0200/latest/)
- [ARM Procedure Call Standard (AAPCS)](https://developer.arm.com/documentation/den0013/latest/)

### QEMU ARM Documentation

- [QEMU System ARM](https://www.qemu.org/docs/master/system/arm/index.html)
- [QEMU virt machine](https://www.qemu.org/docs/master/system/arm/virt.html)

### Device Tree

- [Device Tree Specification](https://www.devicetree.org/specifications/)
- [DeviceTree for ARM Linux](https://www.kernel.org/doc/Documentation/devicetree/bindings/)

### Bootloaders

- [U-Boot for ARM](https://u-boot.readthedocs.io/en/latest/board/arm/)
- [Limine Bootloader](https://github.com/limine-bootloader/limine)

### Useful Tools

- `arm-none-eabi-gdb` - ARM debugger
- `arm-linux-gnueabihf-objdump` - Disassembler
- `dtc` - Device Tree compiler
- `qemu-system-arm` - ARM emulator

---

## Conclusion

Migrating Flux-OS from x86 to ARM is a significant undertaking requiring:

1. **Complete rewrite of boot code** (no GRUB/Multiboot)
2. **Replacement of all hardware access patterns** (port I/O → memory-mapped)
3. **New interrupt and timer systems**
4. **New bootloader integration**
5. **Updated build toolchain**

However, the core kernel logic and GUI system are already largely portable. The key challenges are at the hardware abstraction layer, not in the OS functionality itself.

The migration is recommended for teams with ARM experience or those willing to invest 2-3 months of development effort.

---

*Document Version: 1.0*
*Created for Flux-OS Project*

