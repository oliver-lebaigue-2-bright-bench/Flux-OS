# FLUX-OS Glyph Interface System

## 👨‍💻 Developer

**Oliver Lebaigue** - Sole Developer  
*Age: 16 | Passionate about minimal OS design and retro computing*

---

## 🌟 Overview

The Glyph Interface is the signature design element of Nothing OS, brought to desktop in Flux OS. Originally a hardware LED array on Nothing phones, the Glyph Interface in Flux OS is a software-based notification and status system using animated geometric patterns.

### Core Concepts

- **Software Glyphs**: All glyphs rendered via framebuffer
- **Pattern Language**: Distinct patterns for different events
- **Peripheral Awareness**: Designed for edge-of-vision visibility
- **Progressive Disclosure**: Subtle → urgent notification levels

---

## 🎯 Glyph Types

### 1. Notification Glyph

**Purpose**: Alert user of new notifications

**Visual**:
```
┌─────────────────┐
│      ╭───╮      │
│     │ ●● │     │
│      ╰───╯      │
│    Notification  │
└─────────────────┘
```

**Pattern**:
```
Duration: 3 seconds
Animation: Pulse (expand/contract)
Color: #D4AF37 (Glyph Gold)
Speed: 1.5 seconds per pulse
```

**Default Pattern Code**:
```
[GLYPH-NOTIFY]
{
  "type": "pulse",
  "color": "#D4AF37",
  "duration": 3000,
  "speed": 1500,
  "repeat": 3,
  "intensity": 0.8
}
```

### 2. Progress Ring

**Purpose**: Show ongoing progress (downloads, installation, etc.)

**Visual**:
```
        ╭───╮
      ╱       ╲
    ╱    75%   ╲
    │           │
    ╲           ╱
      ╲       ╱
        ╰───╯
```

**Pattern**:
```
Duration: Until complete
Animation: Segmented fill (clockwise)
Color: #D4AF37 (filled), #1A1A1A (track)
Speed: 360° over operation duration
Easing: Linear
```

**Progress Ring Code**:
```
[GLYPH-PROGRESS]
{
  "type": "ring",
  "color": "#D4AF37",
  "track_color": "#1A1A1A",
  "stroke": 4,
  "segments": 20,
  "easing": "linear"
}
```

### 3. Volume Glyph

