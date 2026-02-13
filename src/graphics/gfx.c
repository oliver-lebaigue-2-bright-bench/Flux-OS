#include "gfx.h"
#include "../gui/gui.h"

// Define global framebuffer state here
uint32_t* framebuffer = 0;
int screen_width = 0;
int screen_height = 0;
int pitch = 4096;  // Default pitch for 1024x768x32 (1024 pixels * 4 bytes)

// Current mouse cursor position
static int cursor_x = 0;
static int cursor_y = 0;
static int cursor_visible = 0;
static uint32_t cursor_bg[16][16];  // Save background for cursor

// Initialize graphics with framebuffer parameters (for ARM)
void gfx_init(int width, int height, void* fb, int fb_pitch) {
    screen_width = width;
    screen_height = height;
    framebuffer = (uint32_t*)fb;
    pitch = fb_pitch;
    cursor_x = width / 2;
    cursor_y = height / 2;
}

// Simple 5x7 bitmap font for numbers and letters
static const uint8_t font_data[256][7] = {
    [' '] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['0'] = {0x3E, 0x51, 0x59, 0x45, 0x3E, 0x00, 0x00},
    ['1'] = {0x00, 0x42, 0x7F, 0x40, 0x00, 0x00, 0x00},
    ['2'] = {0x62, 0x51, 0x49, 0x49, 0x46, 0x00, 0x00},
    ['3'] = {0x22, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00},
    ['4'] = {0x18, 0x14, 0x12, 0x7F, 0x10, 0x00, 0x00},
    ['5'] = {0x2F, 0x49, 0x49, 0x49, 0x31, 0x00, 0x00},
    ['6'] = {0x3C, 0x4A, 0x49, 0x49, 0x30, 0x00, 0x00},
    ['7'] = {0x01, 0x01, 0x79, 0x05, 0x03, 0x00, 0x00},
    ['8'] = {0x36, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00},
    ['9'] = {0x06, 0x49, 0x49, 0x49, 0x1E, 0x00, 0x00},
    ['A'] = {0x7E, 0x09, 0x09, 0x09, 0x7E, 0x00, 0x00},
    ['B'] = {0x7F, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00},
    ['C'] = {0x3E, 0x41, 0x41, 0x41, 0x22, 0x00, 0x00},
    ['D'] = {0x7F, 0x41, 0x41, 0x41, 0x3E, 0x00, 0x00},
    ['E'] = {0x7F, 0x49, 0x49, 0x49, 0x41, 0x00, 0x00},
    ['F'] = {0x7F, 0x09, 0x09, 0x09, 0x01, 0x00, 0x00},
    ['G'] = {0x3E, 0x41, 0x49, 0x49, 0x32, 0x00, 0x00},
    ['H'] = {0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00, 0x00},
    ['I'] = {0x41, 0x7F, 0x41, 0x00, 0x00, 0x00, 0x00},
    ['J'] = {0x20, 0x40, 0x41, 0x3F, 0x01, 0x00, 0x00},
    ['K'] = {0x7F, 0x08, 0x14, 0x22, 0x41, 0x00, 0x00},
    ['L'] = {0x7F, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00},
    ['M'] = {0x7F, 0x02, 0x04, 0x02, 0x7F, 0x00, 0x00},
    ['N'] = {0x7F, 0x04, 0x08, 0x10, 0x7F, 0x00, 0x00},
    ['O'] = {0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00, 0x00},
    ['P'] = {0x7F, 0x09, 0x09, 0x09, 0x06, 0x00, 0x00},
    ['Q'] = {0x3E, 0x41, 0x49, 0x21, 0x5E, 0x00, 0x00},
    ['R'] = {0x7F, 0x09, 0x19, 0x29, 0x46, 0x00, 0x00},
    ['S'] = {0x26, 0x49, 0x49, 0x49, 0x32, 0x00, 0x00},
    ['T'] = {0x01, 0x01, 0x7F, 0x01, 0x01, 0x00, 0x00},
    ['U'] = {0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00, 0x00},
    ['V'] = {0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00, 0x00},
    ['W'] = {0x7F, 0x20, 0x10, 0x20, 0x7F, 0x00, 0x00},
    ['X'] = {0x63, 0x14, 0x08, 0x14, 0x63, 0x00, 0x00},
    ['Y'] = {0x07, 0x08, 0x70, 0x08, 0x07, 0x00, 0x00},
    ['Z'] = {0x61, 0x51, 0x49, 0x45, 0x43, 0x00, 0x00},
    ['a'] = {0x20, 0x54, 0x54, 0x54, 0x78, 0x00, 0x00},
    ['b'] = {0x7F, 0x48, 0x44, 0x44, 0x38, 0x00, 0x00},
    ['c'] = {0x38, 0x44, 0x44, 0x44, 0x20, 0x00, 0x00},
    ['d'] = {0x38, 0x44, 0x44, 0x48, 0x7F, 0x00, 0x00},
    ['e'] = {0x38, 0x54, 0x54, 0x54, 0x18, 0x00, 0x00},
    ['f'] = {0x08, 0x7E, 0x09, 0x01, 0x02, 0x00, 0x00},
    ['g'] = {0x0C, 0x52, 0x52, 0x52, 0x3E, 0x00, 0x00},
    ['h'] = {0x7F, 0x08, 0x04, 0x04, 0x78, 0x00, 0x00},
    ['i'] = {0x44, 0x7D, 0x40, 0x00, 0x00, 0x00, 0x00},
    ['j'] = {0x20, 0x40, 0x44, 0x3D, 0x00, 0x00, 0x00},
    ['k'] = {0x7F, 0x10, 0x28, 0x44, 0x00, 0x00, 0x00},
    ['l'] = {0x41, 0x7F, 0x40, 0x00, 0x00, 0x00, 0x00},
    ['m'] = {0x7C, 0x04, 0x78, 0x04, 0x78, 0x00, 0x00},
    ['n'] = {0x7C, 0x08, 0x04, 0x04, 0x78, 0x00, 0x00},
    ['o'] = {0x38, 0x44, 0x44, 0x44, 0x38, 0x00, 0x00},
    ['p'] = {0x7C, 0x14, 0x14, 0x14, 0x08, 0x00, 0x00},
    ['q'] = {0x08, 0x14, 0x14, 0x18, 0x7C, 0x00, 0x00},
    ['r'] = {0x7C, 0x08, 0x04, 0x04, 0x08, 0x00, 0x00},
    ['s'] = {0x48, 0x54, 0x54, 0x54, 0x20, 0x00, 0x00},
    ['t'] = {0x04, 0x3F, 0x44, 0x40, 0x20, 0x00, 0x00},
    ['u'] = {0x3C, 0x40, 0x40, 0x20, 0x7C, 0x00, 0x00},
    ['v'] = {0x1C, 0x20, 0x40, 0x20, 0x1C, 0x00, 0x00},
    ['w'] = {0x3C, 0x40, 0x38, 0x40, 0x3C, 0x00, 0x00},
    ['x'] = {0x44, 0x28, 0x10, 0x28, 0x44, 0x00, 0x00},
    ['y'] = {0x0C, 0x50, 0x50, 0x50, 0x3C, 0x00, 0x00},
    ['z'] = {0x44, 0x64, 0x54, 0x4C, 0x44, 0x00, 0x00},
    [':'] = {0x00, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00},
    ['.'] = {0x00, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00},
    ['-'] = {0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00},
    ['/'] = {0x20, 0x10, 0x08, 0x04, 0x02, 0x00, 0x00},
    ['|'] = {0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x00},
};

