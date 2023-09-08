ENTRY(Reset_Handler)

MEMORY
{
	CCMRAM 		(xrw)	: ORIGIN = 0x10000000,   LENGTH = 64K
	RAM   	 	(xrw) 	: ORIGIN = 0x20000000,   LENGTH = 192K
	FLASH    	(rx)  	: ORIGIN = 0x8000000,   LENGTH = 2048K
}

SECTIONS
{
	.text :
	{
		KEEP(*(.isr_vector))
		*(.text)
		*(.text.*)
		*(.rodata)
		*(.rodata.*)
		_sromdev = .;
		_eromdev = .;
		_sidata = .;
	} >FLASH

	.data : AT(_sidata)
	{
		_sdata = .;
		*(.data)
		*(.data*)
		_edata = .;
	} >RAM

	.bss :
	{
		_sbss = .;
		*(.bss)
		_ebss = .;
	} >RAM

	_estack = ORIGIN(RAM) + LENGTH(RAM);
}