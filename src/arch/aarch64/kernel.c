/*
 * Flux-OS AArch64 Kernel Entry Point
 * Raspberry Pi 3/4/500 support with full WIMP GUI
 */

#include <stdint.h>
#include <stddef.h>

/* Board-specific includes */
#include "mailbox.h"
#include "fb.h"
#include "timer.h"
#include "gic.h"
#include "mmu.h"
#include "input.h"

/* Common GUI includes - gui is declared in gui.h and defined in desktop.c */
#include "../../gui/gui.h"
#include "../../graphics/gfx.h"

/* Architecture-specific definitions */
/* Pi 3/4/500: UART0 at 0xFE201000 */
/* Pi 5: UART0 at 0xFE201000 (same as Pi 4) */
#define UART0_BASE       0xFE201000

/* UART registers */
#define UART_DR          0x00
#define UART_FR          0x18
#define UART_IBRD        0x24
#define UART_FBRD        0x28
#define UART_LCRH        0x2C
#define UART_CR          0x30
#define UART_IMSC        0x38

/* Framebuffer info from mailbox */
fb_info_t fb_info = {0};

/* NOTE: gui_system_t gui is defined in desktop.c */

/* UART output function */
static void uart_putc(char c) {
    volatile uint32_t *uart = (volatile uint32_t *)UART0_BASE;
    
    /* Wait for transmit FIFO to be empty */
    while (uart[UART_FR >> 2] & (1 << 5)) {
        __asm__ volatile ("nop");
    }
    
    /* Write character */
    uart[UART_DR >> 2] = (uint32_t)c;
}

static void uart_write(const char *str) {
    while (*str) {
        if (*str == '\n') {
            uart_putc('\r');
        }
        uart_putc(*str++);
    }
}

static void uart_write_hex(uint64_t val) {
    char buf[17];
    const char *hex = "0123456789ABCDEF";
    buf[16] = '\0';
    for (int i = 15; i >= 0; i--) {
        buf[i] = hex[val & 0xF];
        val >>= 4;
    }
    uart_write(buf);
}

/* Simple delay */
static void delay(volatile uint32_t count) {
    while (count--) {
        __asm__ volatile ("nop");
    }
}

/* Initialize UART0 */
static void uart_init(void) {
    volatile uint32_t *uart = (volatile uint32_t *)UART0_BASE;
    
    /* Disable UART */
    uart[UART_CR >> 2] = 0;
    
    /* Set 115200 baud rate (system clock is 48MHz) */
    uart[UART_IBRD >> 2] = 26;
    uart[UART_FBRD >> 2] = 17;
    
    /* 8-bit word, enable FIFO */
    uart[UART_LCRH >> 2] = (1 << 4) | (1 << 5) | (1 << 6);
    
    /* Enable UART, TX and RX */
    uart[UART_CR >> 2] = (1 << 0) | (1 << 8) | (1 << 9);
}

/* ARM-specific GUI event loop */
void gui_run(void);

void gui_run(void) {
    if (!gui.initialized) return;
    
    event_t event;
    int last_mouse_x = gui.mouse.x;
    int last_mouse_y = gui.mouse.y;
    int last_buttons = 0;
    
    uart_write("Starting GUI event loop...\r\n");
    
    /* Main event loop */
    while (gui.running) {
        /* Poll for input events - in a real implementation, this would 
         * be driven by interrupts from USB controller */
        
        /* For now, simulate some basic input or wait for USB */
        
        /* Small delay to prevent CPU spinning */
        for (volatile int i = 0; i < 100000; i++) {
            __asm__ volatile ("nop");
        }
        
        /* Process queued events */
        while (gui_poll_event(&event)) {
            gui_handle_event(&event);
        }
        
        /* Redraw if needed */
        if (gui.needs_redraw && gui.framebuffer) {
            gui_redraw_all();
        }
    }
    
    uart_write("GUI event loop exited\r\n");
}

/* Main kernel entry */
void kernel_main(void) {
    uart_init();
    
    uart_write("\r\n");
    uart_write("=======================================\r\n");
    uart_write("Flux-OS AArch64 for Raspberry Pi\r\n");
    uart_write("WIMP GUI Edition\r\n");
    uart_write("=======================================\r\n");
    
    /* Initialize memory management unit */
    uart_write("Initializing MMU...\r\n");
    mmu_init();
    
    /* Initialize interrupt controller */
    uart_write("Initializing GIC...\r\n");
    gic_init();
    
    /* Initialize system timer */
    uart_write("Initializing timer...\r\n");
    timer_init();
    
    /* Get framebuffer from mailbox */
    uart_write("Requesting framebuffer...\r\n");
    if (mailbox_get_fb(&fb_info) == 0) {
        uart_write("Framebuffer allocated:\r\n");
        uart_write("  Width: ");
        uart_write_hex(fb_info.width);
        uart_write("\r\n");
        uart_write("  Height: ");
        uart_write_hex(fb_info.height);
        uart_write("\r\n");
        uart_write("  Pitch: ");
        uart_write_hex(fb_info.pitch);
        uart_write("\r\n");
        uart_write("  Base: ");
        uart_write_hex(fb_info.base);
        uart_write("\r\n");
    } else {
        uart_write("Failed to get framebuffer!\r\n");
        /* Hang */
        while (1) { __asm__ volatile ("wfi"); }
    }
    
    /* Initialize input system */
    uart_write("Initializing input system...\r\n");
    input_init(fb_info.width, fb_info.height);
    
    /* Initialize GUI with framebuffer */
    if (fb_info.base != 0) {
        uart_write("Initializing GUI...\r\n");
        gui_init(fb_info.width, fb_info.height, (void *)fb_info.base, fb_info.pitch);
        
        if (gui.initialized) {
            uart_write("Creating desktop...\r\n");
            gui_create_desktop();
            
            uart_write("Kernel initialized!\r\n");
            uart_write("Starting GUI...\r\n");
            
            /* Run GUI */
            gui_run();
        } else {
            uart_write("GUI initialization failed!\r\n");
        }
    }
    
    uart_write("System halted.\r\n");
    
    /* Idle loop */
    while (1) {
        __asm__ volatile ("wfi");
    }
}

