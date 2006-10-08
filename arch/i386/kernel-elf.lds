OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
ENTRY (multiboot_start)

SECTIONS
{

	. = 0xFFFF;
	kernel_stack = .;
	. = 0x11000;
    	alloc_mem_start = .;
	. = 0x9FFFF;
	alloc_mem_end = .;
	. = 0x100000;

	page_directory = .;
	. += 0x1000;
	page_table = .;
	. += 0x1000;

	.init :
	{
		*(.init)
	}

	.text :
	{
		*(.text)
	}

	.rodata ALIGN (0x1000) :
	{
		*(.rodata)
	}

	.data ALIGN (0x1000) :
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

