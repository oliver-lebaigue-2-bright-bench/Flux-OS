# FLUX-OS System Architecture

## 👨‍💻 Developer

**Oliver Lebaigue** - Sole Developer  
*Age: 16 | Passionate about minimal OS design and retro computing*

---

## 🏗️ Architecture Overview

Flux OS is a 32-bit, monolithic kernel operating system designed from scratch for x86 architecture. It follows a minimalist philosophy inspired by Nothing OS, with a typography-driven user interface.

```
┌─────────────────────────────────────────────────────────────────┐
│                         USER SPACE                              │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                  FLUXWM (Window Manager)                 │   │
│  │  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐       │   │
│  │  │ Terminal│ │  Files  │ │ Settings│ │  Apps   │       │   │
│  │  └─────────┘ └─────────┘ └─────────┘ └─────────┘       │   │
│  └─────────────────────────────────────────────────────────┘   │
│                    GUI Library (gfx.c)                         │
├─────────────────────────────────────────────────────────────────┤
│                        KERNEL SPACE                             │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                    SYSTEM CALLS                        │   │
│  │  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐       │   │
│  │  │ Process │ │ Memory  │ │   File  │ │  Graph  │       │   │
│  │  │ Manager │ │ Manager │ │ Systems │ │ Manager │       │   │
│  │  └─────────┘ └─────────┘ └─────────┘ └─────────┘       │   │
│  └─────────────────────────────────────────────────────────┘   │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                      KERNEL CORE                         │   │
│  │  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐       │   │
│  │  │  Scheduler│ │  IDT    │ │   GDT   │ │   PIT   │       │   │
│  │  └─────────┘ └─────────┘ └─────────┘ └─────────┘       │   │
│  └─────────────────────────────────────────────────────────┘   │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                    DRIVER LAYER                          │   │
│  │  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐       │   │
│  │  │ Keyboard│ │  Mouse  │ │  VBE    │ │   PIT   │       │   │
│  │  │  Driver │ │  Driver │ │  Driver │ │   RTC   │       │   │
│  │  └─────────┘ └─────────┘ └─────────┘ └─────────┘       │   │
│  └─────────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────────┤
│                      HARDWARE ABSTRACTION                        │
│              (x86 Assembly, Multiboot, BIOS/UEFI)              │
├─────────────────────────────────────────────────────────────────┤
│                         HARDWARE                                 │
│              (CPU, RAM, Video, Storage, I/O)                   │
└─────────────────────────────────────────────────────────────────┘
```

---

## 💾 Memory Layout

### Physical Memory Map

```
┌─────────────────────────┬──────────────────────────┐
│ 0x00000000 - 0x0009FFFF │ Lower 640KB (Usable RAM)  │
├─────────────────────────┼──────────────────────────┤
│ 0x000A0000 - 0x000BFFFF │ VGA Video Memory          │
├─────────────────────────┼──────────────────────────┤
│ 0x000C0000 - 0x000FFFFF │ BIOS & Reserved           │
├─────────────────────────┼──────────────────────────┤
│ 0x00100000 - 0x00FFFFFF │ Usable RAM (15MB)         │
├─────────────────────────┼──────────────────────────┤
│ 0x01000000+             │ Kernel & Modules          │
├─────────────────────────┼──────────────────────────┤
│ 0x100000 - 0x3FFFFFF    │ Kernel Image (15MB)        │
└─────────────────────────┴──────────────────────────┘
```

### Virtual Memory Layout (Protected Mode)

```
┌─────────────────────────┬──────────────────────────┐
│ 0xFFFFFFFF - 0xC0000000 │ Kernel Space (High 1GB)  │
├─────────────────────────┼──────────────────────────┤
│ 0xBFFFFFFF - 0xC0000000 │ Kernel Code              │
├─────────────────────────┼──────────────────────────┤
│ 0xBFFFFFF0 - 0xBFFFFFFF │ Kernel Data              │
├─────────────────────────┼──────────────────────────┤
│ 0x40000000 - 0xBFFFFFFF │ User Space (High 3GB)     │
├─────────────────────────┼──────────────────────────┤
│ 0x00000000 - 0x3FFFFFFF │ User Space (Low 1GB)     │
└─────────────────────────┴──────────────────────────┘
```

### Linker Script Memory Map

```ld
/* linker.ld */
OUTPUT_FORMAT("elf32-i386")
ENTRY(start)

SECTIONS
{
    . = 0x100000;          /* Kernel load address */
    
    .text : {
        *(.text*)
    } = 0x90
    
    .rodata : {
        *(.rodata*)
    }
    
    .data : {
        *(.data*)
    }
    
    .bss : {
        *(COMMON)
        *(.bss*)
    }
}
```

