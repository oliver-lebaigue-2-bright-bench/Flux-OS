/*
 * Flux-OS libc compatibility layer for ARM (AArch64)
 * Minimal implementations for kernel use
 */

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

/* Simple heap allocator for ARM */
/* Use a fixed buffer in DRAM - will be placed by linker */
#define HEAP_START 0x10000000  /* 256MB - in RAM region */
#define HEAP_SIZE  (8 * 1024 * 1024)  /* 8MB heap */

/* Heap management structure */
typedef struct header {
    uint32_t size;
    struct header* next;
    int is_allocated;
} header_t;

static char heap_memory[HEAP_SIZE] __attribute__((aligned(8)));
static header_t* heap_head = (header_t*)heap_memory;
static int heap_initialized = 0;

/* Initialize the heap */
void heap_init() {
    if (heap_initialized) return;
    
    heap_head = (header_t*)heap_memory;
    heap_head->size = HEAP_SIZE - sizeof(header_t);
    heap_head->next = 0;
    heap_head->is_allocated = 0;
    heap_initialized = 1;
}

/* Simple malloc implementation */
void* malloc(size_t size) {
    if (!heap_initialized) heap_init();
    if (size == 0) return 0;
    
    /* Align to 8 bytes */
    size = (size + 7) & ~0x07;
    
    header_t* prev = 0;
    header_t* curr = heap_head;
    
    while (curr) {
        /* Check if this block is free and large enough */
        if (!curr->is_allocated && curr->size >= size) {
            /* Allocate this block */
            curr->is_allocated = 1;
            
            /* Return pointer after header */
            return (void*)((char*)curr + sizeof(header_t));
        }
        
        prev = curr;
        curr = curr->next;
    }
    
    /* No suitable block found - out of memory */
    return 0;
}

/* Simple free implementation */
void free(void* ptr) {
    if (!ptr) return;
    if (!heap_initialized) return;
    
    /* Get header from pointer */
    header_t* header = (header_t*)((char*)ptr - sizeof(header_t));
    
    /* Mark as free */
    header->is_allocated = 0;
    
    /* Simple coalescing: merge with next block if it's also free */
    if (header->next && !header->next->is_allocated) {
        header->size += sizeof(header_t) + header->next->size;
        header->next = header->next->next;
    }
}

/* Calloc: allocate and zero memory */
void* calloc(size_t nmemb, size_t size) {
    void* ptr = malloc(nmemb * size);
    if (ptr) {
        /* Zero the memory */
        char* p = (char*)ptr;
        for (size_t i = 0; i < nmemb * size; i++) {
            p[i] = 0;
        }
    }
    return ptr;
}

/* Realloc: resize an allocated block */
void* realloc(void* ptr, size_t size) {
    if (!ptr) return malloc(size);
    if (size == 0) {
        free(ptr);
        return 0;
    }
    
    /* Get header from pointer */
    header_t* header = (header_t*)((char*)ptr - sizeof(header_t));
    size_t old_size = header->size;
    
    /* Allocate new block */
    void* new_ptr = malloc(size);
    if (new_ptr) {
        /* Copy old data */
        char* src = (char*)ptr;
        char* dst = (char*)new_ptr;
        size_t copy_size = old_size < size ? old_size : size;
        for (size_t i = 0; i < copy_size; i++) {
            dst[i] = src[i];
        }
        free(ptr);
    }
    
    return new_ptr;
}

