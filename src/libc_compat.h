#ifndef LIBC_COMPAT_H
#define LIBC_COMPAT_H

#include <stddef.h>

// Memory allocation
void* malloc(size_t size);
void free(void* ptr);
void* calloc(size_t nmemb, size_t size);
void* realloc(void* ptr, size_t size);

// String functions
size_t strlen(const char* str);
int strcmp(const char* s1, const char* s2);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);

// Formatted output
int snprintf(char* buf, size_t size, const char* fmt, ...);

// VGA text mode helpers
void vga_print(const char* str);
void vga_clear();
void vga_set_position(int r, int c);

// Heap initialization
void heap_init();

#endif // LIBC_COMPAT_H

