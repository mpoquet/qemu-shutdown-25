SECTIONS
{
   . = 0x80000;
	.text.boot : { *(.text.boot) }
	.text : { *(.text) }
	.rodata : { *(.rodata) }
	.data : { *(.data) }
	. = ALIGN(0x8);
	__bss_start = .;
	.bss : { *(.bss*) } 
	__bss_end = .;
}