void set_pixel(int x, int y, uint32_t color) {
    if (x >= 0 && x < screen_width && y >= 0 && y < screen_height && framebuffer) {
        uint32_t* row = (uint32_t*)((uint8_t*)framebuffer + y * pitch);
        row[x] = color;
    }
}

void fill_rect(int x, int y, int width, int height, uint32_t color) {
    // Clamp to screen bounds
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x + width > screen_width) width = screen_width - x;
    if (y + height > screen_height) height = screen_height - y;
    
    // Fast fill using direct memory writes with pitch
    if (framebuffer) {
        for (int py = y; py < y + height; py++) {
            uint32_t* row = (uint32_t*)((uint8_t*)framebuffer + py * pitch);
            for (int px = x; px < x + width; px++) {
                row[px] = color;
            }
        }
    }
}

void draw_rect(int x, int y, int width, int height, uint32_t color) {
    // Top and bottom lines
    for (int px = x; px < x + width; px++) {
        set_pixel(px, y, color);
        set_pixel(px, y + height - 1, color);
    }
    // Left and right lines
    for (int py = y; py < y + height; py++) {
        set_pixel(x, py, color);
        set_pixel(x + width - 1, py, color);
    }
}

void draw_line(int x1, int y1, int x2, int y2, uint32_t color) {
    // Bresenham's line algorithm - faster than floating point
    int dx = (x2 > x1) ? x2 - x1 : x1 - x2;
    int dy = (y2 > y1) ? y2 - y1 : y1 - y2;
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    int x = x1, y = y1;
    
    while (1) {
        set_pixel(x, y, color);
        
        if (x == x2 && y == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err = err - dy;
            x = x + sx;
        }
        if (e2 < dx) {
            err = err + dx;
            y = y + sy;
        }
    }
}