---

## 🔄 Boot Process

### Stage 1: BIOS/UEFI

```
1. Power On → POST (Power-On Self-Test)
2. BIOS loads MBR (Master Boot Record) from boot device
3. BIOS transfers control to bootloader
```

### Stage 2: Bootloader (boot.s)

```assembly
; src/boot/boot.s

[BITS 16]
[ORG 0x7C00]

start:
    ; Load kernel from disk using BIOS INT 0x13
    ; Switch to Protected Mode (32-bit)
    ; Set up GDT (Global Descriptor Table)
    ; Jump to kernel entry point
```

**Bootloader Responsibilities**:
- Load kernel sectors from disk to memory
- Enable A20 line (access to high memory)
- Set up GDT for protected mode
- Enable protected mode
- Parse Multiboot header
- Jump to kernel entry (0x100000)

### Stage 3: Kernel Entry (kernel.c)

```c
/* src/kernel/kernel.c */

void kernel_main(multiboot_info_t* mb_info, uint32_t magic) {
    // 1. Verify Multiboot magic
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        // Error: Not loaded by Multiboot-compliant bootloader
        return;
    }
    
    // 2. Parse memory map
    parse_memory_map(mb_info);
    
    // 3. Initialize VBE (if available)
    init_vbe(mb_info);
    
    // 4. Set up graphics mode
    set_graphics_mode();
    
    // 5. Initialize GUI
    gui_init();
    
    // 6. Start scheduler
    scheduler_start();
}
```

### Stage 4: GUI Initialization

```
gui_init()
├── gui_create_desktop()
│   ├── gui_load_fonts()
│   ├── gui_create_taskbar()
│   ├── gui_create_desktop_icons()
│   └── gui_create_widgets()
├── gui_load_wallpaper()
└── gui_start_event_loop()
    └── event_poll()
        ├── handle_keyboard()
        ├── handle_mouse()
        ├── update_glyphs()
        └── render_frame()
```

---

## 🧩 Kernel Components

### Process Manager

```c
// Process control block
typedef struct {
    uint32_t pid;              // Process ID
    uint32_t state;             // Running/Blocked/Ready
    uint32_t entry_point;       // Program entry address
    uint32_t stack_ptr;         // Stack pointer
    uint32_t stack_size;        // Stack size
    page_directory_t* mm;        // Memory management
    struct list_head siblings;  // Process list
} process_t;
```

### Memory Manager

```c
// Page directory entry
typedef struct {
    uint32_t present    : 1;    // Page present in memory
    uint32_t rw         : 1;    // Read/Write
    uint32_t user       : 1;    // User/Kernel mode
    uint32_t accessed   : 1;    // Accessed flag
    uint32_t dirty      : 1;    // Dirty flag
    uint32_t pat        : 1;    // Page attribute table
    uint32_t global     : 1;    // Global page
    uint32_t available  : 3;    // Available for OS use
    uint32_t frame      : 20;   // Physical frame address
} page_entry_t;
```

### File System

```
Flux FS (Designed for embedded/boot use)
├── Superblock (1024 bytes)
├── Inode Table
├── Data Blocks
└── Directory Entries

or

FAT32 (For boot compatibility)
└── Standard FAT32 structure
```

### Graphics System

```c
// Framebuffer structure
typedef struct {
    uint32_t* address;         // Framebuffer base address
    uint32_t width;             // Screen width in pixels
    uint32_t height;            // Screen height in pixels
    uint32_t pitch;             // Bytes per row
    uint32_t bpp;               // Bits per pixel
} framebuffer_t;

// Graphics primitives
void gfx_draw_pixel(int x, int y, color_t color);
void gfx_draw_line(int x1, int y1, int x2, int y2, color_t color);
void gfx_draw_rect(int x, int y, int w, int h, color_t color);
void gfx_fill_rect(int x, int y, int w, int h, color_t color);
void gfx_draw_text(int x, int y, const char* text, font_t* font);
void gfx_draw_circle(int x, int y, int r, color_t color);
```

### Typography Engine

```c
// Font character
typedef struct {
    uint8_t width;              // Character width
    uint8_t height;             // Character height
    uint8_t data[];            // Bitmap data
} font_char_t;

// Font definition
typedef struct {
    const char* name;           // Font family name
    uint8_t size;               // Font size in pixels
    uint8_t baseline;           // Baseline offset
    uint16_t num_chars;        // Number of characters
    font_char_t chars[];       // Character data
} font_t;
```

### Glyph Interface

