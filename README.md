<p align="center">
  <img alt="Flux-OS" width="200" src="https://via.placeholder.com/200?text=flux+glyph">
</p>

# Flux-OS

**The Nothing-inspired desktop operating system. Pure. Minimalist. Invisible.**

Flux-OS is a clean, elegant desktop operating system built on the design language of Nothing (R). Like Nothing's philosophy, Flux-OS removes unnecessary complexity and brings clarity through minimalist interfaces and invisible technology.

Built on [Redox](https://www.redox-os.org) — a modern microkernel OS written in Rust — Flux-OS applies Nothing's distinctive design principles to create a desktop experience focused on simplicity and refinement.

---

## Design Philosophy

Flux-OS embraces Nothing (R)'s core design language:

- **Minimalism**: Remove visual clutter. Simplify to the essential.
- **Invisible Technology**: Technology that works seamlessly without demanding attention.
- **Glyph-Based UI**: Signature glyph aesthetic for intuitive, light navigation.
- **Clarity Through Reduction**: Every pixel serves a purpose.
- **Light & Shadow**: Elegant use of negative space and typography.

---

## Key Features

- **Minimalist Desktop**: A clean, clutter-free interface inspired by Nothing's glyph design language
- **Elegant Window Manager**: Smooth animations and invisible system interactions
- **Rust-based Foundation**: Safe, efficient, and performant core systems
- **Microkernel Architecture**: Modular, secure, and maintainable OS design
- **Modern Terminal**: Fast, beautiful command-line interface
- **Glyph Icon System**: Consistent, minimalist application glyphs

---

## Getting Started

### System Requirements
- x86_64 processor with 2GB+ RAM
- 10GB+ storage for full installation
- UEFI-capable firmware recommended

### Quick Start

#### Building from Source

```bash
# Clone the repository
git clone https://github.com/oliver-lebaigue-2-bright-bench/Flux-OS.git
cd Flux-OS

# Build (requires Rust, requires podman or Docker)
make all

# Run in QEMU
make qemu
```

#### Using Pre-built Image

Download the latest release from [Releases](https://github.com/oliver-lebaigue-2-bright-bench/Flux-OS/releases) and write to USB:

```bash
dd if=flux-os-latest.iso of=/dev/sdX bs=4M && sync
```

#### Virtual Machine

For VirtualBox, VMware, or KVM — see [Build Instructions](./BUILDING.md)

---

## Architecture Overview

Flux-OS is built on proven, modern foundations:

| Component | Details | Status |
|-----------|---------|--------|
| **Kernel** | Microkernel architecture in Rust | Production |
| **Filesystem** | RedoxFS (native) + POSIX support | Production |
| **Shell** | Ion - modern shell | Production |
| **Display Server** | Orbital - elegant window manager | Production |
| **Package Manager** | pkgutils | Beta |
| **Standard Library** | relibc (POSIX in Rust) | Production |

---

## Repository Structure

```
flux-os/
├── kernel/                # Core microkernel
├── base/                  # Essential system components
├── filesystem/            # RedoxFS implementation
├── display/               # Orbital display server & window manager
├── applications/          # Core desktop applications
├── recipes/               # Build recipes & package definitions
├── toolchain/             # Rust toolchain & build utilities
└── docs/                  # Documentation & design guidelines
```

---

## Community & Contribution

Flux-OS is built on the excellent work of [Redox OS](https://www.redox-os.org). We welcome contributions!

- **Design Contributions**: Help refine the Flux-OS design language
- **Code Contributions**: Improve performance, security, and features
- **Documentation**: Help us explain Flux-OS clearly
- **Testing**: Find bugs, report issues, suggest improvements

See [CONTRIBUTING.md](./CONTRIBUTING.md) for detailed guidelines.

### Getting Help

- [Community Chat](https://matrix.to/#/#flux-os:matrix.org)
- [Documentation](./docs/)
- [Build Guide](./BUILDING.md)
- [FAQ](./docs/FAQ.md)

---

## Design Resources

Flux-OS uses Nothing (R)'s design language:

- **Color Palette**: Minimal black, white, and grays
- **Typography**: Clean, modern sans-serif
- **Iconography**: Glyph-based system icons
- **Spacing**: Refined negative space
- **Motion**: Subtle, purposeful animations

See [DESIGN.md](./docs/DESIGN.md) for the complete design system specification.

---

## Technology Stack

- **Language**: Rust (54.5%) - safe, performant systems programming
- **Shell Scripts**: Shell (19%) - automation and scripting
- **Build System**: Makefile (12.4%) - reliable build orchestration
- **System Libraries**: C (7.9%) - POSIX compatibility
- **UI**: HTML/CSS (4.2%) - modern web-based applications

---

## Performance

Flux-OS is optimized for modern hardware:

- **Boot Time**: ~5 seconds to desktop
- **Memory Usage**: ~200MB base system
- **Responsiveness**: Minimal system overhead
- **Efficiency**: Low CPU usage at idle

---

## License

Flux-OS is licensed under the MIT License — see [LICENSE](./LICENSE) for details.

Based on [Redox OS](https://www.redox-os.org) (MIT Licensed)

---

## Inspiration

- [Nothing (R)](https://nothing.tech) - Design philosophy & aesthetic
- [Redox OS](https://www.redox-os.org) - Technical foundation
- Minimalist design traditions - Dieter Rams, Japanese design principles

---

<p align="center">
  <strong>Flux-OS: Technology, refined.</strong>
  <br/>
  <sub>Built on Redox. Designed with Nothing.</sub>
</p>
