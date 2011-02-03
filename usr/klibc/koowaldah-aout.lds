OUTPUT_FORMAT("a.out-i386-linux", "a.out-i386-linux", "a.out-i386-linux")
ENTRY (_start)
PROVIDE(__stack = 0);

SECTIONS
{
	/* we'll assume for now that our userspace loads at 1GB address */
	. = 0x40000000;
	.text :
	{
		*(.text)
		*(.rodata*)
	}


	/* Should work even with empty .data section */
	. = ALIGN(0x1000);
	.data :
	{
		*(.data)
/*
		*(.comment)
		*(.debug_abbrev)
		*(.debug_info)
		*(.debug_line)
		*(.debug_frame)
		*(.debug_pubnames)
		*(.debug_aranges)
		*(.debug_str)
		*(.debug_loc)
		*(.debug_macinfo)
*/
	}

	.bss :
	{
		__bss_start = .;
		*(COMMON)
		*(.bss)
		__bss_end = .;
		_end = .;
		__end = .;
	}

	/DISCARD/ :
	{
		*(*);
	}

}

