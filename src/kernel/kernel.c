#include <stdint.h>
#include <stddef.h>

// Include our libc compatibility layer
#include "../libc_compat.h"

// Forward declaration of GUI functions
extern void gui_init(int width, int height, void* fb, int pitch);
extern void gui_run();
extern void gui_shutdown();
extern void gui_create_desktop();

// Graphics library declarations
typedef struct {
    int x, y;
    int width, height;
    int flags;
    uint32_t bg_color;
    uint32_t border_color;
    const char* title;
} window_t;

// Graphics globals (defined in gfx.c)
extern uint32_t* framebuffer;
extern int screen_width;
extern int screen_height;
extern int pitch;

// Graphics function declarations
extern void set_pixel(int x, int y, uint32_t color);
extern void fill_rect(int x, int y, int width, int height, uint32_t color);
extern void draw_rect(int x, int y, int width, int height, uint32_t color);
extern void draw_line(int x1, int y1, int x2, int y2, uint32_t color);
extern void draw_char(int x, int y, char c, uint32_t fg, uint32_t bg);
extern void draw_string(int x, int y, const char* str, uint32_t fg, uint32_t bg);
extern void clear_screen(uint32_t color);
extern void draw_wallpaper();

#define COLOR_BLACK     0xFF000000
#define COLOR_WHITE     0xFFFFFFFF
#define COLOR_RED       0xFFFF0000
#define COLOR_GREEN     0xFF00FF00
#define COLOR_BLUE      0xFF0000FF
#define COLOR_GRAY      0xFF808080
#define COLOR_DARK_GRAY 0xFF404040
#define COLOR_LIGHT_GRAY 0xFFC0C0C0
#define COLOR_CYAN      0xFF00FFFF
#define COLOR_YELLOW    0xFFFFFF00
#define COLOR_MAGENTA   0xFFFF00FF

// Note: vga_buffer is now defined in libc_compat.c
// volatile uint16_t* vga_buffer = (volatile uint16_t*)0xB8000;

// Simple structure for Multiboot info
typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
} __attribute__((packed)) multiboot_info_t;

// VBE Mode Info Structure
typedef struct {
    uint16_t attributes;
    uint8_t  winA, winB;
    uint16_t granularity;
    uint16_t winsize;
    uint16_t segmentA, segmentB;
    uint32_t realFctPtr;
    uint16_t pitch;
    uint16_t width, height;
    uint8_t  wChar, yChar, planes, bpp, banks;
    uint8_t  memory_model, bank_size, image_pages;
    uint8_t  reserved0;
    uint8_t  red_mask, red_position;
    uint8_t  green_mask, green_position;
    uint8_t  blue_mask, blue_position;
    uint8_t  rsv_mask, rsv_position;
    uint8_t  directcolor_attributes;
    uint32_t physbase;  // Framebuffer address
} __attribute__((packed)) vbe_mode_info_t;

// Port I/O helpers - declare these before use
static inline uint8_t inb(uint16_t port);
static inline void outb(uint16_t port, uint8_t val);
static inline uint16_t inw(uint16_t port);
static inline void outw(uint16_t port, uint16_t val);

// Serial port for debugging
#define SERIAL_PORT 0x3F8

static void serial_init() {
    outb(SERIAL_PORT + 1, 0x00);  // Disable interrupts
    outb(SERIAL_PORT + 3, 0x80);  // Enable DLAB
    outb(SERIAL_PORT + 0, 0x03);  // Set divisor to 3 (38400 baud)
    outb(SERIAL_PORT + 1, 0x00);
    outb(SERIAL_PORT + 3, 0x03);  // 8 bits, no parity, one stop bit
    outb(SERIAL_PORT + 2, 0xC7);  // Enable FIFO
    outb(SERIAL_PORT + 4, 0x0B);  // IRQs enabled, RTS/DSR set
}

static void serial_write_char(char c) {
    // Wait for transmit buffer to be empty
    while ((inb(SERIAL_PORT + 5) & 0x20) == 0);
    outb(SERIAL_PORT, c);
}

static void serial_write(const char* str) {
    while (*str) {
        if (*str == '\n') {
            serial_write_char('\r');
        }
        serial_write_char(*str++);
    }
}

// VGA register ports
#define VGA_CRTC_PORT 0x3D4
#define VGA_DATA_PORT 0x3D5
#define VGA_MISC_PORT 0x3C2

