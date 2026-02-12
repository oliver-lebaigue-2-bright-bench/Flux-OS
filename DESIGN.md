# FLUX-OS Design Guide

## 👨‍💻 Developer

**Oliver Lebaigue** - Sole Developer  
*Age: 16 | Passionate about minimal OS design and retro computing*

---

## 🎨 Design System

### Color Palette

Nothing OS is known for its distinctive monochrome aesthetic. Flux OS extends this to desktop:

| Name | Hex | RGB | Usage |
|------|-----|-----|-------|
| **True Black** | `#000000` | 0, 0, 0 | Backgrounds, deep UI |
| **Pure White** | `#FFFFFF` | 255, 255, 255 | Text, highlights |
| **Glyph Gold** | `#D4AF37` | 212, 175, 55 | Accents, notifications |
| **Warm Grey** | `#A8A8A8` | 168, 168, 168 | Secondary text |
| **Dark Grey** | `#404040` | 64, 64, 64 | Borders, disabled |
| **Soft Grey** | `#C0C0C0` | 192, 192, 192 | Hover states |

### Typography Colors

- **Primary Text**: `#FFFFFF` on `#000000` (inverted)
- **Secondary Text**: `#A8A8A8`
- **Caption Text**: `#808080`
- **Link/Accent**: `#D4AF37`

### Spacing System

Consistent spacing based on typography grid:

| Token | Pixels | Usage |
|-------|--------|-------|
| `space-xs` | 4px | Tight spacing, inline elements |
| `space-sm` | 8px | Icon padding, compact layouts |
| `space-md` | 16px | Standard spacing, paragraph margin |
| `space-lg` | 24px | Section spacing, widget margins |
| `space-xl` | 32px | Major layout gaps |
| `space-2xl` | 48px | Screen margins, dialog padding |

### Border System

| Type | Width | Color | Radius | Usage |
|------|-------|-------|--------|-------|
| **hairline** | 1px | `#404040` | 0px | Thin dividers |
| **thin** | 2px | `#A8A8A8` | 2px | Input borders |
| **medium** | 3px | `#D4AF37` | 4px | Active states |
| **thick** | 4px | `#FFFFFF` | 8px | Major UI elements |

---

## 🧩 Component Library

### Buttons

**Primary Button**
```
Background: #D4AF37 (Glyph Gold)
Text: #000000
Border: None
Radius: 4px
Padding: 12px × 24px
Font: Body (14px, Medium)
```

**Secondary Button**
```
Background: Transparent
Text: #FFFFFF
Border: 2px solid #404040
Radius: 4px
Padding: 10px × 22px
Font: Body (14px, Medium)
```

**Text Button**
```
Background: Transparent
Text: #D4AF37
Border: None
Padding: 8px × 16px
Font: Body (14px, Medium)
```

### Windows

**Window Structure**
```
Title Bar:
  Height: 32px
  Background: #1A1A1A
  Text: #FFFFFF (Window Title)
  Close Button: #D4AF37 glyph
  
Content Area:
  Background: #000000
  Padding: 16px
  
Border:
  Width: 2px
  Color: #404040
  Radius: 8px
```

### Widgets

**Dot-Matrix Clock**
```
Font: Custom Dot Matrix (24px Bold)
Color: #FFFFFF
Background: #000000
Update: Every second
Animation: Subtle pulse on hour change
```

**Glyph Progress Ring**
```
Stroke Width: 4px
Progress Color: #D4AF37
Track Color: #1A1A1A
Animation: Smooth rotation
```

---

## 🎬 Animation System

### Animation Principles

1. **Functional**: Every animation serves a purpose
2. **Subtle**: Never distracting or slow
3. **Responsive**: Feel instant and reactive
4. **Consistent**: Same animation curves throughout

### Animation Tokens

| Name | Duration | Easing | Usage |
|------|----------|--------|-------|
| `anim-instant` | 0ms | Linear | State changes |
| `anim-fast` | 100ms | Ease-out | Small interactions |
| `anim-normal` | 200ms | Ease-out | Standard transitions |
| `anim-slow` | 300ms | Ease-in-out | Major changes |
| `anim-glyph` | 800ms | Ease-in-out | Glyph sequences |

### Animation Examples

**Window Open**
```css
animation: windowOpen 200ms ease-out;

@keyframes windowOpen {
  from {
    opacity: 0;
    transform: scale(0.95) translateY(10px);
  }
  to {
    opacity: 1;
    transform: scale(1) translateY(0);
  }
}
```

**Glyph Pulse**
```css
animation: glyphPulse 1.5s ease-in-out infinite;

@keyframes glyphPulse {
  0%, 100% {
    opacity: 1;
    transform: scale(1);
  }
  50% {
    opacity: 0.8;
    transform: scale(0.98);
  }
}
```

---

## 📐 Layout Principles

### Grid System

- **Base Unit**: 4px grid
- **Columns**: 12-column flexible grid
- **Max Width**: 1920px (desktop standard)
- **Content Width**: 1200px (optimal readability)

### Responsive Breakpoints

| Breakpoint | Width | Target |
|------------|-------|--------|
| `small` | < 1024px | Tablet |
| `medium` | 1024px - 1440px | Laptop |
| `large` | > 1440px | Desktop |

### Layout Hierarchy

1. **Global**: Screen margins, background
2. **Window**: Window positioning, Z-index
3. **Section**: Content groupings
4. **Component**: Individual elements
5. **Element**: Text, icons within components

---

## 🖼️ Iconography

### Style Guidelines

- **Monochrome**: Single color (usually white or Glyph Gold)
- **Line Art**: Thin strokes (2px)
- **Geometric**: Simple shapes, no organic curves
- **Consistent**: Same visual weight throughout

### Icon Sizes

| Size | Pixels | Usage |
|------|--------|-------|
| `icon-sm` | 16px | Inline with text |
| `icon-md` | 24px | Standard icon size |
| `icon-lg` | 32px | Widget icons |
| `icon-xl` | 48px | Feature icons |

---

## ♿ Accessibility

### Contrast Requirements

- **AA Standard**: 4.5:1 minimum for body text
- **AAA Standard**: 7:1 for important information
- **Large Text**: 3:1 minimum (18px+ or 14px+ bold)

### Focus States

- **Focus Ring**: 2px solid #D4AF37
- **Offset**: 2px from element edge
- **Animation**: 100ms ease-out

### Keyboard Navigation

- **Tab**: Forward navigation
- **Shift+Tab**: Backward navigation
- **Enter/Space**: Activate
- **Arrow Keys**: Within components

---

## 📱 Mobile Considerations

While primarily desktop-focused:

- **Touch Targets**: Minimum 44px × 44px
- **Swipe Gestures**: Supported where natural
- **Responsive Layout**: Adapts to smaller screens

---

*FLUX-OS Design Guide v1.0*  
*Created by Oliver Lebaigue (Age 16)*  
*Nothing OS Inspired Desktop Experience*

