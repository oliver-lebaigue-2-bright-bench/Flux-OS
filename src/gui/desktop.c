#include "gui.h"
#include "../graphics/gfx.h"

// Note: Memory functions (malloc, free, etc.) are provided by libc_compat_arm.c
// which is linked in during build

// Global GUI system
gui_system_t gui;

// Forward declarations
static void gui_vga_clear(void);

// Initialize GUI system
void gui_init(int width, int height, void* fb, int pitch) {
    // Initialize graphics with framebuffer
    gfx_init(width, height, fb, pitch);
    
    // Initialize state
    gui.initialized = 1;
    gui.width = width;
    gui.height = height;
    gui.pitch = pitch;
    gui.framebuffer = fb;
    
    // Initialize input state
    gui.mouse.x = width / 2;
    gui.mouse.y = height / 2;
    gui.mouse.dx = 0;
    gui.mouse.dy = 0;
    gui.mouse.buttons = 0;
    gui.mouse.wheel = 0;
    gui.mouse.is_abs = 1;
    
    gui.keyboard.scancode = 0;
    gui.keyboard.key = 0;
    gui.keyboard.pressed = 0;
    gui.keyboard.shift = 0;
    gui.keyboard.ctrl = 0;
    gui.keyboard.alt = 0;
    
    // Window management
    gui.windows = 0;
    gui.active_window = 0;
    gui.dragging_window = 0;
    gui.resizing_window = 0;
    gui.window_counter = 0;
    
    // Desktop
    gui.taskbar_height = TASKBAR_HEIGHT;
    gui.clock_x = width - 80;
    gui.start_text = "Start";
    
    // Event queue
    gui.event_head = 0;
    gui.event_tail = 0;
    
    // Loop control
    gui.running = 1;
    gui.needs_redraw = 1;
    
    // Initialize window system
    window_system_init();
    
    // Clear screen with background
    clear_screen(GUI_COLOR_DESKTOP);
}

// Shutdown GUI
void gui_shutdown() {
    gui.running = 0;
    gui.initialized = 0;
}

// Queue an event
void gui_queue_event(event_t* event) {
    int next = (gui.event_tail + 1) % 64;
    if (next != gui.event_head) {
        gui.event_queue[gui.event_tail] = *event;
        gui.event_tail = next;
    }
}

// Poll for event (non-blocking)
int gui_poll_event(event_t* event) {
    if (gui.event_head == gui.event_tail) {
        return 0;
    }
    *event = gui.event_queue[gui.event_head];
    gui.event_head = (gui.event_head + 1) % 64;
    return 1;
}

// Handle GUI events
void gui_handle_event(event_t* event) {
    if (!event) return;
    
    switch (event->type) {
        case EVENT_MOUSE_MOVE: {
            // Update mouse position
            gui.mouse.x = event->mouse_x;
            gui.mouse.y = event->mouse_y;
            // Find window under cursor and route event
            window_t* win = window_at(event->mouse_x, event->mouse_y);
            if (win) {
                window_handle_event(win, event);
            }
            break;
        }
        case EVENT_MOUSE_DOWN: {
            gui.mouse.buttons |= (1 << event->mouse_button);
            // Bring window to front if clicked
            window_t* win = window_at(event->mouse_x, event->mouse_y);
            if (win) {
                window_bring_to_front(win);
                window_handle_event(win, event);
            }
            break;
        }
        case EVENT_MOUSE_UP: {
            gui.mouse.buttons &= ~(1 << event->mouse_button);
            window_t* win = window_at(event->mouse_x, event->mouse_y);
            if (win) {
                window_handle_event(win, event);
            }
            break;
        }
        case EVENT_MOUSE_CLICK: {
            window_t* win = window_at(event->mouse_x, event->mouse_y);
            if (win && win->on_click) {
                win->on_click(win, event->mouse_x, event->mouse_y);
            }
            break;
        }
        case EVENT_KEY_DOWN: {
            gui.keyboard.scancode = event->key_code;
            gui.keyboard.key = event->key_code;
            gui.keyboard.pressed = 1;
            
            // ESC to exit
            if (event->key_code == 0x01) {  // ESC key
                gui.running = 0;
            }
            break;
        }
        case EVENT_KEY_UP: {
            gui.keyboard.pressed = 0;
            break;
        }
        case EVENT_REDRAW:
            gui.needs_redraw = 1;
            break;
            
        default:
            break;
    }
}