// QEMU/VBox VBE port
#define VBE_DISPI_INDEX_ID 0x1CE
#define VBE_DISPI_DATA 0x1CF

// Set VBE mode
static void vbe_set_mode(uint16_t mode) {
    outb(VBE_DISPI_INDEX_ID, 0x02);  // Set VBE mode
    outw(VBE_DISPI_DATA, mode);
}

// Check if VBE is available
static int vbe_available() {
    outb(VBE_DISPI_INDEX_ID, 0x0);  // Check VBE ID
    uint16_t id = inw(VBE_DISPI_DATA);
    return (id == 0xB0C5 || id == 0xB0C4);
}

// Get VBE mode info
static int vbe_get_mode_info(uint16_t mode, uint32_t* width, uint32_t* height, uint32_t* pitch, uint32_t* addr) {
    outb(VBE_DISPI_INDEX_ID, 0x01);  // Mode number
    outw(VBE_DISPI_DATA, mode);
    
    outb(VBE_DISPI_INDEX_ID, 0x01);  // Read back mode
    uint16_t cur_mode = inw(VBE_DISPI_DATA);
    
    if (cur_mode != mode) {
        return 0;  // Mode not set
    }
    
    outb(VBE_DISPI_INDEX_ID, 0x02);  // X resolution
    *width = inw(VBE_DISPI_DATA);
    
    outb(VBE_DISPI_INDEX_ID, 0x03);  // Y resolution
    *height = inw(VBE_DISPI_DATA);
    
    outb(VBE_DISPI_INDEX_ID, 0x04);  // BPP
    uint16_t bpp = inw(VBE_DISPI_DATA);
    
    *pitch = (*width * bpp) / 8;
    
    // Linear framebuffer address for VBE 3.0+
    *addr = 0xE0000000;
    
    return 1;
}

