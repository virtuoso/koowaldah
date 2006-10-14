
/*
 * arch/i386/paging.c
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#include <kuca.h>
#include <arch/asm.h> 

extern u8 page_directory;
extern u8 page_table;

int paging_init()
{
#if 0
	u32 address=0;
	int i;

	u32 * page_table_pointer = (u32 *) &page_table;
	u32 * page_directory_pointer = (u32 *) &page_directory;

//	kprintf("page_dir = %x, page_table = %x\n", page_table_pointer, page_directory_pointer);
	
	/* map the first 4 MB */
	for(i=0; i<1024; i++){
		page_table_pointer[i] = address | 3; /* 011 = supervisor level, read/write, present */
		address = address + 4096; 
	}

	/* Fill the first entry in the page dir and mark others as absent */
	page_directory_pointer[0] = ( u32) page_table_pointer | 3;

	for(i=1; i<1024; i++){
		page_directory_pointer[i] = 0 | 2; /* 010 = supervisor level, read/write, not present */
	};

//	kprintf("Enabling paging.\n");
	
	write_cr3((u32) page_directory_pointer);
//	kprintf("write_cr3.\n");
	write_cr0(read_cr0() | 0x80000000);
//	kprintf("write cr0\n");
		
#endif
	return 0;		
}


