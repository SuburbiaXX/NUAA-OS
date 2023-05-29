ENTRY(start_kernel)
SECTIONS
{
    . = 0xC0010000;
    __text_start = .;
    .text : { cpu/entry.o(.text) *(.text) }
    .rodata : { *(.rodata) }
    __text_end = .;

    . = ALIGN(4096);
    __data_start = .;
    .data : { *(.data) }
    __data_end = .;

    . = ALIGN(4096);
    __bss_start = .;
    .bss : { *(.bss) }
    __bss_end = .;
}
