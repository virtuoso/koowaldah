
/*
 * arch/i386/paging.c
 *
 * Copyright (C) 2005 Alexey Zaytsev
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Koowaldah developers nor the names of theyr 
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
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


