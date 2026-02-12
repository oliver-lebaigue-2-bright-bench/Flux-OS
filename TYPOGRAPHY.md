# FLUX-OS Typography System

## 👨‍💻 Developer

**Oliver Lebaigue** - Sole Developer  
*Age: 16 | Passionate about minimal OS design and retro computing*

---

## 📖 Typography Philosophy

Typography is the foundation of Flux OS. Just as Nothing OS revolutionized mobile with its text-first approach, Flux OS brings this philosophy to the desktop. Every pixel of text must justify its existence.

### Core Principles

1. **Legibility First**: Every character must be instantly readable
2. **Hierarchy Through Size**: Weight and color support, not the primary differentiator
3. **Consistent Rhythm**: Vertical rhythm based on baseline grid
4. **Efficient Space**: More information, less visual weight
5. **Functional Decoration**: Typography that serves function

---

## 🔤 Font Family

### Primary Font: Flux Sans

A custom-designed bitmap sans-serif inspired by Nothing's typography:

```
Flux Sans - Character Set (ASCII Extended)
```

**Design Characteristics:**
- Geometric foundations (near-perfect circles, straight lines)
- Open apertures for legibility
- Moderate x-height for readability
- True black weight for maximum contrast
- Slight curvature on select terminals

### Monospace Font: Flux Mono

For code, terminal, and tabular data:

**Design Characteristics:**
- Fixed-width characters (all glyphs same width)
- Technical, precise appearance
- Code-friendly distinguishability (0/O, 1/l/I)
- Terminal-green accent in terminal mode

---

## 📐 Font Sizes

### Complete Size Table

| Token | Size | Line Height | Tracking | Usage |
|-------|------|-------------|----------|-------|
| `display-xl` | 48px | 56px | -2px | Hero text, splash screens |
| `display-lg` | 36px | 44px | -1px | Major headlines |
| `display-md` | 28px | 36px | 0px | Section headers |
| `heading-xl` | 24px | 32px | 0px | Window titles |
| `heading-lg` | 20px | 28px | 0px | Dialog titles |
| `heading-md` | 18px | 26px | 0px | Subsection headers |
| `body-xl` | 16px | 24px | 0px | Primary body text |
| `body-lg` | 14px | 20px | 0px | Secondary body text |
| `body-md` | 12px | 18px | 0px | Compact text |
| `caption-lg` | 11px | 16px | 0px | Metadata, labels |
| `caption-md` | 10px | 14px | 0px | Timestamps, status |
| `caption-sm` | 9px | 12px | 0px | Fine print |
| `mono-lg` | 14px | 20px | 0px | Code, terminal |
| `mono-md` | 12px | 16px | 0px | Compact code |
| `mono-sm` | 10px | 14px | 0px | Dense data |

---

## 🎨 Typography Hierarchy

### Level 1: Display (Hero)

```
Font: Flux Sans Bold
Size: 48px
Color: #FFFFFF
Background: #000000
Tracking: -2px
Usage: Splash screen logo, boot messages
Example: "FLUX-OS"
```

### Level 2: Heading (Primary)

```
Font: Flux Sans Medium
Size: 24px
Color: #FFFFFF
Background: #1A1A1A
Usage: Window titles, main section headers
Example: "System Settings"
```

### Level 3: Subheading (Secondary)

```
Font: Flux Sans Regular
Size: 18px
Color: #FFFFFF
Background: #000000
Usage: Dialog titles, subsection headers
Example: "Display Settings"
```

### Level 4: Body (Primary)

```
Font: Flux Sans Regular
Size: 14px
Color: #FFFFFF
Background: #000000
Line Height: 20px
Usage: Content paragraphs, descriptions
Example: "Configure your display preferences below."
```

### Level 5: Body (Secondary)

```
Font: Flux Sans Regular
Size: 12px
Color: #A8A8A8
Background: #000000
Line Height: 18px
Usage: Secondary information, tooltips
Example: "Press Enter to confirm"
```

### Level 6: Caption

```
Font: Flux Sans Light
Size: 10px
Color: #808080
Background: #000000
Line Height: 14px
Usage: Timestamps, status information
Example: "Last updated: 2 minutes ago"
```

---

## 🔣 Character Sets

### Flux Sans (Primary)

```
ABCDEFGHIJKLMNOPQRSTUVWXYZ
abcdefghijklmnopqrstuvwxyz
0123456789
!@#$%^&*()_+-=[]{}|;':",./<>?
`~
```

### Flux Mono (Monospace)

```
ABCDEFGHIJKLMNOPQRSTUVWXYZ
abcdefghijklmnopqrstuvwxyz
0123456789
!@#$%^&*()_+-=[]{}|;':",./<>?
`~
```

