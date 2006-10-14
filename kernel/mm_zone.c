
/*
 * kernel/mm_zone.c
 *
 * Copyright (C) 2006 Alexey Zaytsev
 * 
 * Is distributed under the GPL 2 license.
 * 
 */

#include <kuca.h>
#include <mm_zone.h>

extern void arch_init_boot_zone(struct mem_zone * zone);
	
struct mem_zone boot_zone;

struct mem_info memory_info;

void init_mem_info ()
{
	KLIST0_INIT(&memory_info.zone_list);
	KLIST0_INIT(&boot_zone.list);

	arch_init_boot_zone(&boot_zone);

	klist0_append(&boot_zone.list, &memory_info.zone_list);
}


