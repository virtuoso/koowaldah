ENTRY(kern_start)
SECTIONS
{
	
	.text  0x1000 : {
		*(.text)
	}
  
	.data  : {
		*(.data)
	}

	.bss  :	{ 					
		*(.bss)
		_bss_end = .;    
	}
	
	alloc_mem_start = .;
	. = 0x8FFFF;
	alloc_mem_end = .;
	. += 1;
	page_table = .;
	. += 0x1000;
	page_directory = .;
}
