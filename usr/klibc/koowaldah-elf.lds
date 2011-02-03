ENTRY (_start)

SECTIONS
{
	.text :
	{
		*(.text)
	}

	.rodata :
	{
		*(.rodata)
	}

	.data ALIGN(0x1000) :
	{
		*(.data)
	}

	.bss :
	{
		__bss_start = .;
		*(COMMON)
		*(.bss)
		_bss_end = .;
		__bss_end = .;
	}
}

