#include <stdint.h>

volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;

void kernel_main() {
    const char* str = "FLUX-OS v1.0 // READY";
    int color_attribute = 0x0F;
    
    for (int i = 0; i < 2000; i++) {
        vga_buffer[i] = (uint16_t) ' ' | (uint16_t) (color_attribute << 8);
    }

    int i = 0;
    while (str[i] != '\0') {
        vga_buffer[i] = (uint16_t) str[i] | (uint16_t) (color_attribute << 8);
        i++;
    }
}