// Port I/O helpers
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outw(uint16_t port, uint16_t val) {
    __asm__ volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

// VGA text mode output helper
static void vga_write_text(const char* str, int row, int col, uint8_t color) {
    volatile uint16_t* vga = (volatile uint16_t*)0xB8000;
    
    if (row < 0) row = 0;
    if (row >= 25) row = 24;
    
    for (int i = 0; str[i]; i++) {
        int c = col + i;
        if (c >= 80) break;
        vga[row * 80 + c] = (uint16_t)str[i] | ((uint16_t)color << 8);
    }
}

static void vga_clear_line(int row) {
    volatile uint16_t* vga = (volatile uint16_t*)0xB8000;
    for (int i = 0; i < 80; i++) {
        vga[row * 80 + i] = 0x0A20;
    }
}

static void vga_clear_screen() {
    volatile uint16_t* vga = (volatile uint16_t*)0xB8000;
    for (int i = 0; i < 80 * 25; i++) {
        vga[i] = 0x0A20;
    }
}

// Port I/O wrapper
static void outportb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static uint8_t inportb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void kernel_main(multiboot_info_t* mb_info) {
    // Initialize heap first
    heap_init();
    
    // Initialize serial port for debug output
    serial_init();
    serial_write("FLUX-OS Kernel Starting...\n");
    
    // Clear screen to known state
    vga_clear_screen();
    
    // Display startup message
    vga_write_text("FLUX-OS Kernel Starting...", 0, 0, 0x0A);
    serial_write("FLUX-OS Kernel Starting...\n");
    
    // Initialize graphics
    int graphics_initialized = 0;
    int graphics_available = 0;
    
    vga_write_text("Initializing graphics...", 1, 0, 0x0A);
    serial_write("Initializing graphics...\n");
    
    // Try VBE mode 1024x768x32 first (standard mode)
    uint32_t fb_addr = 0;
    uint32_t fb_width = 0;
    uint32_t fb_height = 0;
    uint32_t fb_pitch = 0;
    
    // Method 1: Try QEMU/VBox VBE
    if (vbe_available()) {
        vga_write_text("VBE BIOS detected!", 2, 0, 0x0A);
        serial_write("VBE BIOS detected!\n");
        
        // Try mode 0x0118 (1024x768x32)
        vbe_set_mode(0x0118);
        if (vbe_get_mode_info(0x0118, &fb_width, &fb_height, &fb_pitch, &fb_addr)) {
            vga_write_text("VBE Mode 1024x768x32 OK", 3, 0, 0x0A);
            serial_write("VBE Mode 1024x768x32 OK\n");
            graphics_initialized = 1;
        }
        // Try 800x600x32 as fallback
        else if (vbe_get_mode_info(0x0115, &fb_width, &fb_height, &fb_pitch, &fb_addr)) {
            vga_write_text("VBE Mode 800x600x32 OK", 3, 0, 0x0A);
            serial_write("VBE Mode 800x600x32 OK\n");
            graphics_initialized = 1;
        }
        // Try 640x480x32 as last resort
        else if (vbe_get_mode_info(0x0110, &fb_width, &fb_height, &fb_pitch, &fb_addr)) {
            vga_write_text("VBE Mode 640x480x32 OK", 3, 0, 0x0A);
            serial_write("VBE Mode 640x480x32 OK\n");
            graphics_initialized = 1;
        }
    } else {
        vga_write_text("No VBE BIOS, trying multiboot info...", 2, 0, 0x0B);
        serial_write("No VBE BIOS, trying multiboot info...\n");
    }
    
    // Method 2: Try Multiboot VBE info
    if (!graphics_initialized && mb_info && (mb_info->flags & (1 << 12))) {
        vga_write_text("Multiboot graphics info available", 3, 0, 0x0B);
        
        vbe_mode_info_t* vbe_mode = (vbe_mode_info_t*)mb_info->vbe_mode_info;
        if (vbe_mode && vbe_mode->physbase) {
            fb_width = vbe_mode->width;
            fb_height = vbe_mode->height;
            fb_pitch = vbe_mode->pitch;
            fb_addr = vbe_mode->physbase;
            graphics_initialized = 1;
            vga_write_text("Using VBE framebuffer from GRUB", 4, 0, 0x0A);
        }
    }
    
    // Method 3: Use well-known framebuffer addresses
    if (!graphics_initialized) {
        vga_write_text("Using hardcoded framebuffer", 5, 0, 0x0C);
        
        // Try common framebuffer addresses
        fb_addr = 0xE0000000;  // Common VBE address
        fb_width = 1024;
        fb_height = 768;
        fb_pitch = 1024 * 4;
        graphics_initialized = 1;
    }
    
    // Set up global graphics state
    framebuffer = (uint32_t*)fb_addr;
    screen_width = fb_width;
    screen_height = fb_height;
    pitch = fb_pitch;
    
    // Clear framebuffer
    if (framebuffer) {
        uint32_t* fb = (uint32_t*)framebuffer;
        for (uint32_t i = 0; i < (fb_width * fb_height); i++) {
            fb[i] = 0xFF1a1a1a;  // Dark gray background
        }
    }
    
    // Display graphics info on VGA
    char info_buf[64];
    snprintf(info_buf, sizeof(info_buf), "Framebuffer: 0x%X", (unsigned int)fb_addr);
    vga_write_text(info_buf, 8, 0, 0x07);
    
    snprintf(info_buf, sizeof(info_buf), "Resolution: %dx%d", (int)fb_width, (int)fb_height);
    vga_write_text(info_buf, 9, 0, 0x07);
    
    snprintf(info_buf, sizeof(info_buf), "Pitch: %d bytes", (int)fb_pitch);
    vga_write_text(info_buf, 10, 0, 0x07);
    
    // Initialize GUI
    vga_write_text("Initializing GUI...", 12, 0, 0x0A);
    
    if (graphics_initialized && framebuffer) {
        // Initialize the GUI system
        gui_init(screen_width, screen_height, framebuffer, pitch);
        
        // Create desktop
        gui_create_desktop();
        
        vga_write_text("GUI initialized!", 13, 0, 0x0A);
        vga_write_text("Starting GUI loop...", 14, 0, 0x0A);
        
        // Run the GUI (this will loop indefinitely)
        gui_run();
    } else {
        // Fallback to text mode
        vga_write_text("ERROR: No graphics available!", 13, 0, 0x0C);
        vga_write_text("Running in text mode only", 14, 0, 0x07);
    }
    
    // Display message on framebuffer if available
    if (framebuffer && graphics_initialized) {
        // Draw welcome message
        draw_string(100, 100, "FLUX-OS", COLOR_WHITE, 0xFF1a1a1a);
        draw_string(100, 120, "Visual Operating System", COLOR_CYAN, 0xFF1a1a1a);
        draw_string(100, 140, "GUI Loaded Successfully!", COLOR_GREEN, 0xFF1a1a1a);
    }
    
    // Infinite loop
    while (1) {
        __asm__("hlt");
    }
}

