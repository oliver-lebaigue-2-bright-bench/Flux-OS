#ifndef GFX_H
#define GFX_H

#include <stdint.h>
#include <stddef.h>

// Color definitions (32-bit ARGB format)
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
#define COLOR_LIGHT_BLUE 0xFF5a9fd4
#define COLOR_DARK_BLUE 0xFF1a4d6d
#define COLOR_TITLE_BAR 0xFF2a5f7f
#define COLOR_TASKBAR   0xFF1a4d6d
#define COLOR_DESKTOP   0xFF0d3d52
#define COLOR_WINDOW_BG 0xFFE0E0E0
#define COLOR_BUTTON    0xFFD0D0D0
#define COLOR_BUTTON_HOVER 0xFFE8E8E8
#define COLOR_BORDER    0xFF808080

// Graphics initialization (for ARM framebuffer)
void gfx_init(int width, int height, void* fb, int fb_pitch);

// Basic drawing functions
void set_pixel(int x, int y, uint32_t color);
void fill_rect(int x, int y, int width, int height, uint32_t color);
void draw_rect(int x, int y, int width, int height, uint32_t color);
void draw_line(int x1, int y1, int x2, int y2, uint32_t color);
void draw_char(int x, int y, char c, uint32_t fg, uint32_t bg);
void draw_string(int x, int y, const char* str, uint32_t fg, uint32_t bg);
void clear_screen(uint32_t color);

// Mouse cursor functions
void draw_mouse_cursor(int x, int y);
void hide_mouse_cursor(void);
void show_mouse_cursor(void);

// UI drawing functions  
void draw_wallpaper(void);
void draw_taskbar(uint32_t color);

#endif // GFX_H
