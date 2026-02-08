#include <stdint.h>

// Graphics library declarations (same as in gfx.h)
typedef struct {
    int x, y;
    int width, height;
    int flags;
    uint32_t bg_color;
    uint32_t border_color;
    const char* title;
} window_t;

extern uint32_t* framebuffer;
extern int screen_width;
extern int screen_height;

// Graphics function declarations
extern void set_pixel(int x, int y, uint32_t color);
extern void fill_rect(int x, int y, int width, int height, uint32_t color);
extern void draw_rect(int x, int y, int width, int height, uint32_t color);
extern void draw_line(int x1, int y1, int x2, int y2, uint32_t color);
extern void draw_char(int x, int y, char c, uint32_t fg, uint32_t bg);
extern void draw_string(int x, int y, const char* str, uint32_t fg, uint32_t bg);
extern void draw_window(window_t* win);
extern void draw_taskbar(uint32_t color);
extern void draw_wallpaper();
extern void clear_screen(uint32_t color);

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

// Standard VGA text buffer (backup)
volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;

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

// VBE Mode Info Structure (simplified)
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
    uint32_t physbase; // THE FRAMEBUFFER ADDRESS
} __attribute__((packed)) vbe_mode_info_t;

void kernel_main(multiboot_info_t* mb_info) {
    volatile uint16_t* vga = (uint16_t*)0xB8000;
    
    // Clear screen
    for (int i = 0; i < 80 * 25; i++) {
        vga[i] = 0x0720;  // Space, white on black
    }
    
    // Save original pointer before we modify it
    uint32_t ptr = (uint32_t)mb_info;
    uint32_t ptr_copy = ptr;  // Keep a copy for the final check
    
    // Line 0: Confirm kernel loaded
    int row = 0, col = 0;
    const char* line = "FLUX-OS: Kernel running!";
    for (int i = 0; line[i]; i++) {
        vga[row * 80 + col + i] = (uint16_t)line[i] | ((uint16_t)0x0A << 8);
    }
    
    // Line 1: Show pointer as decimal
    row = 1;
    col = 0;
    const char* ptr_label = "mb_info pointer (decimal): ";
    for (int i = 0; ptr_label[i]; i++) {
        vga[row * 80 + col + i] = (uint16_t)ptr_label[i] | ((uint16_t)0x0B << 8);
    }
    
    // Convert pointer to decimal string
    char dec_str[12] = "0000000000\0";
    if (ptr == 0) {
        dec_str[0] = '0';
        dec_str[1] = '\0';
    } else {
        int len = 0;
        uint32_t temp = ptr;
        while (temp > 0) {
            len++;
            temp /= 10;
        }
        for (int i = len - 1; i >= 0; i--) {
            dec_str[i] = '0' + (ptr % 10);
            ptr /= 10;
        }
        dec_str[len] = '\0';
    }
    
    // Write decimal
    col = 27; // After label
    for (int i = 0; dec_str[i]; i++) {
        vga[row * 80 + col + i] = (uint16_t)dec_str[i] | ((uint16_t)0x0C << 8);  // Red for pointer
    }
    
    // Line 2: Result - use ptr_copy, not ptr (which was modified)
    row = 2;
    col = 0;
    if (ptr_copy == 0) {
        const char* msg = "Result: mb_info is NULL - MULTIBOOT NOT INITIALIZED";
        for (int i = 0; msg[i]; i++) {
            vga[row * 80 + col + i] = (uint16_t)msg[i] | ((uint16_t)0x0C << 8);  // Red
        }
    } else {
        const char* msg = "Result: mb_info is VALID - Proceeding with graphics!";
        for (int i = 0; msg[i]; i++) {
            vga[row * 80 + col + i] = (uint16_t)msg[i] | ((uint16_t)0x0A << 8);  // Green
        }
    }
    
    // Line 4: Now set up graphics if valid
    if (ptr_copy != 0) {
        // Check multiboot flags to see if graphics info is present
        row = 4;
        col = 0;
        const char* flag_check = "Multiboot flags (hex): ";
        for (int i = 0; flag_check[i]; i++) {
            vga[row * 80 + col + i] = (uint16_t)flag_check[i] | ((uint16_t)0x0B << 8);
        }
        
        // Show flags as hex
        uint32_t flags = mb_info->flags;
        const char hex_digits[] = "0123456789ABCDEF";
        int flag_pos = 23;
        for (int i = 28; i >= 0; i -= 4) {
            vga[row * 80 + flag_pos++] = (uint16_t)hex_digits[(flags >> i) & 0xF] | ((uint16_t)0x0C << 8);
        }
        
        // Check for graphics flag (bit 12)
        row = 5;
        col = 0;
        if (mb_info->flags & (1 << 12)) {
            const char* has_gfx = "Bit 12 SET: Graphics info available!";
            for (int i = 0; has_gfx[i]; i++) {
                vga[row * 80 + col + i] = (uint16_t)has_gfx[i] | ((uint16_t)0x0A << 8);  // Green
            }
        } else {
            const char* no_gfx = "Bit 12 NOT SET: No graphics info in multiboot!";
            for (int i = 0; no_gfx[i]; i++) {
                vga[row * 80 + col + i] = (uint16_t)no_gfx[i] | ((uint16_t)0x0C << 8);  // Red
            }
        }
        
        // Try to access VBE info anyway
        row = 6;
        col = 0;
        const char* vbe_check = "Raw memory at +76: ";
        for (int i = 0; vbe_check[i]; i++) {
            vga[row * 80 + col + i] = (uint16_t)vbe_check[i] | ((uint16_t)0x0B << 8);
        }
        
        // Read 4 bytes at offset 76 from multiboot info
        uint32_t* mb_ptr = (uint32_t*)((uint32_t)mb_info + 76);
        uint32_t vbe_raw = *mb_ptr;
        
        // Show as hex
        const char hex_d[] = "0123456789ABCDEF";
        int pos = 19;
        for (int i = 28; i >= 0; i -= 4) {
            vga[row * 80 + pos++] = (uint16_t)hex_d[(vbe_raw >> i) & 0xF] | ((uint16_t)0x0C << 8);
        }
        
        // Try reading the vbe_mode_info
        row = 7;
        col = 0;
        
        if (vbe_raw == 0) {
            const char* msg = "VBE pointer is NULL!";
            for (int i = 0; msg[i]; i++) {
                vga[row * 80 + col + i] = (uint16_t)msg[i] | ((uint16_t)0x0C << 8);
            }
        // GRUB is in text mode, not graphics mode
        // Use hardcoded QEMU framebuffer that we know works
        row = 7;
        col = 0;
        const char* using_fb = "Using hardcoded QEMU framebuffer...";
        for (int i = 0; using_fb[i]; i++) {
            vga[row * 80 + col + i] = (uint16_t)using_fb[i] | ((uint16_t)0x0A << 8);
        }
        
        // Known working values from testing
        framebuffer = (uint32_t*)0xFD000000;
        screen_width = 1024;
        screen_height = 768;
        uint16_t pitch = 1024 * 4;  // 1024 pixels * 4 bytes per pixel = 4096 bytes per scanline
        
        row = 8;
        col = 0;
        const char* fb_addr = "Framebuffer: 0xFD000000";
        for (int i = 0; fb_addr[i]; i++) {
            vga[row * 80 + col + i] = (uint16_t)fb_addr[i] | ((uint16_t)0x0A << 8);
        }
        
        row = 9;
        col = 0;
        const char* dims = "Resolution: 1024x768";
        for (int i = 0; dims[i]; i++) {
            vga[row * 80 + col + i] = (uint16_t)dims[i] | ((uint16_t)0x0A << 8);
        }
            
            // Try to write one pixel - this should render the full GUI
            if (framebuffer != 0) {
                row = 10;
                col = 0;
                const char* rendering = "Rendering to framebuffer...";
                for (int i = 0; rendering[i]; i++) {
                    vga[row * 80 + col + i] = (uint16_t)rendering[i] | ((uint16_t)0x0A << 8);
                }
                
                // Fill entire screen with a gradient blue color
                uint32_t* row_ptr = framebuffer;
                for (int y = 0; y < screen_height; y++) {
                    uint32_t color = 0xFF110033 + (y << 8);  // Gradient
                    for (int x = 0; x < screen_width; x++) {
                        row_ptr[x] = 0xFF1a4d6d;  // Dark blue
                    }
                    // Move to next row using pitch
                    row_ptr = (uint32_t*)((uint8_t*)row_ptr + pitch);
                }
                
                // Draw wallpaper with proper pitch
                draw_wallpaper();
                
                // Create and render a window
                window_t win = {
                    .x = 50,
                    .y = 50,
                    .width = 300,
                    .height = 200,
                    .flags = 0,
                    .bg_color = 0xFF3366CC,
                    .border_color = 0xFF0000FF,
                    .title = "Flux-OS GUI"
                };
                draw_window(&win);
                
                // Draw text
                draw_string(60, 70, "GUI Successfully Rendered!", 0xFFFFFFFF, 0xFF3366CC);
                draw_taskbar(0xFF1a1a1a);
                
                row = 11;
                col = 0;
                const char* success = "GUI COMPLETE!";
                for (int i = 0; success[i]; i++) {
                    vga[row * 80 + col + i] = (uint16_t)success[i] | ((uint16_t)0x0A << 8);
                }
            } else {
                row = 12;
                col = 0;
                const char* fail = "ERROR: Invalid framebuffer, width, or height!";
                for (int i = 0; fail[i]; i++) {
                    vga[row * 80 + col + i] = (uint16_t)fail[i] | ((uint16_t)0x0C << 8);
                }
            }
        }
    }
    
    // Infinite idle loop
    while (1) {
        __asm__("hlt");
    }
}
