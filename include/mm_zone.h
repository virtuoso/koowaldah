
/*
 * include/mm_zone.h
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
#ifndef __MM_ZONE_H__
#define __MM_ZONE_H__

#include <page_alloc.h>
#include <klist0.h>
#include <mem_area.h>

enum zone_type {
	ZONE_BOOT = 0,
	ZONE_USER,
	ZONE_VMAP,
	NUM_ZONES,
};


struct mem_zone {
	void *base;
	unsigned long total_pages;
	unsigned long free_pages;
	struct klist0_node alloc_levels[MAX_ORDER];
	struct klist0_node list;
};

extern struct mem_zone mem_zone[NUM_ZONES];

struct mem_info {
	struct klist0_node zone_list;
};

void mem_zone_init(struct mem_zone *zone);

#endif /* __MM_ZONE_H__ */
