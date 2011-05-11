
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

#define ZONE_BOOT 0x0
#define ZONE_USER 0x1

struct mem_zone {
	void *base;
	unsigned long total_pages;
	unsigned long free_pages;
	struct klist0_node alloc_levels[MAX_ORDER];
	struct klist0_node list;
};

struct mem_info {
	struct klist0_node zone_list;
};

void mem_zone_init(struct mem_zone *zone);

#endif /* __MM_ZONE_H__ */
