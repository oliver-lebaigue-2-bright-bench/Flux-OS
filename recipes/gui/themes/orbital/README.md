# Orbital Desktop Environment Theme

## Flux-OS Desktop Components

This theme applies Nothing (R)'s design language to the Orbital window manager and desktop environment.

## Components

### Window Manager

#### Window Chrome
- **Title bar height**: 28px
- **Title bar background**: #f5f5f5
- **Title text color**: #000000
- **Title text font**: Inter, 13px
- **Window border**: 1px #e0e0e0
- **Window shadow**: 8px rgba(0,0,0,0.08)
- **Border radius**: 4px

#### Window Controls (Glyphs)
- **Close button**: Circle glyph, 16x16px
- **Minimize button**: Minus glyph, 16x16px
- **Maximize button**: Square glyph, 16x16px
- **Button color**: #000000
- **Button hover**: #f0f0f0 background
- **Padding**: 6px

### Taskbar

- **Background**: #f5f5f5
- **Height**: 40px
- **Border-top**: 1px #e0e0e0
- **Icon size**: 24x24px
- **Icon spacing**: 8px
- **Padding**: 8px

#### Taskbar Buttons
- **Resting**: Glyph on transparent background
- **Hover**: Glyph on #e8e8e8 background
- **Active**: Glyph on #d0d0d0 background, bottom border 2px #000000

### System Tray

- **Icon size**: 16x16px
- **Spacing**: 6px
- **Padding**: 8px 12px
- **Background**: #f5f5f5

### Workspace Switcher

- **Position**: Top-left or top-right
- **Display**: Glyph indicators (1, 2, 3, etc.)
- **Active indicator**: Bold text or accent background
- **Spacing**: 12px between workspace indicators

### Alt-Tab Switcher

- **Window grid layout**: Thumbnails + app name
- **Background**: #ffffff with border #e0e0e0
- **App name font**: Inter 12px
- **Selection highlight**: 2px border #000000
- **Item padding**: 8px

### System Menu

- **Trigger**: Top-right corner click or hotkey
- **Background**: #ffffff
- **Border**: 1px #e0e0e0
- **Text color**: #000000
- **Separator**: 1px #f0f0f0
- **Item padding**: 12px 16px
- **Icon size**: 16x16px

### Notification System

- **Toast background**: #ffffff
- **Toast border**: 1px #e0e0e0
- **Toast shadow**: 8px rgba(0,0,0,0.08)
- **Text color**: #000000
- **Title font**: Inter 14px bold
- **Body font**: Inter 12px
- **Padding**: 12px 16px
- **Animation**: Slide from right, 200ms ease-out
- **Auto-dismiss**: 4 seconds

### Context Menus

- **Background**: #ffffff
- **Border**: 1px #e0e0e0
- **Shadow**: 8px rgba(0,0,0,0.12)
- **Item padding**: 12px 16px
- **Item height**: 32px
- **Separator**: 1px #f0f0f0
- **Icon size**: 16x16px

### Desktop Background

- **Default color**: #ffffff (pure white)
- **Alt option**: #f5f5f5 (light gray)
- **Wallpaper**: Minimalist, high-contrast patterns recommended

## Fonts

- **System font**: Inter
- **Fallbacks**: -apple-system, BlinkMacSystemFont, Segoe UI, sans-serif
- **Monospace**: JetBrains Mono
- **Default size**: 13px for UI elements

## Glyph System

All UI controls use the Flux-OS glyph system:
- 16x16px for standard UI
- 24x24px for toolbars
- 32x32px for application launcher
- Black (#000000) for resting state
- Gray (#999999) for inactive/disabled

## Animation Timings

- **Window open/close**: 200ms ease-out
- **Menu show**: 100ms ease-out
- **Hover effects**: 100ms ease-out
- **Focus indicators**: Instant (no animation)
- **Workspace switch**: 300ms ease-in-out

## Accessibility

- **Focus indicators**: 2px border #000000
- **Minimum contrast**: 4.5:1 for text
- **Touch targets**: Minimum 44x44px
- **Motion**: Respect `prefers-reduced-motion` setting
