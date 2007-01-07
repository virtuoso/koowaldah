/*
 * kernel/mem_area.c
 *
 * Copyright (C) 2006 Alexander Shishkin
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Koowaldah developers nor the names of their
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE DEVELOPERS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE DEVELOPERS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
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
		mem_area_put(map->m_mma[i], map);

	free_map(map);
}

struct mem_area *mem_area_alloc(struct mapping *map, unsigned long start,
		struct klist0_node *page_list, u32 flags)
{
	struct mem_area *mma;
	struct klist0_node *tmp;

	bug_on(in_kernel(start));
	bug_on(map == &root_map);

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

	klist0_reparent(page_list, &mma->m_plist);
	
	klist0_for_each(tmp, &mma->m_plist) {
		struct page *pg = 
			klist0_entry(tmp, struct page, area_list);
		map_page(map, mma->m_end, 
			(u32) page_to_addr(pg), mma->m_prot);
		__mem_area_add_page(mma, pg);
	}

	return mma;

}

void mem_area_attach(struct mapping *dst, struct mem_area *mma)
{
	struct klist0_node *tmp;
	struct page *pg;

	if (dst->m_nmma >= MMA_MAX) {
		kprintf("Cannot add more mem_areas to this map\n");
		return;
	}
	
	dst->m_mma[dst->m_nmma++] = mma;
	mma->m_users++;

	klist0_for_each(tmp, &mma->m_plist) {
		pg = klist0_entry(tmp, struct page, area_list);

		map_page(dst, pg->virt, (u32)page_to_addr(pg), mma->m_prot);
	}
}

struct mem_area *mem_area_alloc_new(struct mapping *map, unsigned long start,
		u32 pages, u32 flags)
{

	bug_on(pages < 0);

	KLIST0_DECLARE(pg_list);

	while(pages--) {
		struct page *pg = alloc_page(ZONE_USER);
		bug_on(!pg); /* XXX Try to recover here. */
		klist0_append(&pg->area_list, &pg_list);
	}

	return mem_area_alloc(map, start, &pg_list, flags);
}

struct mem_area *mem_area_clone(struct mapping *map, struct mem_area *mma)
{
	struct mem_area *ret;
	struct klist0_node *tmp1, *tmp2;
	struct page *pg1, *pg2;

	ret = mem_area_alloc_new(map, mma->m_start, mma->m_pages,
			mma->m_flags);
	if (!ret)
		return NULL;

	tmp2 = ret->m_plist.next;
	klist0_for_each(tmp1, &mma->m_plist) {
		pg1 = klist0_entry(tmp1, struct page, area_list);
		pg2 = klist0_entry(tmp2, struct page, area_list);

		/* we know that pages in both areas go in the very same order,
		 * so we can copy just like this */
		memory_copy((void *)page_to_addr(pg2),
				(void *)page_to_addr(pg1), PAGE_SIZE);
		tmp2 = tmp2->next;
	}

	return ret;
}

void mem_area_unmap(struct mem_area *mma, struct mapping *map, int free)
{
	struct klist0_node *tmp = mma->m_plist.next;
	struct klist0_node *dtmp;
	struct page *pg;

	while (tmp != &mma->m_plist) {
		pg = klist0_entry(tmp, struct page, area_list);
	
		unmap_page(map, pg->virt);
		dtmp = tmp->next;

		if (free) {
			free_pages(pg);
			pg->virt = NOPAGE_ADDR;
		}
		tmp = dtmp;
	}
	
	slice_free(mma, mma_pool);
}

void mem_area_kill(struct mem_area *mma, struct mapping *map)
{
	struct klist0_node *tmp = mma->m_plist.next;
	struct klist0_node *dtmp;
	struct page *pg;

	while (tmp != &mma->m_plist) {
		pg = klist0_entry(tmp, struct page, area_list);
	
		unmap_page(map, pg->virt);
		pg->virt = NOPAGE_ADDR;
		dtmp = tmp->next;

		free_pages(pg);
		tmp = dtmp;
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

void mem_area_put(struct mem_area *mma, struct mapping *map)
{
	if (!--mma->m_users)
		mem_area_unmap(mma, map, 1);
	else
		mem_area_unmap(mma, map, 0);
}

