#include "gui.h"
#include "../graphics/gfx.h"
#include "../libc_compat.h"

// Global GUI system
gui_system_t gui;

// VGA text mode buffer
static volatile uint16_t* vga_buffer = (volatile uint16_t*)0xB8000;
static int vga_col = 0;
static int vga_row = 0;

// Forward declarations
static void vga_putchar(char c, uint8_t color);
static void gui_vga_clear();
static void vga_write(const char* str, uint8_t color);

// Initialize GUI system
void gui_init(int width, int height, void* fb, int pitch) {
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
    
    // Initialize subsystems
    window_system_init();
    keyboard_init();
    
    // Enable mouse
    enable_mouse();
    
    // Clear VGA buffer
    gui_vga_clear();
    vga_write("GUI Initialized", 0x0A);
}

// VGA helper functions
static void vga_putchar(char c, uint8_t color) {
    if (c == '\n') {
        vga_row++;
        vga_col = 0;
        return;
    }
    if (c == '\r') {
        vga_col = 0;
        return;
    }
    if (vga_row >= 25) {
        // Scroll
        for (int r = 0; r < 24; r++) {
            for (int c = 0; c < 80; c++) {
                vga_buffer[r * 80 + c] = vga_buffer[(r + 1) * 80 + c];
            }
        }
        for (int c = 0; c < 80; c++) {
            vga_buffer[24 * 80 + c] = 0x0A20;
        }
        vga_row = 24;
    }
    vga_buffer[vga_row * 80 + vga_col] = (uint16_t)c | ((uint16_t)color << 8);
    vga_col++;
    if (vga_col >= 80) {
        vga_col = 0;
        vga_row++;
    }
}


static void gui_vga_clear() {
    for (int i = 0; i < 80 * 25; i++) {
        vga_buffer[i] = 0x0A20;
    }
    vga_col = 0;
    vga_row = 0;
}

static void vga_write(const char* str, uint8_t color) {
    while (*str) {
        vga_putchar(*str++, color);
    }
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
        case EVENT_MOUSE_MOVE:
        case EVENT_MOUSE_DOWN:
        case EVENT_MOUSE_UP:
        case EVENT_MOUSE_CLICK:
            // Could update cursor position on screen
            break;
            
        case EVENT_KEY_DOWN:
        case EVENT_KEY_UP:
            break;
            
        case EVENT_REDRAW:
            gui.needs_redraw = 1;
            break;
            
        default:
            break;
    }
}

// Port I/O helpers
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

// Main GUI loop
void gui_run() {
    if (!gui.initialized) return;

    event_t event;

    g_mouse_packet_byte = 0;
    last_mouse_buttons = gui.mouse.buttons;

    // Clear keyboard buffer
    for (int i = 0; i < 256; i++) {
        port_inb(KEYBOARD_DATA_PORT);
    }

    // Initial message
    vga_write("\nStarting GUI loop...\n", 0x0A);
    vga_write("Press ESC to exit\n", 0x07);

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
                
                // ESC to exit
                if ((scancode & 0x7F) == 0x01) {
                    vga_write("\nGUI exiting...\n", 0x0C);
                    gui.running = 0;
                }
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

// Get time string
void gui_get_time_string(char* buffer, size_t size) {
    snprintf(buffer, size, "12:00");
}

// Placeholder functions
void gui_redraw_all() {
    if (!gui.framebuffer) return;
    
    // Draw desktop background
    fill_rect(0, 0, gui.width, gui.height - gui.taskbar_height, GUI_COLOR_DESKTOP);
    
    // Draw windows
    windows_draw_all();
    
    // Draw taskbar
    int y = gui.height - gui.taskbar_height;
    fill_rect(0, y, gui.width, gui.taskbar_height, GUI_COLOR_TASKBAR);
    draw_line(0, y, gui.width, y, GUI_COLOR_LIGHT_GRAY);
    
    // Draw simple start button
    fill_rect(5, y + 4, 70, 24, GUI_COLOR_LIGHT_GRAY);
    draw_rect(5, y + 4, 70, 24, GUI_COLOR_DARK_GRAY);
    draw_string(13, y + 12, "Start", GUI_COLOR_BLACK, GUI_COLOR_LIGHT_GRAY);
    
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
    // Create welcome window
    window_t* welcome = window_create(
        gui.width / 2 - 200,
        gui.height / 2 - 150,
        400,
        300,
        "Welcome to Flux-OS"
    );
    welcome->flags = WINDOW_FLAG_HAS_CLOSE | WINDOW_FLAG_HAS_MINIMIZE;
    
    // Create about window
    window_t* about = window_create(
        gui.width / 2 - 150,
        gui.height / 2 - 100,
        300,
        200,
        "About Flux-OS"
    );
    about->flags = WINDOW_FLAG_HAS_CLOSE | WINDOW_FLAG_HAS_MINIMIZE;
}

void gui_register_shortcut(int key, int ctrl, void (*callback)()) {
}

void gui_set_mouse_position(int x, int y) {
    gui.mouse.x = x;
    gui.mouse.y = y;
}
