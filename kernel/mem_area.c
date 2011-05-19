/*
 * kernel/mem_area.c
 *
 * Copyright (C) 2006, 2007 Alexander Shishkin
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
#include <error.h>
#include <bug.h>
#include <lib.h>
#include <klist0.h>
#include <slice.h>
#include <mm_zone.h>

static struct slice_pool *mma_pool;

void __init mma_init()
{
	mma_pool = slice_pool_create(0, sizeof(struct mem_area));
	bug_on(!mma_pool);
}

void kill_user_map(struct mapping *map)
{
	int i = 0;

	while (map->m_mma[i])
		mem_area_put(map->m_mma[i]);

	free_map(map);
}

static void mma_map_pages(struct mem_area *mma, int add)
{
	struct klist0_node *tmp;
	struct mem_area *org;

	org = mma_original(mma);

	klist0_for_each(tmp, &org->m_plist) {
		struct page *pg =
			klist0_entry(tmp, struct page, area_list);
		if (mma->m_flags & MMA_STACK)
			map_page(mma->m_map,
				 add ? mma->m_start - PAGE_SIZE : pg->virt,
				 (uintptr_t)page_to_addr(pg), mma->m_prot);
		else
			map_page(mma->m_map, add ? mma->m_end : pg->virt,
				 (uintptr_t)page_to_addr(pg), mma->m_prot);

		if (add)
			__mem_area_add_page(mma, pg);
	}
}

void mem_area_remap(struct mem_area *mma, unsigned int flags)
{
	mma->m_flags &= ~(MMA_RW|MMA_EXEC);
	mma->m_flags |= flags & (MMA_RW|MMA_EXEC);
	mma->m_prot  = (flags & MMA_RW) ? PTF_RW : 0;
	if (flags & MMA_EXEC)
		mma->m_prot |= PTF_EXEC;

	mma_map_pages(mma, 0);
}

struct mem_area *mem_area_alloc(struct mapping *map, unsigned long start,
		struct klist0_node *page_list, u32 flags)
{
	struct mem_area *mma;

	/*
	 * we currently reuse mmas for in-kernel mappings on mmuless dummy
	 */
	/*bug_on(in_kernel(start));
	bug_on(map == &root_map);*/

	mma = slice_alloc(mma_pool);
	if (!mma)
		return NULL; /* ERRPTR? */

	/* initialize mma structure */
	mma->m_map   = map;
	mma->m_start =
	mma->m_end   = start;
	mma->m_pages = 0;
	mma->m_users = 1;
	mma->m_flags = flags;
	mma->m_prot  = (flags & MMA_RW) ? PTF_RW : 0;
	if (flags & MMA_EXEC)
		mma->m_prot |= PTF_EXEC;

	if (!page_list || klist0_empty(page_list))
		return mma;

	klist0_reparent(page_list, &mma->m_plist);

	mma_map_pages(mma, 1);

	return mma;

}

struct mem_area *mem_area_alloc_new(struct mapping *map, unsigned long start,
		u32 pages, u32 flags)
{
	int ret, zone = ZONE_USER;
	uintptr_t start_phys = 0;

	bug_on(pages < 0);

	if (map == &root_map)
		zone = ZONE_VMAP;

	KLIST0_DECLARE(pg_list);

	if (flags & MMA_CONT)
		ret = alloc_pagelist_contig(zone, pages, &start_phys, &pg_list);
	else
		ret = alloc_pagelist_sparse(zone, pages, &start_phys, &pg_list);

	if (ret)
		return NULL;

	if (zone == ZONE_VMAP)
		start = start_phys;

	return mem_area_alloc(map, start, &pg_list, flags);
}

void mem_area_grow(struct mem_area *mma, u32 pages)
{
	if (!(mma->m_flags & (MMA_GROWS | MMA_STACK)))
		return; /* things like this don't grow */

	if (pages < 0)
		return;

	/* XXX: MMA_CLONE? */
	while(pages--) {
		struct page *pg = alloc_page(ZONE_USER);
		bug_on(!pg); /* XXX Try to recover here. */

		mem_area_add_page(mma, pg);
		map_page(mma->m_map, pg->virt,
				(u32)page_to_addr(pg),
				mma->m_prot);
	}
}

