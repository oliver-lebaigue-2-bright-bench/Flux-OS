/*
 * Flux-OS AArch64 Input Handler Header
 */

#ifndef AARCH64_INPUT_H
#define AARCH64_INPUT_H

#include <stdint.h>

/* Initialize input system */
void input_init(int screen_width, int screen_height);

/* Mouse functions */
void input_get_mouse(int* x, int* y);
int input_get_mouse_buttons(void);
void input_get_mouse_delta(int* dx, int* dy);
void input_set_mouse(int x, int y);
void input_update_mouse(int dx, int dy, int buttons);

/* Keyboard functions */
int input_get_keyboard_key(void);
int input_get_keyboard_pressed(void);
int input_get_keyboard_shift(void);
int input_get_keyboard_ctrl(void);
void input_update_keyboard(int key, int pressed, int shift, int ctrl);

/* USB functions */
int usb_init(void);
int usb_device_connected(void);

#endif /* AARCH64_INPUT_H */

