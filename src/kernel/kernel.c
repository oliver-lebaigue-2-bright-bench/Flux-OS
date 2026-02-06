#include <stdint.h>

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

// Pointer to our framebuffer
uint32_t* framebuffer;

// Variables for screen size
int width;
int height;

void put_pixel(int x, int y, int color) {
    if (x < width && y < height) {
        framebuffer[y * width + x] = color;
    }
}

void kernel_main(multiboot_info_t* mb_info) {
    // 1. Set default (VGA Text) if graphics fail
    const char* str = "FLUX-OS v2.0 // GRAPHICS MODE";
    int color_attribute = 0x0F;
    for (int i = 0; i < 2000; i++) {
        vga_buffer[i] = (uint16_t) ' ' | (uint16_t) (color_attribute << 8);
    }
    int i = 0;
    while (str[i] != '\0') {
        vga_buffer[i] = (uint16_t) str[i] | (uint16_t) (color_attribute << 8);
        i++;
    }

    // 2. Check if GRUB provided VBE info
    if (mb_info->flags & (1 << 12)) { // Bit 12 is Graphics info
        vbe_mode_info_t* vbe_info = (vbe_mode_info_t*)mb_info->vbe_mode_info;
        
        // Set framebuffer address
        framebuffer = (uint32_t*)vbe_info->physbase;
        width = vbe_info->width;
        height = vbe_info->height;
    } else {
        // No graphics info? Stop here.
        return;
    }

    // 3. Draw the screen (Nothing Red: #FF0033)
    int red = 0xFF0033;

    // Fill the screen with red pixels
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            put_pixel(x, y, red);
        }
    }
}
