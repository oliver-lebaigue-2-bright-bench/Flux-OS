# Flux-OS Design Language

## Philosophy: Nothing (R) Inspired Minimalism

Flux-OS embraces the design philosophy of Nothing (R), removing unnecessary complexity and bringing clarity through minimalist interfaces and invisible technology.

## Core Design Principles

### 1. Minimalism Through Reduction

Every element serves a purpose. Unnecessary decoration is removed. Clean lines, generous whitespace, and essential elements only.

**Example**: A window close button is a simple circle outline, no additional styling.

### 2. Invisible Technology

The OS should work seamlessly without demanding attention. When everything works perfectly, the technology becomes invisible.

**Example**: Smooth transitions that feel natural, not flashy notifications.

### 3. Glyph-Based Iconography

A signature glyph system for intuitive navigation. Consistent, minimal icons that communicate instantly.

**Example**: A folder is a simple house shape, a file is a rectangle with one corner folded.

### 4. Clarity Through Absence

Don't show unnecessary information. Let content breathe. Use whitespace as a design element.

**Example**: System dialogs show only essential options, rest are hidden in menus.

### 5. Light & Shadow

Elegant use of negative space and typography. Subtle shadows create depth without heaviness.

**Example**: A slight 8px drop shadow under floating windows, never harsh.

## Color Palette

| Role | Color | Usage |
|------|-------|-------|
| **Primary** | `#000000` Black | Text, primary elements |
| **Surface** | `#FFFFFF` White | Backgrounds, panels |
| **Secondary** | `#F5F5F5` Light Gray | Subtle backgrounds |
| **Tertiary** | `#808080` Medium Gray | Secondary text |
| **Accent** | `#999999` Gray | Subtle accents |
| **Disabled** | `#CCCCCC` Light Gray | Disabled states |

## Typography

- **Font Family**: Inter (or system -apple-system, BlinkMacSystemFont, Segoe UI)
- **Body Text**: 14px, line-height 1.6
- **Headings**: 18px, regular weight
- **Small Text**: 12px for secondary information
- **Monospace**: JetBrains Mono for code

## Spacing

Base unit is **8px**:
- Small spacing: 4px
- Medium spacing: 8px
- Large spacing: 16px
- Extra large: 24px

## Animation

- **Duration**: 200ms for most transitions
- **Easing**: `cubic-bezier(0.4, 0, 0.2, 1)` (Material Design standard)
- **Philosophy**: Animations aid navigation, never distract
- **Accessibility**: Always respect `prefers-reduced-motion`

## Components

### Buttons

```
Default:  #f5f5f5 background, #000000 text
Hover:    #e8e8e8 background
Active:   #d0d0d0 background
Disabled: #cccccc text on #f5f5f5
```

### Windows

- **Background**: #ffffff
- **Border**: 1px #f0f0f0
- **Shadow**: `rgba(0, 0, 0, 0.08)` at 8px blur
- **Radius**: 4px (subtle)

### Text Input

- **Background**: #ffffff
- **Border**: 1px #e0e0e0
- **Focus Border**: 2px #000000
- **Cursor**: #000000

## Desktop Environment

### Taskbar

- Minimal, clean
- Light gray (#f5f5f5) background
- Glyph icons only, no labels
- Hover effects are subtle (slight background shade)

### File Manager

- List view by default
- Generous padding between items
- Glyph folder/file icons
- Single-click activation with hover state

### System Dialogs

- Centered on screen
- Essential options only
- Confirm button is primary action (subtle styling)
- Cancel button is secondary (lower emphasis)

## Orbital Window Manager

The Orbital display server will be themed with Nothing's minimalist aesthetic:

- Window chrome: minimal title bar with close/minimize/maximize glyphs
- Focus indicator: subtle 1px accent border
- Alt-Tab switcher: clean grid of window previews
- Workspace switcher: number glyphs (1, 2, 3...)

## Accessibility

- **Contrast Ratio**: All text meets WCAG AA (4.5:1)
- **Focus Indicators**: Always visible, 2px border
- **Motion**: Respect `prefers-reduced-motion` setting
- **Sizing**: Touch targets minimum 44x44px

## Implementation Notes

This design language applies to:
1. System UI (window manager, taskbar, dialogs)
2. Built-in applications (file manager, terminal, text editor)
3. Icon system (glyph pack)
4. Default theme
5. Documentation and branding

External applications may use their own themes but should support light/dark variants.
