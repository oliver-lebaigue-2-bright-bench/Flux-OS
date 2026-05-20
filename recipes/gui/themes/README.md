# Flux-OS Application Theme

## Overview

This directory contains theme assets for Flux-OS applications using the Nothing (R) design language.

## Structure

```
ortheme/
├── gtk/                    # GTK 4 theme
├── qt/                     # Qt 5/6 theme
├── web/                    # Web-based apps (Electron, web apps)
├── orbital/                # Orbital desktop environment
└── shared/                 # Shared assets (glyphs, fonts)
```

## Theme Principles

1. **Minimalism**: Remove visual clutter
2. **Consistency**: Unified aesthetic across all apps
3. **Accessibility**: High contrast, clear focus states
4. **Performance**: Lightweight, efficient rendering
5. **Responsiveness**: Smooth animations, fluid interactions

## Color Palette

```css
--color-black:        #000000;
--color-white:        #FFFFFF;
--color-dark-gray:    #1a1a1a;
--color-light-gray:   #f5f5f5;
--color-medium-gray:  #808080;
--color-accent-gray:  #999999;
--color-disabled:     #cccccc;
```

## Using the Theme

### GTK Applications
```bash
export GTK_THEME=Flux-Nothing
```

### Qt Applications
```bash
export QT_STYLE_OVERRIDE=Flux-Nothing
```

### Web Applications
Include the CSS file:
```html
<link rel="stylesheet" href="/usr/share/themes/Flux-Nothing/web/styles.css">
```

## Development

To modify the theme:

1. Edit the source files in respective toolkit directories
2. Test in target applications
3. Ensure consistency across platforms
4. Verify accessibility compliance
5. Submit changes via pull request

## License

Flux-OS Theme is licensed under MIT License.
