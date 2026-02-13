/*
 * Flux-OS AArch64 Input Handler
 * Handles USB HID input for Raspberry Pi
 * Uses polling from USB controller or simulated input
 */

#include <stdint.h>
#include "mailbox.h"
#include "fb.h"
#include "timer.h"

/* USB controller registers (B cmps */
#define USB_BASE         0x20980000  /* USB controller base */
#define USB_CONTROL      0x00
#define USB_STATUS       0x04
#define USB_FRAMING      0x08

/* Simple input state */
typedef struct {
    int mouse_x;
    int mouse_y;
    int mouse_buttons;
    int mouse_dx;
    int mouse_dy;
    int keyboard_last_key;
    int keyboard_shift;
    int keyboard_ctrl;
    int keyboard_pressed;
} input_state_t;

static input_state_t input_state = {0};
static int input_initialized = 0;

/* USB OHCI registers */
#define OHCI_BASE        0x20980000
#define OHCI_CMDSTATUS   0x08
#define OHCI_INTERRUPT   0x0C
#define OHCI_HCCA        0x18
#define OHCI_CONTROL     0x04
#define OHCI_RHSTATUS    0x50

/* Read a 32-bit register */
static inline uint32_t read32(uint64_t addr) {
    return *(volatile uint32_t*)addr;
}

/* Write a 32-bit register */
static inline void write32(uint64_t addr, uint32_t val) {
    *(volatile uint32_t*)addr = val;
}

/* USB controller initialization */
int usb_init(void) {
    /* Reset USB controller */
    uint32_t val = read32(OHCI_BASE + OHCI_CMDSTATUS);
    write32(OHCI_BASE + OHCI_CMDSTATUS, val | 0x01);  /* Host controller reset */
    
    /* Wait for reset to complete */
    for (int i = 0; i < 100000; i++) {
        val = read32(OHCI_BASE + OHCI_CMDSTATUS);
        if ((val & 0x01) == 0) break;
    }
    
    /* Enable USB controller */
    val = read32(OHCI_BASE + OHCI_CONTROL);
    val |= 0x01;  /* Controller enable */
    write32(OHCI_BASE + OHCI_CONTROL, val);
    
    return 0;
}

/* Try to read USB keyboard/mouse data */
static int usb_poll_input(void) {
    if (!input_initialized) return 0;
    
    /* Read USB controller interrupt status */
    uint32_t irq_status = read32(OHCI_BASE + OHCI_INTERRUPT);
    
    /* Check for USB peripheral interrupt */
    if (irq_status & 0x04000000) {  /* USB interrupt */
        /* Read HCCA to get ED/TD info */
        uint32_t hcca = read32(OHCI_BASE + OHCI_HCCA);
        
        /* This is a simplified check - real USB handling would parse
         * the HCCA and process USB HID reports */
        
        /* Clear interrupt */
        write32(OHCI_BASE + OHCI_INTERRUPT, irq_status);
    }
    
    return 0;
}

/* Initialize input system */
void input_init(int screen_width, int screen_height) {
    /* Initialize input state */
    input_state.mouse_x = screen_width / 2;
    input_state.mouse_y = screen_height / 2;
    input_state.mouse_buttons = 0;
    input_state.mouse_dx = 0;
    input_state.mouse_dy = 0;
    input_state.keyboard_last_key = 0;
    input_state.keyboard_shift = 0;
    input_state.keyboard_ctrl = 0;
    input_state.keyboard_pressed = 0;
    
    /* Try to initialize USB */
    usb_init();
    
    input_initialized = 1;
}

/* Get mouse position */
void input_get_mouse(int* x, int* y) {
    *x = input_state.mouse_x;
    *y = input_state.mouse_y;
}

/* Get mouse buttons */
int input_get_mouse_buttons(void) {
    return input_state.mouse_buttons;
}

/* Get mouse delta movement */
void input_get_mouse_delta(int* dx, int* dy) {
    *dx = input_state.mouse_dx;
    *dy = input_state.mouse_dy;
    input_state.mouse_dx = 0;
    input_state.mouse_dy = 0;
}

/* Set mouse position */
void input_set_mouse(int x, int y) {
    input_state.mouse_x = x;
    input_state.mouse_y = y;
}

/* Update mouse from USB HID report */
void input_update_mouse(int dx, int dy, int buttons) {
    input_state.mouse_dx = dx;
    input_state.mouse_dy = dy;
    input_state.mouse_x += dx;
    input_state.mouse_y += dy;
    input_state.mouse_buttons = buttons;
    
    /* Clamp to screen bounds */
    if (input_state.mouse_x < 0) input_state.mouse_x = 0;
    if (input_state.mouse_y < 0) input_state.mouse_y = 0;
}

/* Get keyboard state */
int input_get_keyboard_key(void) {
    int key = input_state.keyboard_last_key;
    input_state.keyboard_last_key = 0;
    return key;
}

int input_get_keyboard_pressed(void) {
    return input_state.keyboard_pressed;
}

int input_get_keyboard_shift(void) {
    return input_state.keyboard_shift;
}

int input_get_keyboard_ctrl(void) {
    return input_state.keyboard_ctrl;
}

/* Update keyboard from USB HID report */
void input_update_keyboard(int key, int pressed, int shift, int ctrl) {
    input_state.keyboard_last_key = key;
    input_state.keyboard_pressed = pressed;
    input_state.keyboard_shift = shift;
    input_state.keyboard_ctrl = ctrl;
}

/* Check for USB connected devices (simplified) */
int usb_device_connected(void) {
    uint32_t rhstatus = read32(OHCI_BASE + OHCI_RHSTATUS);
    return (rhstatus & 0x01) != 0;  /* Device present */
}