// Get time string
void gui_get_time_string(char* buffer, size_t size) {
    snprintf(buffer, size, "12:00");
}

// Redraw entire GUI
void gui_redraw_all() {
    if (!gui.framebuffer) return;
    
    // Draw desktop background - gradient effect
    for (int y = 0; y < gui.height - gui.taskbar_height; y++) {
        // Create a subtle gradient from top to bottom
        uint8_t r = 0x0d + (y * 3 / 100);
        uint8_t g = 0x3d + (y * 3 / 100);
        uint8_t b = 0x52 + (y * 2 / 100);
        uint32_t color = 0xFF000000 | (r << 16) | (g << 8) | b;
        
        uint32_t* row = (uint32_t*)((uint8_t*)gui.framebuffer + y * gui.pitch);
        for (int x = 0; x < gui.width; x++) {
            row[x] = color;
        }
    }
    
    // Draw windows
    windows_draw_all();
    
    // Draw taskbar
    int y = gui.height - gui.taskbar_height;
    fill_rect(0, y, gui.width, gui.taskbar_height, GUI_COLOR_TASKBAR);
    draw_line(0, y, gui.width, y, GUI_COLOR_LIGHT_GRAY);
    
    // Draw start button with gradient
    fill_rect(5, y + 4, 70, 24, GUI_COLOR_BUTTON);
    draw_rect(5, y + 4, 70, 24, GUI_COLOR_BORDER);
    draw_string(13, y + 10, "Start", GUI_COLOR_BLACK, GUI_COLOR_BUTTON);
    
    // Draw clock area
    char time_str[16];
    gui_get_time_string(time_str, sizeof(time_str));
    draw_string(gui.width - 70, y + 10, time_str, GUI_COLOR_WHITE, GUI_COLOR_TASKBAR);
    
    gui.needs_redraw = 0;
}

void widget_draw(widget_t* widget) {
    if (!widget) return;
    if (widget->draw) {
        widget->draw(widget);
    }
}

void widget_destroy(widget_t* widget) {
    if (!widget) return;
    if (widget->next) {
        widget_destroy(widget->next);
    }
    free(widget);
}

void gui_update_clock() {
    if (!gui.framebuffer) return;
}

void gui_create_desktop() {
    // Create welcome window - centered on screen
    window_t* welcome = window_create(
        gui.width / 2 - 200,
        gui.height / 2 - 180,
        400,
        320,
        "Welcome to Flux-OS"
    );
    welcome->flags = WINDOW_FLAG_HAS_CLOSE | WINDOW_FLAG_HAS_MINIMIZE | WINDOW_FLAG_HAS_MAXIMIZE;
    welcome->bg_color = GUI_COLOR_WINDOW_BG;
    
    // Draw welcome content directly on window
    // (In a full implementation, this would be handled by window content widgets)
    
    // Create about window
    window_t* about = window_create(
        gui.width / 2 - 150,
        gui.height / 2 - 50,
        300,
        150,
        "About Flux-OS"
    );
    about->flags = WINDOW_FLAG_HAS_CLOSE;
    about->bg_color = GUI_COLOR_WINDOW_BG;
    
    // Request initial redraw
    gui.needs_redraw = 1;
}

void gui_register_shortcut(int key, int ctrl, void (*callback)()) {
    (void)key;
    (void)ctrl;
    (void)callback;
}

void gui_set_mouse_position(int x, int y) {
    gui.mouse.x = x;
    gui.mouse.y = y;
}

