
/*
 * include/page_alloc.h
 *
 * Copyright (C) 2005 Alexey Zaytsev
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

#ifndef __PAGE_ALLOC_H__
#define __PAGE_ALLOC_H__

#include <koowaldah.h>
#include <klist0.h>
#include <lib.h>
#include <arch/pages.h>

#define MAX_ORDER 11

struct mem_zone;

struct page {
	int index;
	unsigned long virt;
	struct klist0_node list;
	struct klist0_node area_list;
	union {
		u32 order;
	} private;
	struct mem_zone *zone;
};

/* mask in flags for enum zone_type */
#define ZONE_MASK	0x3

struct page *alloc_pages(unsigned int flags, int order);
void *get_pages(unsigned int flags, int order);
void *page_to_addr(struct page *page);
struct page *addr_to_page(void *addr);

#define INPAGES(x) \
	(roundup_l2((x), PAGE_SIZE) >> PAGE_SHIFT)

static inline void *get_pages_to_fit(unsigned int flags, size_t size)
{
	return get_pages(flags, log2(INPAGES(size)));
}

#define alloc_page(flags) alloc_pages(flags, 0) 
#define get_page(flags) get_pages(flags, 0)

int alloc_pagelist_sparse(unsigned int flags, size_t npages, uintptr_t *startp,
		       struct klist0_node *pg_list);
int alloc_pagelist_contig(unsigned int flags, size_t npages, uintptr_t *startp,
		       struct klist0_node *pg_list);
void free_pagelist(struct klist0_node *pg_list);

void free_pages(struct page *pg);
void put_pages(void *addr);

#define free_page(page) free_pages(page)
#define put_page(addr) put_pages(addr)

void print_alloc_info();

#endif /* __MM_H__ */