```c
// Glyph types
typedef enum {
    GLYPH_NOTIFY,              // Notification glyph
    GLYPH_PROGRESS,            // Progress ring
    GLYPH_VOLUME,              // Volume indicator
    GLYPH_PULSE,               // System pulse
    GLYPH_BATTERY,             // Battery indicator
    GLYPH_NETWORK              // Network status
} glyph_type_t;

// Glyph structure
typedef struct {
    glyph_type_t type;          // Glyph type
    int x, y;                   // Position
    uint32_t color;             // Primary color
    uint32_t anim_state;        // Animation state
    void (*render)(struct glyph* g);
} glyph_t;
```

---

## 🔧 System Calls

### Available System Calls

| ID | Name | Parameters | Description |
|----|------|------------|-------------|
| 0x01 | `write` | fd, buffer, count | Write to file/terminal |
| 0x02 | `read` | fd, buffer, count | Read from file/device |
| 0x03 | `open` | path, flags | Open file |
| 0x04 | `close` | fd | Close file descriptor |
| 0x05 | `fork` | - | Create child process |
| 0x06 | `execve` | path, argv, envp | Execute program |
| 0x07 | `waitpid` | pid, status, options | Wait for process |
| 0x08 | `exit` | status | Terminate process |
| 0x09 | `mmap` | addr, length, prot, flags | Map memory |
| 0x0A | `munmap` | addr, length | Unmap memory |
| 0x0B | `brk` | addr | Set program break |
| 0x0C | `ioctl` | fd, request, argp | Device I/O control |
| 0x0D | `gettimeofday` | tv, tz | Get current time |
| 0x0E | `draw_pixel` | x, y, color | Draw pixel to framebuffer |
| 0x0F | `draw_text` | x, y, text, font | Draw text string |
| 0x10 | `gui_event` | event_type, data | Send GUI event |

---

## 📦 Interrupt Handling

### Interrupt Descriptor Table (IDT)

```
┌─────────────────────────────────────────┐
│ IDT Entry 0x00  │  Division by Zero    │
├─────────────────────────────────────────┤
│ IDT Entry 0x01  │  Debug Exception     │
├─────────────────────────────────────────┤
│ IDT Entry 0x02  │  NMI Interrupt       │
├─────────────────────────────────────────┤
│ IDT Entry 0x03  │  Breakpoint           │
├─────────────────────────────────────────┤
│ IDT Entry 0x04  │  Overflow             │
├─────────────────────────────────────────┤
│ IDT Entry 0x05  │  Bound Range Exceeded │
├─────────────────────────────────────────┤
│ IDT Entry 0x06  │  Invalid Opcode       │
├─────────────────────────────────────────┤
│ IDT Entry 0x07  │  Device Not Available │
├─────────────────────────────────────────┤
│ IDT Entry 0x08  │  Double Fault         │
├─────────────────────────────────────────┤
│ IDT Entry 0x09  │  Coprocessor Segment  │
├─────────────────────────────────────────┤
│ IDT Entry 0x0A  │  Invalid TSS          │
├─────────────────────────────────────────┤
│ IDT Entry 0x0B  │  Segment Not Present  │
├─────────────────────────────────────────┤
│ IDT Entry 0x0C  │  Stack Fault          │
├─────────────────────────────────────────┤
│ IDT Entry 0x0D  │  General Protection   │
├─────────────────────────────────────────┤
│ IDT Entry 0x0E  │  Page Fault           │
├─────────────────────────────────────────┤
│ IDT Entry 0x0F  │  Reserved             │
├─────────────────────────────────────────┤
│ IDT Entry 0x10  │  x87 FPU Error        │
├─────────────────────────────────────────┤
│ ...              │  ...                  │
├─────────────────────────────────────────┤
│ IDT Entry 0x80  │  System Call (0x80)   │
├─────────────────────────────────────────┤
│ ...              │  ...                  │
└─────────────────────────────────────────┘
```

### IRQ Mapping

| IRQ | Hardware | Handler |
|-----|----------|---------|
| 0 | Programmable Interval Timer (PIT) | `timer_irq()` |
| 1 | Keyboard Controller | `keyboard_irq()` |
| 2 | Cascade (IRQ 8-15) | - |
| 8 | Real-Time Clock (RTC) | `rtc_irq()` |
| 12 | PS/2 Mouse | `mouse_irq()` |
| 13 | FPU/Math Coprocessor | `fpu_irq()` |
| 14 | Primary IDE Channel | `ide_irq()` |
| 15 | Secondary IDE Channel | `ide_irq()` |

---

## 🎨 User Interface Architecture

### Desktop Composition

