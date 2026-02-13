/*
 * Flux-OS AArch64 Bootloader for Raspberry Pi 3/4/500
 * 
 * The Pi firmware loads our kernel at 0x80000 and jumps to _start.
 */

.section .text.boot

.global _start

_start:
    /* Set stack pointer */
    ldr x0, =boot_stack_top
    mov sp, x0
    
    /* Clear bss */
    ldr x0, =__bss_start
    ldr x1, =__bss_end
    cmp x0, x1
    b.eq bss_done

bss_clear:
    str xzr, [x0], #8
    cmp x0, x1
    b.ne bss_clear

bss_done:
    /* Jump to kernel_main */
    b kernel_main

.align 4
boot_stack_bottom:
    .skip 16384      /* 16KB stack */
boot_stack_top:

/*
 * Exception Vector Table
 */
.section .vectors, "ax"
.global vector_table
.align 12

vector_table:
    b vector_table   /* Synchronous */
    b vector_table   /* IRQ */
    b vector_table   /* FIQ */
    b vector_table   /* SError */
    b vector_table   /* Synchronous */
    b vector_table   /* IRQ */
    b vector_table   /* FIQ */
    b vector_table   /* SError */
    b vector_table   /* Synchronous */
    b vector_table   /* IRQ */
    b vector_table   /* FIQ */
    b vector_table   /* SError */
    b vector_table   /* Synchronous */
    b vector_table   /* IRQ */
    b vector_table   /* FIQ */
    b vector_table   /* SError */

.global __bss_start
.global __bss_end
__bss_start:
__bss_end:

