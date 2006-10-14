
/*
 * arch/i386/bootmem_zone.c
 *
 * Copyright (C) 2006 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#include <kuca.h>
#include <mm_zone.h>

#include "multiboot_info.h"

struct multiboot_info_t * multiboot_info; /* Is set from multiboot.S */
extern u32 kernel_end; /* kerlen-elf.lds */

void arch_init_boot_zone(struct mem_zone * zone)
{

	u32 total_mem;
	zone->base = &kernel_end; 
	
	total_mem = multiboot_info->mem_upper * 1024;
	total_mem -= ((u32)(&kernel_end) - 0x100000); /* mem_upper starts at 1Mb. */

	zone->total_pages = total_mem / PAGE_SIZE;
	
	mem_zone_init(zone);
	
}



