.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bss
.align 16
stack_bottom:
.skip 16384 
stack_top:

.section .text
.global _start
.type _start, @function

_start:
  # Disable interrupts
  cli
  
  # Set up stack - stack grows downward, so top is the highest address
  mov $stack_top, %esp
  
  # Push multiboot info pointer (in EBX) as argument
  # stdcall/cdecl: arguments pushed right-to-left
  push %ebx
  
  # Call C kernel_main
  call kernel_main
  
  # Kernel shouldn't return, but if it does, halt
  cli
hang:
  hlt
  jmp hang

.size _start, . - _start
