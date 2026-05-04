MEMORY
{
    /* 512K Instruction Memory */
    ROM (rx)  : ORIGIN = 0x00000000, LENGTH = 512K

    /* 64K General Data Memory */
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

SECTIONS
{
    .text : {
        *(.text.init) /* Ensure _start is at the absolute beginning */
        *(.text*)
        *(.rodata*)
        *(.srodata*)
        . = ALIGN(8);
    } > ROM

    /* Initialize data in RAM, but store the permanent copy in ROM */
    .data : {
        . = ALIGN(8);
        _sdata = .;         /* VMA start */

        /* Global Pointer setup for RISC-V linker relaxations */
        PROVIDE( __global_pointer$ = . + 0x800 );

        *(.sdata2*)
        *(.data*)
        *(.sdata*)
        . = ALIGN(8);
        _edata = .;         /* VMA end */
    } > RAM AT > ROM

    /* LMA address for the bootloader to copy from */
    _sidata = LOADADDR(.data);

    .bss : {
        . = ALIGN(8);
        _sbss = .;          /* Start of BSS to zero-out at boot */
        *(.bss*)
        *(.sbss*)
        *(COMMON)
        . = ALIGN(8);
        _ebss = .;          /* End of BSS */
        _end = .;           /* Defines the start of the heap */
    } > RAM
}

/* Define the absolute end of the General Data RAM */
__ram_end = ORIGIN(RAM) + LENGTH(RAM);

/* The stack grows downward from the top of RAM */
__stack_top = __ram_end;

/* Provide a symbol for the absolute maximum heap address */
__heap_limit = __ram_end;