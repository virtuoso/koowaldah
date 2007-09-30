OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)

ENTRY(header)

SECTIONS
{
	. = 0x0;

	.init :  {
		*(.init.bs)
		*(.init.hdr)
		*(.init.data)
		*(.init.code)
	}

	.text : {
		*(.init)
		*(.text)
	}

	.data : {
		. = ALIGN(16);
		*(.data)
		. = ALIGN(16);
		*(.rodata*)
	}

	. = 0x90000 + LOADADDR(.data) + SIZEOF(.data);
	.text32 : AT(LOADADDR(.data) + SIZEOF(.data)) {
		*(.text32)

	}

	.data32 : AT(LOADADDR(.text32) + SIZEOF(.text32)) {
		*(.data32)
		LONG(0x5a5aaa55);
	}

	.kernel : AT(0x2000) {
		*(.kernel)
	}
}