```
┌─────────────────────────────────────────────────────────┐
│ FluxWM Window Manager                                   │
├─────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────┐   │
│  │  ┌─────────────────────────────────────────┐    │   │
│  │  │          Desktop Surface                │    │   │
│  │  │  ┌─────────┐ ┌─────────┐ ┌─────────┐  │    │   │
│  │  │  │ Icon A   │ │ Icon B   │ │ Icon C  │  │    │   │
│  │  │  └─────────┘ └─────────┘ └─────────┘  │    │   │
│  │  │                                         │    │   │
│  │  │  ┌─────────────────────────────────┐   │    │   │
│  │  │  │        Window (App)             │   │    │   │
│  │  │  ├─────────────────────────────────┤   │    │   │
│  │  │  │                                 │   │    │   │
│  │  │  │         App Content             │   │    │   │
│  │  │  │                                 │   │    │   │
│  │  │  └─────────────────────────────────┘   │    │   │
│  │  └─────────────────────────────────────────┘    │   │
│  └─────────────────────────────────────────────────┘   │
│  ┌─────────────────────────────────────────────────┐   │
│  │  Taskbar    [Menu]  [Time]    [Glyph]  [Volume] │   │
│  └─────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────┘
```

### Window Manager State

```c
typedef struct {
    list_head_t windows;        // All windows
    window_t* active;            // Currently active window
    window_t* focused;           // Keyboard-focused window
    surface_t* desktop;          // Desktop surface
    surface_t* taskbar;         // Taskbar surface
    glyph_t* glyph_indicators;   // Glyph system indicators
} fluxwm_state_t;
```

### Event Loop

```c
void event_loop() {
    while (running) {
        // Poll for events
        event_t event = event_poll();
        
        switch (event.type) {
            case EVENT_KEYBOARD:
                handle_keyboard(event.key);
                break;
            case EVENT_MOUSE_MOVE:
                handle_mouse_move(event.x, event.y);
                break;
            case EVENT_MOUSE_BUTTON:
                handle_mouse_button(event.button, event.state);
                break;
            case EVENT_TIMER:
                handle_timer(event.timer_id);
                break;
            case EVENT_REDRAW:
                gui_redraw_all();
                break;
        }
        
        // Update animations
        update_animations();
        
        // Render frame
        gui_render();
    }
}
```

---

## 🔒 Security Model

### Privilege Levels

```
Ring 0 (Kernel Mode)
  │
  ├─ System calls (controlled entry)
  │
Ring 1-2 (Unused)
  │
Ring 3 (User Mode)
  └─ Applications
```

### Memory Protection

- **User/Kernel separation**: 1GB user, 3GB kernel
- **Page-level protection**: Read/Write/Execute flags
- **Supervisor mode only**: Critical operations restricted
- **No direct hardware access**: System calls required

### Future Security Features

- [ ] Process isolation
- [ ] ASLR (Address Space Layout Randomization)
- [ ] DEP (Data Execution Prevention)
- [ ] Secure boot chain
- [ ] User authentication

---

## 📊 Performance Characteristics

### Target Benchmarks

| Metric | Target | Current |
|--------|--------|---------|
| Boot Time | < 3 seconds | ~5 seconds |
| Context Switch | < 10μs | TBD |
| Interrupt Latency | < 50μs | TBD |
| Memory Overhead | < 5% | TBD |
| GUI FPS | 60 fps | TBD |

### Optimization Goals

1. **Zero-copy rendering**: Direct framebuffer access
2. **Batch operations**: Minimize draw calls
3. **Lazy evaluation**: Defer work when possible
4. **Efficient algorithms**: O(n) or better

---

## 🔮 Future Architecture

### Planned Additions

```
Near Term:
├── USB Support (OHCI/UHCI)
├── ACPI Power Management
└── Better Memory Management

Medium Term:
├── Virtual File System (VFS)
├── ELF Binary Format Support
├── Basic Shell (FluxSH)
└── Userland Applications

Long Term:
├── ARM Port (ARMv7-A)
├── SMP Support (Multi-core)
├── GPU Acceleration (OpenGL ES)
└── Network Stack (TCP/IP)
```

---

## 📚 References

### Internal Documentation

- [README.md](README.md) - Main project overview
- [VISION.md](VISION.md) - Design philosophy
- [DESIGN.md](UI/UX specifications)
- [TYPOGRAPHY.md](Font system)
- [GLYPH.md](Glyph interface)

### External Resources

- [Intel Software Developer Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
- [Multiboot Specification](https://www.gnu.org/software/grub/manual/multiboot/)
- [OSDev Wiki](https://wiki.osdev.org)
- [ELF Specification](https://refspecs.linuxfoundation.org/elf/gabi4+/)

---

*FLUX-OS System Architecture v1.0*  
*Created by Oliver Lebaigue (Age 16)*  
*Nothing OS Inspired Desktop Experience*

