SECTIONS
{
    .text :
    {
        . = 0;
        KEEP(*(.isr_vector))
        . = 0x3FC;
        KEEP(*(.run_options))
        . = 0x410;
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
        . = ALIGN(4);
        _etext = .;
        _sidata = _etext;
    } >FLASH =0xff

    .data : AT(_sidata)
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data.*)
        . = ALIGN(4);
        _edata = .;
    } >RAM =0

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >RAM

    /* Added for arm-none-eabi-gcc's sake. */
    /DISCARD/ :
    {
        *(.ARM.exidx*)
    }

    /* Stack is placed at the end of RAM and is 4-byte aligned.
       ARM's SP register points at top of the stack. */
    _estack = (ORIGIN(RAM) + LENGTH(RAM)) & 0xFFFFFFFC;

    /* Linker will return error if there is not enough space in RAM for stack
       and heap */
    ASSERT(_ebss + _minimum_stack_and_heap_size <= _estack,
           "There is not enough space in RAM for stack and heap.")

    /* Heap's end (see sbrk in libc). */
    PROVIDE(end = _ebss);
    PROVIDE(_end = _ebss);

    /* Now only debugging information is left. */

    /* Remove debugging information for standard libraries. */
    DISCARD :
    {
        libc.a(*)
        libm.a(*)
        libgcc.a(*)
    }

    /* Leave all other debugging information. */
    .stab            0 : {*(.stab)}
    .stabstr         0 : {*(.stabstr)}
    .stab.excl       0 : {*(.stab.excl)}
    .stab.exclstr    0 : {*(.stab.exclstr)}
    .stab.index      0 : {*(.stab.index)}
    .stab.indexstr   0 : {*(.stab.indexstr)}
    .comment         0 : {*(.comment)}
    .debug           0 : {*(.debug)}
    .line            0 : {*(.line)}
    .debug_srcinfo   0 : {*(.debug_srcinfo)}
    .debug_sfnames   0 : {*(.debug_sfnames)}
    .debug_aranges   0 : {*(.debug_aranges)}
    .debug_pubnames  0 : {*(.debug_pubnames)}
    .debug_info      0 : {*(.debug_info .gnu.linkonce.wi.*)}
    .debug_abbrev    0 : {*(.debug_abbrev)}
    .debug_line      0 : {*(.debug_line)}
    .debug_frame     0 : {*(.debug_frame)}
    .debug_str       0 : {*(.debug_str)}
    .debug_loc       0 : {*(.debug_loc)}
    .debug_macinfo   0 : {*(.debug_macinfo)}
    .debug_weaknames 0 : {*(.debug_weaknames)}
    .debug_funcnames 0 : {*(.debug_funcnames)}
    .debug_typenames 0 : {*(.debug_typenames)}
    .debug_varnames  0 : {*(.debug_varnames)}
}
