/* Linker script for Kinetis MK22FN1M0VLK12, 1024 KiB FLASH, 128 KiB RAM */

MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 1024K
    RAM (rwx) : ORIGIN = 0x1FFF0000, LENGTH = 128K
}

/* Linker will return error if there is not enough space in RAM for stack
   and heap */
_minimum_stack_and_heap_size = 81920;

INCLUDE cortex-m.lds