**Purpose**: Visual volume feedback (Nothing's signature)

**Visual**:
```
    │
    │    ╭───╮
────┼───╱     ╲───
    │  ╱  ◉    ╲
    │  ╲       ╱
    │   ╰───╯
    │
    │
```

**Pattern**:
```
Duration: Duration of volume change
Animation: Fill from left to right
Color: #FFFFFF
Levels: 10 segments
Hold: 1 second at final level
```

**Volume Glyph Code**:
```
[GLYPH-VOLUME]
{
  "type": "segments",
  "color": "#FFFFFF",
  "segments": 10,
  "max_filled": {volume_level},
  "animation": "fill_left",
  "hold_time": 1000
}
```

### 4. System Pulse

**Purpose**: Subtle system status indicator

**Visual**:
```
    ●
   ●●
  ●●●
   ●●
    ●
```

**Pattern**:
```
Duration: Continuous (background)
Animation: Breathing (slow expand/contract)
Color: #00FF00 (success), #FF0000 (error)
Speed: 4 seconds per cycle
Intensity: Low (for peripheral awareness)
```

**System Pulse Code**:
```
[GLYPH-PULSE]
{
  "type": "breath",
  "color": "#00FF00",
  "duration": 4000,
  "repeat": "infinite",
  "min_opacity": 0.3,
  "max_opacity": 1.0,
  "easing": "ease-in-out"
}
```

### 5. Battery Glyph

**Purpose**: Power status indicator

**Visual**:
```
┌─╮
│█│
│█│
│█│
│█│
└─╯
```

**Pattern**:
```
Duration: Continuous
Animation: Segments fill based on percentage
Color: #00FF00 (>50%), #D4AF37 (20-50%), #FF0000 (<20%)
Blink: Slow blink when <10%
```

**Battery Glyph Code**:
```
[GLYPH-BATTERY]
{
  "type": "segments",
  "color": "{color_by_level}",
  "segments": 5,
  "filled": {battery_percentage / 20},
  "blink_below": 10,
  "blink_speed": 1000
}
```

### 6. Notification Ring

**Purpose**: Count-based notification display

**Visual**:
```
    ╭─────────╮
   ╱   ╭───╮   ╲
  │   │ ● │    │
  │    ╰───╯    │
   ╲    │      ╱
    ╰───╯───╯
```

**Pattern**:
```
Duration: Until acknowledged
Animation: Rotating segments
Color: #D4AF37
Count: Number shown in center
Sound: Optional subtle chime
```

**Notification Ring Code**:
```
[GLYPH-NOTIFY-RING]
{
  "type": "rotate",
  "color": "#D4AF37",
  "segments": 12,
  "speed": 5000,
  "count": {notification_count},
  "sound": "subtle_chime"
}
```

---

## 🎨 Animation Patterns

### Pulse Animation

```
@keyframes glyphPulse {
  0% {
    transform: scale(1);
    opacity: 1;
  }
  50% {
    transform: scale(1.2);
    opacity: 0.8;
  }
  100% {
    transform: scale(1);
    opacity: 1;
  }
}
```

### Ring Fill Animation

```
@keyframes glyphRingFill {
  from {
    stroke-dashoffset: 100%;
  }
  to {
    stroke-dashoffset: 0%;
  }
}
```

### Breathing Animation

```
@keyframes glyphBreathe {
  0%, 100% {
    opacity: 0.3;
    transform: scale(0.9);
  }
  50% {
    opacity: 1;
    transform: scale(1);
  }
}
```

---

## 📊 Notification Priority Levels

### Priority 1: Silent (System)

- **Events**: Power connected, normal operation
- **Glyph**: System Pulse (very subtle)
- **Sound**: None
- **Duration**: Continuous, background

### Priority 2: Passive (Info)

- **Events**: Email received, sync complete
- **Glyph**: Single pulse
- **Sound**: None or very subtle
- **Duration**: 2 seconds

### Priority 3: Active (Alert)

- **Events**: Message received, calendar reminder
- **Glyph**: Repeated pulse + count
- **Sound**: Subtle chime
- **Duration**: Until acknowledged or dismissed

### Priority 4: Urgent (Important)

- **Events**: Phone call, alarm, critical battery
- **Glyph**: Fast flashing
- **Sound**: Alert sound
- **Duration**: Until action taken

---

## 🎛️ Configuration

### Global Settings

```
[GlyphSettings]
enabled = true
default_priority = 2
sound_enabled = true
led_brightness = 0.8
animation_speed = 1.0
```

### Per-App Settings

```
[AppGlyphSettings]
email = {priority: 2, sound: subtle}
calendar = {priority: 3, sound: chime}
messages = {priority: 3, sound: chime}
system = {priority: 1, sound: none}
```

---

## 🔧 Implementation

### Glyph Structure

```c
typedef struct {
    uint8_t type;           // GLYPH_TYPE_*
    uint8_t x;              // Screen X position
    uint8_t y;              // Screen Y position
    uint16_t width;         // Glyph width
    uint16_t height;        // Glyph height
    uint32_t color;         // RGB color
    uint32_t bg_color;      // Background color
    void* animation_state;  // Animation state data
} Glyph;
```

### Render Function

```c
void render_glyph(Glyph* glyph, uint32_t tick) {
    switch (glyph->type) {
        case GLYPH_TYPE_PULSE:
            render_glyph_pulse(glyph, tick);
            break;
        case GLYPH_TYPE_RING:
            render_glyph_ring(glyph, tick);
            break;
        case GLYPH_TYPE_VOLUME:
            render_glyph_volume(glyph, tick);
            break;
        case GLYPH_TYPE_BATTERY:
            render_glyph_battery(glyph, tick);
            break;
        // ... other types
    }
}
```

### Animation Loop

```c
void glyph_update_all(uint32_t tick) {
    Glyph* glyph = glyph_get_list();
    
    while (glyph != NULL) {
        if (glyph_is_active(glyph)) {
            render_glyph(glyph, tick);
        }
        glyph = glyph->next;
    }
}
```

---

## 🎨 Design Guidelines

### Do

- ✅ Use Glyph Gold (#D4AF37) for primary notifications
- ✅ Keep animations subtle (peripheral visibility)
- ✅ Use consistent patterns (learned behavior)
- ✅ Support acknowledgment gestures

### Don't

- ❌ Use bright colors in dark mode contexts
- ❌ Create distracting animations
- ❌ Mix patterns for similar events
- ❌ Override user notification preferences

---

## 📱 Integration Points

### Desktop Integration

- **Taskbar**: Glyph indicators integrated
- **Notification Center**: Full glyph display
- **Lock Screen**: Large clock + glyph status
- **Boot Screen**: Minimal glyph animation

### App Integration

```c
// Example: Create notification glyph
Glyph* notify = glyph_create(
    GLYPH_TYPE_NOTIFY,
    x_position,
    y_position,
    NOTIFICATION_GOLD
);
glyph_set_priority(notify, PRIORITY_ACTIVE);
glyph_set_animation(notify, "pulse");
glyph_show(notify);
```

---

## 🌈 Color Reference

| Glyph Type | Default Color | Alt Color | Error Color |
|------------|---------------|-----------|-------------|
| Notification | #D4AF37 | #FFFFFF | #FF0000 |
| Progress | #D4AF37 | #00FF00 | - |
| Volume | #FFFFFF | #A8A8A8 | - |
| Pulse | #00FF00 | #FFFF00 | #FF0000 |
| Battery | #00FF00 | #D4AF37 | #FF0000 |
| Network | #00FF00 | #D4AF37 | #FF0000 |

---

## 📚 References

### Inspiration

- **Nothing Glyph Interface**: Original hardware implementation
- **Apple Dynamic Island**: Contextual notifications
- **Ambient Computing**: Peripheral awareness design

---

*FLUX-OS Glyph Interface System v1.0*  
*Created by Oliver Lebaigue (Age 16)*  
*Nothing OS Inspired Desktop Experience*