void draw_char(int x, int y, char c, uint32_t fg, uint32_t bg) {
    if (!framebuffer) return;
    
    const uint8_t* char_data = font_data[(unsigned char)c];
    
    for (int row = 0; row < 7; row++) {
        uint8_t byte = char_data[row];
        uint32_t* fb_row = (uint32_t*)((uint8_t*)framebuffer + (y + row) * pitch) + x;
        
        for (int col = 0; col < 8; col++) {
            fb_row[col] = (byte & (1 << col)) ? fg : bg;
        }
    }
}

void draw_string(int x, int y, const char* str, uint32_t fg, uint32_t bg) {
    if (!str) return;
    
    int x_pos = x;
    while (*str) {
        if (x_pos + 8 > screen_width) {
            // Wrap to next line
            x_pos = x;
            y += 8;
        }
        draw_char(x_pos, y, *str, fg, bg);
        x_pos += 8;
        str++;
    }
}

void clear_screen(uint32_t color) {
    fill_rect(0, 0, screen_width, screen_height, color);
}

void draw_wallpaper() {
    // Simple fast wallpaper with alternating colors (ARGB format)
    uint32_t colors[4] = {0xFF1a4d6d, 0xFF0d3d52, 0xFF2a5f7f, 0xFF1a4d6d};
    
    if (!framebuffer) return;
    
    for (int y = 0; y < screen_height; y++) {
        uint32_t color = colors[(y / 4) % 4];
        uint32_t* row = (uint32_t*)((uint8_t*)framebuffer + y * pitch);
        
        for (int x = 0; x < screen_width; x++) {
            row[x] = color;
        }
    }
}

void draw_window(window_t* win) {
    // Draw background
    fill_rect(win->x, win->y, win->width, win->height, win->bg_color);
    
    // Draw title bar
    fill_rect(win->x, win->y, win->width, 20, COLOR_LIGHT_GRAY);
    
    // Draw border
    draw_rect(win->x, win->y, win->width, win->height, win->border_color);
    
    // Draw title text
    if (win->title) {
        draw_string(win->x + 4, win->y + 6, win->title, COLOR_BLACK, COLOR_LIGHT_GRAY);
    }
}

void draw_taskbar(uint32_t color) {
    int taskbar_height = 30;
    int taskbar_y = screen_height - taskbar_height;
    
    // Draw taskbar background
    fill_rect(0, taskbar_y, screen_width, taskbar_height, color);
    
    // Draw taskbar border
    draw_line(0, taskbar_y, screen_width, taskbar_y, COLOR_WHITE);
    
    // Draw logo/text
    draw_string(5, taskbar_y + 8, "FLUX-OS", COLOR_WHITE, color);
}

