cat > linker.ld << 'EOF'
ENTRY(_start)

SECTIONS
{
    . = 1M;
    .text :
    {
        *(.multiboot)
        *(.text)
    }
    .rodata :
    {
        *(.rodata)
    }
    .data :
    {
        *(.data)
    }
    .bss :
    {
        *(.bss)
    }
}
EOF
