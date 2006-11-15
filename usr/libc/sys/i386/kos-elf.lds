OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
ENTRY (_start)

SECTIONS
{
	/* we'll assume for now that our userspace loads at 1GB address */
	. = 0x40000000;
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