struct mem_area *mem_area_clone(struct mapping *map, struct mem_area *mma)
{
	struct mem_area *ret;

	bug_on(mma->m_flags & (MMA_STACK|MMA_GROWS));

	ret = mem_area_alloc(map, mma->m_start, NULL, mma->m_flags);
	if (!ret)
		return NULL;

	mma->m_users++;
	ret->m_prot = mma->m_prot;
	ret->m_pages = mma->m_pages;
	ret->m_flags |= MMA_CLONE;
	ret->m_original = mma;
	ret->m_sizelim = mma->m_sizelim;

	mma_map_pages(ret, 1);

	return ret;
}

struct mem_area *mem_area_copy(struct mapping *map, struct mem_area *mma)
{
	struct mem_area *ret;
	struct klist0_node *lsrc, *ldst;

	ret = mem_area_alloc_new(map, mma->m_start, mma->m_pages,
				 (mma->m_flags | MMA_RW) & ~MMA_STACK);
	if (!ret)
		return NULL;

	ldst = ret->m_plist.next;
	klist0_for_each(lsrc, &mma->m_plist) {
		struct page *pgsrc, *pgdst;

		bug_on(ldst == &ret->m_plist);

		pgsrc = klist0_entry(lsrc, struct page, area_list);
		pgdst = klist0_entry(ldst, struct page, area_list);
		memory_copy(page_to_addr(pgdst), page_to_addr(pgsrc),
			    PAGE_SIZE);

		ldst = ldst->next;
	}

	ret->m_flags |= mma->m_flags & MMA_STACK;
	ret->m_sizelim = mma->m_sizelim;

	if (!(mma->m_flags & MMA_RW))
		mem_area_remap(ret, mma->m_flags);

	return ret;
}

static void mem_area_unmap(struct mem_area *mma)
{
	struct mem_area *org = mma_original(mma);
	struct klist0_node *tmp = org->m_plist.next;
	struct klist0_node *dtmp;
	struct page *pg;

	while (tmp != &mma->m_plist) {
		pg = klist0_entry(tmp, struct page, area_list);

		unmap_page(mma->m_map, pg->virt);
		dtmp = tmp->next;

		tmp = dtmp;
	}

	slice_free(mma, mma_pool);
}

static void mem_area_kill(struct mem_area *mma)
{
	struct klist0_node *tmp = mma->m_plist.next;
	struct klist0_node *dtmp;
	struct page *pg;

	bug_on(mma->m_flags & MMA_CLONE);

	if (!klist0_empty(&mma->m_plist)) {
		while (tmp != &mma->m_plist) {
			pg = klist0_entry(tmp, struct page, area_list);

			unmap_page(mma->m_map, pg->virt);
			pg->virt = NOPAGE_ADDR;
			dtmp = tmp->next;

			tmp = dtmp;
		}

		if (mma->m_flags & MMA_CONT)
			free_pages(klist0_entry(mma->m_plist.next, struct page,
						area_list));
		else
			free_pagelist(&mma->m_plist);
	}

	slice_free(mma, mma_pool);
}

void mem_area_add_page(struct mem_area *mma, struct page *page)
{
	if (!klist0_empty(&page->area_list))
		bug();

	klist0_prepend(&page->area_list, &mma->m_plist);
	__mem_area_add_page(mma, page);
}

void mem_area_remove_page(struct mem_area *mma, struct page *page)
{
	if (klist0_empty(&page->area_list))
		bug();

	klist0_unlink(&page->area_list);
	__mem_area_remove_page(mma, page);
}

void mem_area_put(struct mem_area *mma)
{
	struct mem_area *org = mma_original(mma);

	if (!--org->m_users)
		mem_area_kill(mma);
	else
		mem_area_unmap(mma);
}

