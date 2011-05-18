
/*
 * kernel/mm_zone.c
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
#include <bug.h>

extern void arch_init_boot_zone(struct mem_zone *);
void __init mma_init();

void slice_init(void);

struct mem_zone mem_zone[NUM_ZONES];

struct mem_info memory_info;

void __init init_mem_info()
{
	int i;

	KLIST0_INIT(&memory_info.zone_list);
	for (i = 0; i < NUM_ZONES; i++)
		KLIST0_INIT(&mem_zone[i].list);

	arch_init_boot_zone(mem_zone);

	slice_init();

	for (i = 0; i < NUM_ZONES; i++)
		klist0_append(&mem_zone[i].list, &memory_info.zone_list);

	mma_init();
}

