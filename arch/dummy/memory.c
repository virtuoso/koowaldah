/*
 * arch/dummy/memory.c
 *
 * Copyright (C) 2007 Alexander Shishkin
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
 */

#include <koowaldah.h>
#include <dummy/loader.h>

void arch_init_boot_zone(struct mem_zone *zone, struct mem_zone *user)
{
	u32 phys_mem, total_mem;

	phys_mem = total_mem = dummy_get_mem_size();
	kprintf("Got %dM (0x%x) bytes of memory\n",
			phys_mem >> 20, total_mem);

	zone->base = NULL;
	zone->total_pages = 0xf0000 >> PAGE_SHIFT;
	kprintf("Total pages: %d\n", zone->total_pages);

	mem_zone_init(zone);

	user->total_pages = (total_mem >> PAGE_SHIFT) - zone->total_pages;
	user->base = (u32 *)((unsigned long)zone->base + (zone->total_pages << PAGE_SHIFT));

	kprintf("Total pages: %d, base: %x\n", user->total_pages, user->base);
	mem_zone_init(user);
}

void paging_init()
{
}

void map_page(struct mapping *map, u32 virt, u32 phys, u16 flags)
{
}

void unmap_page(struct mapping *map, u32 virt)
{
}

void map_pages(struct mapping *map, u32 virt, u32 phys, u32 n, u16 flags)
{
}

void display_map(struct mapping *map)
{
}

struct mapping root_map;

void switch_map(struct mapping *from, struct mapping *to)
{
}

void clone_map(struct mapping *dst, struct mapping *map)
{
}

void free_map(struct mapping *map)
{
}