### Glyph Icons (Custom)

```
[NOTIFY]  - Notification bell
[CLOCK]   - Time display
[GEAR]    - Settings
[POWER]   - Shutdown
[VOLUME]  - Speaker icon
[NETWORK] - Connection status
[SEARCH]  - Magnifying glass
[PLUS]    - Add/new
[MINUS]   - Remove/delete
[CLOSE]   - X/cancel
[CHECK]   - ✓/confirm
[ARROW-L] - Left navigation
[ARROW-R] - Right navigation
[ARROW-U] - Up navigation
[ARROW-D] - Down navigation
```

---

## 📏 Vertical Rhythm

### Baseline Grid

- **Base Unit**: 4px
- **Line Height Multiple**: Even numbers (2, 4, 6...)
- **Paragraph Spacing**: 1× line height
- **Section Spacing**: 2× line height

### Line Length

| Context | Characters | Pixels | Recommendation |
|---------|------------|--------|----------------|
| **Optimal Reading** | 45-75 | ~600px | Primary body text |
| **Wide Layout** | 75-100 | ~800px | Tables, lists |
| **Narrow Layout** | 30-45 | ~400px | Sidebars, dialogs |

---

## 🎯 Use Cases

### Clock Display (Dot-Matrix)

```
Font: Flux Mono Bold
Size: 36px
Color: #FFFFFF
Background: #000000
Tracking: 4px (fixed-width digits)
Example: 12:34:56
```

### Window Title

```
Font: Flux Sans Medium
Size: 24px
Color: #FFFFFF
Background: #1A1A1A
Tracking: -1px
Example: "File Manager"
```

### Menu Item

```
Font: Flux Sans Regular
Size: 14px
Color: #FFFFFF
Background: #000000
Active: #D4AF37 (Glyph Gold)
Example: "Open File..."
```

### Terminal Output

```
Font: Flux Mono Regular
Size: 14px
Color: #00FF00 (optional terminal green)
Background: #000000
Line Height: 20px
Example: flux@os:~$ _
```

### Notification Text

```
Font: Flux Sans Medium
Size: 14px
Color: #FFFFFF
Background: #1A1A1A
Accent: #D4AF37
Example: "New message from Alice"
```

### Timestamp

```
Font: Flux Sans Light
Size: 10px
Color: #808080
Background: #000000
Example: "2:34 PM"
```

---

## 🖥️ Rendering Implementation

### Bitmap Font Structure

```
struct FluxChar {
    uint8_t width;      // Character width (pixels)
    uint8_t height;    // Character height (pixels)
    uint8_t data[];    // Bitmap data (rows of bytes)
};

struct FluxFont {
    const char* name;          // Font family name
    uint8_t size;              // Size in pixels
    uint8_t baseline;          // Baseline offset
    uint8_t height;            // Total height
    FluxChar chars[256];       // All characters
};
```

### Rendering Function

```c
void render_char(Framebuffer* fb, int x, int y, char c, Color color) {
    FluxChar* ch = &font_current->chars[(unsigned char)c];
    for (int row = 0; row < ch->height; row++) {
        for (int col = 0; col < ch->width; col++) {
            if (ch->data[row * ((ch->width + 7) / 8) + col / 8] & (1 << (7 - col % 8))) {
                fb_set_pixel(fb, x + col, y + row, color);
            }
        }
    }
}
```

---

## 🎨 Color Combinations

### Valid Combinations

| Background | Primary Text | Secondary | Caption |
|------------|--------------|-----------|---------|
| **#000000** | #FFFFFF | #A8A8A8 | #808080 |
| **#1A1A1A** | #FFFFFF | #C0C0C0 | #A8A8A8 |
| **#D4AF37** | #000000 | #1A1A1A | #404040 |
| **#FFFFFF** | #000000 | #404040 | #606060 |

### Invalid Combinations

- ❌ #FFFFFF on #FFFFFF
- ❌ #808080 on #000000 (fails contrast)
- ❌ #D4AF37 on #D4AF37

---

## 📚 References

### Inspiration

- **Nothing OS Typography**: The philosophical foundation
- **Swiss Style**: Grid-based layout, hierarchy
- **IBM Plex**: Technical, precise typography
- **Inter**: Screen-optimized legibility

---

*FLUX-OS Typography System v1.0*  
*Created by Oliver Lebaigue (Age 16)*  
*Nothing OS Inspired Desktop Experience*

