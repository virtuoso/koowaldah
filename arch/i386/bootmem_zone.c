
/*
 * arch/i386/bootmem_zone.c
 *
 * Copyright (C) 2006 Alexey Zaytsev
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

#include <koowaldah.h>
#include <mm_zone.h>
#include <textio.h>

#include "multiboot_info.h"

struct multiboot_info_t * multiboot_info; /* Is set from multiboot.S */
extern u32 kernel_end; /* kerlen-elf.lds */
extern u32 phys_mem;
extern u32 phys_pgs;

void arch_init_boot_zone(struct mem_zone * zone)
{

	u32 total_mem;
	zone->base = &kernel_end; 
	
	phys_mem =
	total_mem = multiboot_info->mem_upper * 1024;
	total_mem -= ((u32)(&kernel_end) - 0x100000); /* mem_upper starts at 1Mb. */
	phys_mem += 0x100000;

	kprintf("Got %d (0x%x) bytes of memory\n",
		total_mem, total_mem);

	zone->total_pages = (total_mem / PAGE_SIZE) / KERN_ALLOWANCE;
	phys_pgs = phys_mem / PAGE_SIZE;
	kprintf("Total pages: %d\n", zone->total_pages);
	
	mem_zone_init(zone);
	
}