// gui_run is defined separately for x86 and ARM
// For ARM, we provide a simpler event loop that can be called from kernel.c
#ifndef __aarch64__
#include <stdint.h>

// Port I/O helpers for x86
static inline uint8_t port_inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void port_outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_DATA_PORT 0x60
#define MOUSE_STATUS_PORT 0x64
#define MOUSE_DATA_PORT 0x60

static int last_mouse_buttons = 0;
static int g_mouse_packet_byte = 0;
static uint8_t g_mouse_packet[3] = {0};

// Main GUI loop (x86 version)
void gui_run() {
    if (!gui.initialized) return;

    event_t event;

    g_mouse_packet_byte = 0;
    last_mouse_buttons = gui.mouse.buttons;

    // Clear keyboard buffer
    for (int i = 0; i < 256; i++) {
        port_inb(KEYBOARD_DATA_PORT);
    }

    // Main event loop
    while (gui.running) {
        // Simple delay
        for (int i = 0; i < 50000; i++) {
            __asm__("nop");
        }
        
        // Poll for keyboard input
        int key_count = 0;
        while (key_count < 4) {
            uint8_t status = port_inb(KEYBOARD_STATUS_PORT);
            if (status & 0x01) {
                uint8_t scancode = port_inb(KEYBOARD_DATA_PORT);
                keyboard_handle_scancode(scancode);
                key_count++;
            } else {
                break;
            }
        }
        
        // Poll for mouse input
        if (port_inb(MOUSE_STATUS_PORT) & 0x20) {
            uint8_t byte = port_inb(MOUSE_DATA_PORT);
            
            if ((byte & 0x08) == 0) {
                g_mouse_packet_byte = 0;
            } else {
                g_mouse_packet[g_mouse_packet_byte++] = byte;
                
                if (g_mouse_packet_byte >= 3) {
                    g_mouse_packet_byte = 0;
                    
                    int dx = (int8_t)g_mouse_packet[1];
                    int dy = -(int8_t)g_mouse_packet[2];
                    
                    gui.mouse.x += dx;
                    gui.mouse.y += dy;
                    gui.mouse.buttons = g_mouse_packet[0] & 0x07;
                    
                    if (gui.mouse.x < 0) gui.mouse.x = 0;
                    if (gui.mouse.x >= gui.width) gui.mouse.x = gui.width - 1;
                    if (gui.mouse.y < 0) gui.mouse.y = 0;
                    if (gui.mouse.y >= gui.height) gui.mouse.y = gui.height - 1;
                    
                    event.type = EVENT_MOUSE_MOVE;
                    event.mouse_x = gui.mouse.x;
                    event.mouse_y = gui.mouse.y;
                    gui_queue_event(&event);
                    
                    for (int i = 0; i < 3; i++) {
                        int was_pressed = (last_mouse_buttons & (1 << i));
                        int is_pressed = (gui.mouse.buttons & (1 << i));
                        
                        if (!was_pressed && is_pressed) {
                            event.type = EVENT_MOUSE_DOWN;
                            event.mouse_x = gui.mouse.x;
                            event.mouse_y = gui.mouse.y;
                            event.mouse_button = (mouse_button_t)i;
                            gui_queue_event(&event);
                        } else if (was_pressed && !is_pressed) {
                            event.type = EVENT_MOUSE_UP;
                            event.mouse_x = gui.mouse.x;
                            event.mouse_y = gui.mouse.y;
                            event.mouse_button = (mouse_button_t)i;
                            gui_queue_event(&event);
                            
                            event.type = EVENT_MOUSE_CLICK;
                            gui_queue_event(&event);
                        }
                    }
                    last_mouse_buttons = gui.mouse.buttons;
                }
            }
        }
        
        // Process all queued events
        while (gui_poll_event(&event)) {
            gui_handle_event(&event);
        }
        
        // Redraw if needed
        if (gui.needs_redraw && gui.framebuffer) {
            gui_redraw_all();
        }
    }
}
#endif // !__aarch64__