// 16x16 arrow cursor bitmap
static const uint8_t cursor_bitmap[16][4] = {
    {0x80, 0x00, 0x00, 0x00},  // Row 0
    {0xC0, 0x00, 0x00, 0x00},  // Row 1
    {0xE0, 0x00, 0x00, 0x00},  // Row 2
    {0xF0, 0x00, 0x00, 0x00},  // Row 3
    {0xF8, 0x00, 0x00, 0x00},  // Row 4
    {0xFC, 0x00, 0x00, 0x00},  // Row 5
    {0xFE, 0x00, 0x00, 0x00},  // Row 6
    {0xFF, 0x00, 0x00, 0x00},  // Row 7
    {0xFC, 0x00, 0x00, 0x00},  // Row 8
    {0xF8, 0x00, 0x00, 0x00},  // Row 9
    {0xF0, 0x80, 0x00, 0x00},  // Row 10
    {0xE0, 0xC0, 0x00, 0x00},  // Row 11
    {0xC0, 0xE0, 0x00, 0x00},  // Row 12
    {0x80, 0xF0, 0x00, 0x00},  // Row 13
    {0x00, 0xF8, 0x00, 0x00},  // Row 14
    {0x00, 0xF0, 0x80, 0x00},  // Row 15
};

// Mask for cursor (transparency)
static const uint8_t cursor_mask[16][4] = {
    {0x80, 0x00, 0x00, 0x00},
    {0xC0, 0x00, 0x00, 0x00},
    {0xE0, 0x00, 0x00, 0x00},
    {0xF0, 0x00, 0x00, 0x00},
    {0xF8, 0x00, 0x00, 0x00},
    {0xFC, 0x00, 0x00, 0x00},
    {0xFE, 0x00, 0x00, 0x00},
    {0xFF, 0x00, 0x00, 0x00},
    {0xFC, 0x00, 0x00, 0x00},
    {0xF8, 0x00, 0x00, 0x00},
    {0xF0, 0x80, 0x00, 0x00},
    {0xE0, 0xC0, 0x00, 0x00},
    {0xC0, 0xE0, 0x00, 0x00},
    {0x80, 0xF0, 0x00, 0x00},
    {0x00, 0xF8, 0x00, 0x00},
    {0x00, 0xF0, 0x80, 0x00},
};

// Draw mouse cursor at position
void draw_mouse_cursor(int x, int y) {
    if (!framebuffer) return;
    
    // Save background first if visible
    if (!cursor_visible) {
        for (int py = 0; py < 16; py++) {
            if (y + py >= 0 && y + py < screen_height) {
                for (int px = 0; px < 16; px++) {
                    if (x + px >= 0 && x + px < screen_width) {
                        uint32_t* pixel = (uint32_t*)((uint8_t*)framebuffer + (y + py) * pitch) + (x + px);
                        cursor_bg[py][px] = *pixel;
                    }
                }
            }
        }
    }
    
    // Draw cursor
    for (int py = 0; py < 16; py++) {
        if (y + py >= 0 && y + py < screen_height) {
            for (int px = 0; px < 16; px++) {
                if (x + px >= 0 && x + px < screen_width) {
                    // Check mask bit
                    int byte_idx = px / 8;
                    int bit_idx = 7 - (px % 8);
                    
                    if (cursor_mask[py][byte_idx] & (1 << bit_idx)) {
                        uint32_t* pixel = (uint32_t*)((uint8_t*)framebuffer + (y + py) * pitch) + (x + px);
                        // Cursor color: white with black outline
                        if (cursor_bitmap[py][byte_idx] & (1 << bit_idx)) {
                            *pixel = COLOR_BLACK;
                        } else {
                            *pixel = COLOR_WHITE;
                        }
                    }
                }
            }
        }
    }
    
    cursor_x = x;
    cursor_y = y;
    cursor_visible = 1;
}

// Hide mouse cursor
void hide_mouse_cursor(void) {
    if (!framebuffer || !cursor_visible) return;
    
    // Restore background
    for (int py = 0; py < 16; py++) {
        if (cursor_y + py >= 0 && cursor_y + py < screen_height) {
            for (int px = 0; px < 16; px++) {
                if (cursor_x + px >= 0 && cursor_x + px < screen_width) {
                    uint32_t* pixel = (uint32_t*)((uint8_t*)framebuffer + (cursor_y + py) * pitch) + (cursor_x + px);
                    *pixel = cursor_bg[py][px];
                }
            }
        }
    }
    
    cursor_visible = 0;
}

// Show mouse cursor
void show_mouse_cursor(void) {
    if (framebuffer && !cursor_visible) {
        draw_mouse_cursor(cursor_x, cursor_y);
    }
}
