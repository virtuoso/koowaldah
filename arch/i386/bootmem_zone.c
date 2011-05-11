
/*
 * arch/i386/bootmem_zone.c
 *
 * Copyright (C) 2006 Alexey Zaytsev
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
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

void arch_init_boot_zone(struct mem_zone *zone, struct mem_zone *user)
{
	size_t total_mem;
	zone->base = &kernel_end;

	phys_mem =
	total_mem = multiboot_info->mem_upper * 1024;
	/* mem_upper starts at 1Mb. */
	total_mem -= ((uintptr_t)(&kernel_end) - 0x100000);
	phys_mem += 0x100000;

	kprintf("Got %dM (0x%x) bytes of memory\n",
		phys_mem >> 20, total_mem);

	zone->total_pages = (total_mem / PAGE_SIZE) / KERN_ALLOWANCE;
	phys_pgs = phys_mem / PAGE_SIZE;
	kprintf("Total pages: %d, phys_pgs: %d\n", zone->total_pages, phys_pgs);

	mem_zone_init(zone);

	user->total_pages = (total_mem / PAGE_SIZE) - zone->total_pages;
	user->base = (char *)zone->base + zone->total_pages * PAGE_SIZE;
	kprintf("Total pages: %d, base: %x\n", user->total_pages, user->base);
	mem_zone_init(user);
}

