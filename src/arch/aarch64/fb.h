/*
 * Framebuffer Driver Header
 */

#ifndef FB_H
#define FB_H

#include <stdint.h>

/* Include mailbox.h for fb_info_t definition */
#include "mailbox.h"

/* Initialize framebuffer */
int fb_init(void);

/* Get framebuffer info */
void fb_get_info(fb_info_t *fb);

/* Clear framebuffer */
void fb_clear(uint32_t color);

#endif /* FB_H */

