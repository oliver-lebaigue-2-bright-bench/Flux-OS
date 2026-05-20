# Flux-OS Glyph System

## Design Language

The Flux-OS glyph system is inspired by Nothing (R)'s minimalist aesthetic.

### Core Principles

- **Simplicity**: Each glyph reduces to its essential form
- **Consistency**: Uniform stroke width, grid alignment
- **Clarity**: Legible at all sizes from 16px to 64px+
- **Invisibility**: Technology that fades into the background

### Grid System

- **Base Grid**: 24x24px (1px grid)
- **Stroke Width**: 1.5px for optimal clarity
- **Padding**: 2px from edge at smallest size
- **Alignment**: Center-aligned on 12x12 grid

### Glyph Categories

1. **System** - OS controls, settings, power
2. **Navigation** - Arrows, back, forward, home
3. **Document** - Files, folders, text, media
4. **Communication** - Chat, email, notifications
5. **Creation** - Edit, pencil, palette, brush
6. **Utilities** - Settings, info, warning, check

## Color Usage

- **Default**: #000000 (black)
- **Disabled**: #cccccc (light gray)
- **Accent**: #999999 (medium gray)
- **Background variants**: Use with opacity

## Implementation

Glyphs are SVG-based for perfect scaling. Each glyph includes:
- 16x16, 24x24, 32x32, and 48x48 pixel versions
- Outlined and filled variants
- Dark and light theme support
