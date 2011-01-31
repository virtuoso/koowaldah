
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

extern void arch_init_boot_zone(struct mem_zone *zone, struct mem_zone *user);
void __init mma_init();

void slice_init(void);

struct mem_zone boot_zone;
struct mem_zone user_zone;

struct mem_info memory_info;

void __init init_mem_info()
{
	KLIST0_INIT(&memory_info.zone_list);
	KLIST0_INIT(&boot_zone.list);
	KLIST0_INIT(&user_zone.list);

	arch_init_boot_zone(&boot_zone, &user_zone);

	slice_init();

	klist0_append(&boot_zone.list, &memory_info.zone_list);
	klist0_append(&user_zone.list, &memory_info.zone_list);

	mma_init();
}

