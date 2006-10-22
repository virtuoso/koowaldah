OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
ENTRY (_start)

SECTIONS
{

	. = 0x4000;
	.text :
	{
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
		*(COMMON)
		*(.bss)
		_bss_end = .;
	}
}

