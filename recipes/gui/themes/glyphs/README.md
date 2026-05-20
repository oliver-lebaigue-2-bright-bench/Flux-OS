# Flux-OS Icon Glyph Pack

Minimalist glyph system inspired by Nothing (R).

## Icon Categories

### System Icons
- `system-power.svg` - Power button
- `system-settings.svg` - Gear/settings
- `system-info.svg` - Information
- `system-help.svg` - Help
- `system-time.svg` - Clock
- `system-volume.svg` - Speaker
- `system-brightness.svg` - Sun/brightness
- `system-battery.svg` - Battery
- `system-network.svg` - WiFi signal

### Navigation Icons
- `arrow-back.svg` - Back arrow
- `arrow-forward.svg` - Forward arrow
- `arrow-up.svg` - Up arrow
- `arrow-down.svg` - Down arrow
- `arrow-left.svg` - Left arrow
- `arrow-right.svg` - Right arrow
- `home.svg` - Home/house shape
- `menu.svg` - Hamburger menu (three lines)
- `close.svg` - X/close
- `minimize.svg` - Dash/minimize
- `maximize.svg` - Square/maximize

### Document Icons
- `file-text.svg` - Text file
- `file-image.svg` - Image file
- `file-audio.svg` - Audio file
- `file-video.svg` - Video file
- `file-archive.svg` - Archive/zip
- `file-code.svg` - Code file
- `folder.svg` - Folder
- `folder-open.svg` - Open folder
- `document.svg` - Document/page
- `search.svg` - Magnifying glass

### Communication Icons
- `mail.svg` - Envelope
- `mail-open.svg` - Open envelope
- `chat.svg` - Chat bubble
- `notification.svg` - Bell
- `notification-off.svg` - Muted bell
- `phone.svg` - Phone handset

### Creation Icons
- `edit.svg` - Pencil
- `edit-box.svg` - Document with pencil
- `paint.svg` - Palette
- `brush.svg` - Paintbrush
- `add.svg` - Plus/cross
- `delete.svg` - Trash can
- `duplicate.svg` - Stacked squares
- `copy.svg` - Two overlapping squares
- `paste.svg` - Clipboard

### Utility Icons
- `check.svg` - Checkmark
- `close.svg` - X
- `warning.svg` - Triangle with exclamation
- `error.svg` - Circle with X
- `success.svg` - Circle with checkmark
- `info.svg` - Circle with i
- `star.svg` - Star (outlined)
- `star-filled.svg` - Star (filled)
- `bookmark.svg` - Bookmark
- `heart.svg` - Heart (outlined)
- `heart-filled.svg` - Heart (filled)
- `share.svg` - Share/export
- `download.svg` - Download arrow
- `upload.svg` - Upload arrow
- `refresh.svg` - Refresh/circular arrow
- `settings.svg` - Sliders/controls
- `filter.svg` - Funnel

## Design Specifications

### Grid & Stroke
- **Base grid**: 24x24px
- **Stroke width**: 1.5px
- **Padding**: 2px from edge
- **Corner radius**: 1.5px for rounded corners

### Color
- **Default**: #000000 (black)
- **Disabled**: #cccccc (light gray) at 50% opacity
- **Accent**: #999999 (medium gray)
- **Interactive**: Darken to #1a1a1a on hover

### Sizes
- **Icon 16x16px**: For compact UI
- **Icon 24x24px**: Standard toolbar size
- **Icon 32x32px**: Application launcher
- **Icon 48x48px**: Dialog/preferences
- **Icon 64x64px**: Application icons

### Variants
- **Outlined**: Default style (thin stroke)
- **Filled**: Solid fill versions
- **Bold**: 2px stroke for emphasis

## Usage

```html
<!-- SVG inline -->
<img src="/usr/share/icons/Flux-Nothing/24x24/system/settings.svg" alt="Settings">

<!-- CSS background -->
<div class="icon icon-settings"></div>
```

```css
.icon {
  display: inline-block;
  width: 24px;
  height: 24px;
  background-size: contain;
}

.icon-settings {
  background-image: url('/usr/share/icons/Flux-Nothing/24x24/system/settings.svg');
}
```

## Accessibility

- All glyphs tested at 16x16px minimum
- Sufficient contrast on light and dark backgrounds
- No anti-aliasing artifacts
- Clear, recognizable at small sizes

## Installation

```bash
cp -r Flux-Nothing /usr/share/icons/
update-icon-caches /usr/share/icons/Flux-Nothing
```

## License

Flux-OS Glyph Pack is licensed under MIT License.
