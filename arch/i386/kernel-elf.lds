OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
ENTRY (multiboot_start)

SECTIONS
{
	/* The kernel starts from 1 MB. */
	. = 0x100000;

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
		late_init_start = .;
		*(.init.late)
		late_init_end = .;
		*(.rodata*)
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

	rootfs_start = .;
	.rootfs : { *(.rootfs) }
	rootfs_end = .;
	kernel_end = ALIGN(0x1000);
}

