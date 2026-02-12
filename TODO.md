# FLUX-OS Development Todo List

## 📋 Current Status: Version 2.6 Pre-Alpha

---

## 🎯 Active Tasks

### Phase 3: Graphics System (In Progress)
- [ ] VESA BIOS Extensions (VBE) Driver
- [ ] Framebuffer Memory Mapping
- [ ] Software Rendering Library (GFX)
  - [ ] Font rendering system (Nothing OS typography)
  - [ ] Dot-matrix display primitives
  - [ ] Glyph interface elements

### Phase 4: User Interface (Planned)
- [ ] Desktop Environment (FluxWM)
  - [ ] Nothing OS-inspired monochrome theme
  - [ ] Glyph-based system indicators
  - [ ] Dot-matrix clock and widgets
  - [ ] Typography-driven window decorations
  - [ ] Minimalist taskbar
- [ ] Window Management
- [ ] System Tray

---

## 🔧 Bug Fixes (Priority)

### Critical
- [ ] VBE Initialization Failure (NULL vbe_mode_info)
- [ ] Framebuffer address retrieval from GRUB

### Minor
- [ ] Memory Layout (1MB load address assumption)
- [ ] IDT implementation for interrupts

---

## 📝 Documentation

- [x] README.md updated
- [x] VISION.md created (Nothing OS desktop vision)
- [ ] GUI component documentation

---

## 🛠️ Build & Testing

- [x] Basic build system working
- [ ] VBE graphics mode display
- [ ] GUI rendering verification

---

## 📦 Dependencies

- GCC i386 cross-compiler
- GRUB 2 bootloader
- QEMU emulator
- X11 display server

---

*Last Updated: Version 2.6 Pre-Alpha*
*Status: Active Development - Testers Needed!*