/* Simple string functions */
size_t strlen(const char* str) {
    size_t len = 0;
    while (str && str[len]) {
        len++;
    }
    return len;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && *s2) {
        if (*s1 != *s2) return *s1 - *s2;
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

char* strcpy(char* dest, const char* src) {
    char* ret = dest;
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
    return ret;
}

char* strncpy(char* dest, const char* src, size_t n) {
    size_t i = 0;
    while (i < n && src[i]) {
        dest[i] = src[i];
        i++;
    }
    while (i < n) {
        dest[i] = '\0';
        i++;
    }
    return dest;
}

/* Integer to string conversion */
static char* itoa_helper(char* buf, unsigned int val, int base) {
    if (base < 2 || base > 36) {
        *buf = '\0';
        return buf;
    }
    
    char* ptr = buf;
    char* ptr1 = buf;
    unsigned int tmp_val;
    
    /* Generate digits in reverse order */
    do {
        tmp_val = val % base;
        val /= base;
        if (tmp_val < 10) {
            *ptr++ = '0' + tmp_val;
        } else {
            *ptr++ = 'A' + (tmp_val - 10);
        }
    } while (val);
    
    *ptr-- = '\0';
    
    /* Reverse the string */
    while (ptr1 < ptr) {
        char tmp = *ptr1;
        *ptr1++ = *ptr--;
        *ptr-- = tmp;
    }
    
    return buf;
}

/* Simple snprintf implementation */
int snprintf(char* buf, size_t size, const char* fmt, ...) {
    if (size == 0) return 0;
    
    va_list args;
    va_start(args, fmt);
    
    char* ptr = buf;
    char* end = buf + size - 1;
    
    while (*fmt && ptr < end) {
        if (*fmt == '%') {
            fmt++;
            
            /* Handle flags */
            int left_align = 0;
            int pad_char = ' ';
            int min_width = 0;
            int precision = -1;
            
            /* Check for left alignment */
            if (*fmt == '-') {
                left_align = 1;
                fmt++;
            }
            
            /* Check for zero padding */
            if (*fmt == '0') {
                pad_char = '0';
                fmt++;
            }
            
            /* Parse width */
            if (*fmt == '*') {
                min_width = va_arg(args, int);
                fmt++;
            } else if (*fmt >= '0' && *fmt <= '9') {
                min_width = 0;
                while (*fmt >= '0' && *fmt <= '9') {
                    min_width = min_width * 10 + (*fmt - '0');
                    fmt++;
                }
            }
            
            /* Parse precision */
            if (*fmt == '.') {
                fmt++;
                precision = 0;
                if (*fmt == '*') {
                    precision = va_arg(args, int);
                    fmt++;
                } else if (*fmt >= '0' && *fmt <= '9') {
                    precision = 0;
                    while (*fmt >= '0' && *fmt <= '9') {
                        precision = precision * 10 + (*fmt - '0');
                        fmt++;
                    }
                }
            }
            
            /* Handle conversion specifiers */
            switch (*fmt) {
                case 'd':
                case 'i': {
                    int val = va_arg(args, int);
                    char num_buf[12];
                    char* num_ptr;
                    
                    if (val < 0) {
                        if (ptr < end) *ptr++ = '-';
                        num_ptr = itoa_helper(num_buf, -(unsigned int)val, 10);
                    } else {
                        num_ptr = itoa_helper(num_buf, (unsigned int)val, 10);
                    }
                    
                    /* Pad and output */
                    int len = strlen(num_ptr);
                    int pad = min_width - len;
                    
                    if (!left_align && pad > 0) {
                        while (pad-- && ptr < end) *ptr++ = pad_char;
                    }
                    
                    while (*num_ptr && ptr < end) *ptr++ = *num_ptr++;
                    
                    if (left_align && pad > 0) {
                        while (pad-- && ptr < end) *ptr++ = pad_char;
                    }
                    break;
                }
                
                case 'u': {
                    unsigned int val = va_arg(args, unsigned int);
                    char num_buf[12];
                    char* num_ptr = itoa_helper(num_buf, val, 10);
                    
                    int len = strlen(num_ptr);
                    int pad = min_width - len;
                    
                    if (!left_align && pad > 0) {
                        while (pad-- && ptr < end) *ptr++ = pad_char;
                    }
                    
                    while (*num_ptr && ptr < end) *ptr++ = *num_ptr++;
                    
                    if (left_align && pad > 0) {
                        while (pad-- && ptr < end) *ptr++ = pad_char;
                    }
                    break;
                }
                
                case 'x':
                case 'X': {
                    unsigned int val = va_arg(args, unsigned int);
                    char num_buf[12];
                    char* num_ptr = itoa_helper(num_buf, val, 16);
                    
                    int len = strlen(num_ptr);
                    int pad = min_width - len;
                    
                    if (!left_align && pad > 0) {
                        while (pad-- && ptr < end) *ptr++ = pad_char;
                    }
                    
                    while (*num_ptr && ptr < end) *ptr++ = *num_ptr++;
                    
                    if (left_align && pad > 0) {
                        while (pad-- && ptr < end) *ptr++ = pad_char;
                    }
                    break;
                }
                
                case 'p': {
                    unsigned long val = (unsigned long)va_arg(args, void*);
                    char num_buf[12];
                    char* num_ptr = itoa_helper(num_buf, val, 16);
                    
                    /* Add 0x prefix */
                    if (ptr < end) *ptr++ = '0';
                    if (ptr < end) *ptr++ = 'x';
                    
                    while (*num_ptr && ptr < end) *ptr++ = *num_ptr++;
                    break;
                }
                
                case 'c': {
                    char val = (char)va_arg(args, int);
                    if (ptr < end) *ptr++ = val;
                    break;
                }
                
                case 's': {
                    const char* val = va_arg(args, const char*);
                    if (!val) val = "(null)";
                    
                    size_t len = strlen(val);
                    if (precision >= 0 && (size_t)precision < len) {
                        len = precision;
                    }
                    
                    int pad = min_width - (int)len;
                    
                    if (!left_align && pad > 0) {
                        while (pad-- && ptr < end) *ptr++ = pad_char;
                    }
                    
                    for (size_t i = 0; i < len && ptr < end; i++) {
                        *ptr++ = val[i];
                    }
                    
                    if (left_align && pad > 0) {
                        while (pad-- && ptr < end) *ptr++ = pad_char;
                    }
                    break;
                }
                
                case '%': {
                    if (ptr < end) *ptr++ = '%';
                    break;
                }
                
                default: {
                    /* Unknown specifier, just output as-is */
                    if (ptr < end) *ptr++ = '%';
                    if (ptr < end) *ptr++ = *fmt;
                    break;
                }
            }
            fmt++;
        } else {
            *ptr++ = *fmt++;
        }
    }
    
    *ptr = '\0';
    va_end(args);
    
    return ptr - buf;
}

/* UART output helper - writes to PL011 UART */
static volatile uint32_t* uart0 = (volatile uint32_t*)0xFE201000;

void vga_print(const char* str) {
    /* Redirect to UART for ARM */
    while (*str) {
        if (*str == '\n') {
            /* Wait for TX FIFO */
            while (uart0[6] & 0x20) { }
            uart0[0] = '\r';
        }
        /* Wait for TX FIFO */
        while (uart0[6] & 0x20) { }
        uart0[0] = (uint32_t)*str;
        str++;
    }
}

void vga_clear(void) {
    /* Nothing to clear on UART */
}

/* Memory barriers for ARM */
static inline void dmb(void) {
    __asm__ volatile ("dmb sy" : : : "memory");
}

static inline void dsb(void) {
    __asm__ volatile ("dsb sy" : : : "memory");
}

