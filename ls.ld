MEMORY
{
	FLASH(rx): ORIGIN = 0, LENGTH = 0x00080000
	RAM(rw): ORIGIN = 0x10000000, LENGTH = 0x00080000
}

SECTIONS
{
	.text :
	{
		__vector_base_addr = .;
		KEEP(*(.isr_vectors))
		KEEP(*(.version))
		. = ALIGN(4);
		__cpp_begin = .;
		KEEP(*(.ctors*))
		KEEP(*(.init_array*))
		KEEP(*(.preinit_array*))
		__cpp_end = .;
		. = ALIGN(4);
		*(.text*)
		*(.rodata*)
	} > FLASH
		
	__etext = LOADADDR(.data);
	
	.data :
	{
		. = ALIGN(4);
		__data_start__ = .;
		*(.data*)
		*(.sdata*)
		. = ALIGN(4);
		__data_end__ = .;
	} > RAM AT > FLASH = 0xff
		
	.bss :
	{
		. = ALIGN(4);
		__bss_start__ = .;
		*(.bss*)
		*(.sbss*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end__ = .;
		__stack_start__ = .;
	} >	RAM
}

__StackTop = (ORIGIN(RAM) + LENGTH(RAM));
__stack_lenght = (__StackTop - __stack_start__);
__stack_lenght /= 4;
__stack_lenght &= 0xfffffffc;
__stack_top3 = __stack_start__ + __stack_lenght;
__stack_top2 = __stack_top3 + __stack_lenght;
__stack_top1 = __stack_top2 + __stack_lenght;
__stack_top0 = __stack_top1 + __stack_lenght;