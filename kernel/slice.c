
/*
 * kernel/slice.c
 *
 * Copyright (C) 2006 Alexey Zaytsev
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
 * 3. Neither the name of the Koowaldah developers nor the names of theyr 
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 */

#include <koowaldah.h>
#include <klist0.h>
#include <bitmask.h>
#include <mm_zone.h>
#include <page_alloc.h>
#include <slice.h>
#include <bug.h>

#if 0
#define DEBUG
#endif

#include <debug.h>


extern struct mem_zone boot_zone;

struct slice_swamp {
	struct mem_zone *zone;
	struct klist0_node slices;
} boot_slice_swamp;

void slice_init()
{
	boot_slice_swamp.zone = &boot_zone;
	KLIST0_INIT(&boot_slice_swamp.slices);
}

static void slice_page_info(struct page *pg, int obj_size)
{
	unsigned long *bmask;
	u32 *pg_addr;
	int nobjs = PAGE_SIZE / obj_size;
	
	pg_addr = page_to_addr(pg);

	bmask = (unsigned long *) (((char *)pg_addr) + PAGE_SIZE -
		BITS_TO_LONGS(nobjs) * sizeof(unsigned long));

	kprintf("Page: 0x%x, addr: 0x%x\n", (u32) pg, (u32) pg_addr);
	kprintf("Bitmask (starts at 0x%x, %d bytes long): \n",
		(u32) bmask, nobjs / 8);
	bitmask_print(bmask, nobjs);
	
	
}

/*
 * Print info about a particular slice pool
 */
void slice_pool_info(struct slice_pool *slice)
{
	struct klist0_node *tmp;
	kprintf("Slab 0x%x (%d): \n", (u32) slice, (u32) slice);

	kprintf("Object size: 0x%x (%d)\n",
		slice->obj_size, slice->obj_size);
	kprintf("Objects per page (including used for bitmasking): %d\n",
		PAGE_SIZE / slice->obj_size);
	
	kprintf("Inactive pages:\n");
	klist0_for_each(tmp, &slice->pages_inactive) {
		slice_page_info(klist0_entry(tmp, struct page, list),
			slice->obj_size);
	}
	
	kprintf("Active pages:\n");
	klist0_for_each(tmp, &slice->pages_active) {
		slice_page_info(klist0_entry(tmp, struct page, list),
			slice->obj_size);
	}
}

struct slice_pool *slice_pool_create(u32 flags, int obj_size) 
{
	struct slice_pool *slice;

	if (obj_size > PAGE_SIZE / 2 - sizeof(unsigned long)) {
		/* 
		 * Can't allocate slices that big, maybe fall back to the
		 * page allocator, later.
		 */
		bug();
	}

	u32 * pg = get_page(flags);
	if (!pg)
		return NULL;

	slice = (struct slice_pool *) pg;

	DPRINT("Got page to hold the pool, addr = 0x%x, page = 0x%x, order = %d\n",
		(u32) pg, (u32) addr_to_page(pg), addr_to_page(pg)->private.order);

	KLIST0_INIT(&slice->slice_list);
	
	KLIST0_INIT(&slice->pages_inactive);
	KLIST0_INIT(&slice->pages_active);
	
	slice->obj_size = obj_size;
	slice->flags = flags;
	klist0_append(&slice->slice_list, &boot_slice_swamp.slices);

	return slice;
}

void slice_pool_shrink(struct slice_pool *pool)
{
	struct klist0_node *t, *tmp;;
	struct page *pg;
	unsigned long *bmask;
	int bitmask_size;
	int nobjs;
		
	
	t = pool->pages_active.next;

	while (t != &pool->pages_active) {
		nobjs = PAGE_SIZE / pool->obj_size;
		pg = klist0_entry(t, struct page, list);
		bmask = (unsigned long *) (((char *)page_to_addr(pg))
			+ PAGE_SIZE - BITS_TO_LONGS(nobjs)
			* sizeof(unsigned long));
		
		bitmask_size = BITS_TO_LONGS(nobjs) * sizeof(unsigned long);
		
		if (bitmask_size > PAGE_SIZE - nobjs * pool->obj_size) {
			/* We need to use some bytes from the
			 * slice pool for the bitmask. */
			int consumed_bytes = bitmask_size -
				(PAGE_SIZE - nobjs * pool->obj_size);

			nobjs -= consumed_bytes / pool->obj_size + 
				((consumed_bytes % pool->obj_size)? 1 : 0);
		}

		DPRINT("page = 0x%x, nobjs = %d, bmask = 0x%x\n", 
			(u32) page_to_addr(pg), nobjs, (u32) bmask);

		if (bitmask_all_unset(bmask, nobjs)) {
			tmp = t;
			t = t->next;
			klist0_unlink(tmp);
			free_page(pg);
		} else {
			t = t->next;
		}
	}
}

void slice_pool_recycle(struct slice_pool *pool)
{

	if (!klist0_empty(&pool->pages_inactive)){
		slice_pool_info(pool);
		bug();
	}
	
	slice_pool_shrink(pool);

	if (!klist0_empty(&pool->pages_active)){
		slice_pool_info(pool);
		bug();
	}

	put_page(pool);

}

u32 * slice_alloc(struct slice_pool *pool)
{
	/* XXX Locking! */

	int nobjs = PAGE_SIZE / pool->obj_size;

	if (klist0_empty(&pool->pages_active)) {

		int bitmask_size;
		unsigned long *bmask;
		u32 *pg_addr;
		struct page *pg;
		
		pg = alloc_page(0);
		if (!pg)
			return NULL;
		pg_addr = page_to_addr(pg);
		DPRINT("Allocating a new page: 0x%x\n", (u32) pg_addr);

		bitmask_size = BITS_TO_LONGS(nobjs) * sizeof(unsigned long);
		DPRINT("Bitmask size: %d\n", bitmask_size);
		DPRINT("nobjs = %d, (%d) longs\n", nobjs, BITS_TO_LONGS(nobjs));
		
		bmask = (unsigned long *) (((char *)pg_addr) + PAGE_SIZE -
			BITS_TO_LONGS(nobjs) * sizeof(unsigned long));

		DPRINT("bmask = 0x%x\n", (u32) bmask);
		
		/* Mark all slices as used. */
		bitmask_fill(bmask, nobjs);
		/* bitmask_print(bmask, 1024); */

		/* If the bitmask does not fit in the tail gap. */
		if (bitmask_size > PAGE_SIZE - nobjs * pool->obj_size) {
			/* We need to use some bytes from the
			 * slice pool for the bitmask. */
			int consumed_bytes = bitmask_size -
				(PAGE_SIZE - nobjs * pool->obj_size);
			DPRINT("consumed bytes = %d\n", consumed_bytes);

			nobjs -= consumed_bytes / pool->obj_size + 
				((consumed_bytes % pool->obj_size)? 1 : 0);
		}

		/* Mark all the left slices as free. */
		bitmask_zero(bmask, nobjs);
		DPRINT("nobjs = %d\n", nobjs);
		/* bitmask_print(bmask, 1024); */

		/* use the last avaliable slice to make further allocations
		 * a bit faster.*/
		bitmask_bit_set(bmask, nobjs - 1);

		klist0_append(&pg->list, &pool->pages_active);

		return (u32 *) (((char *)pg_addr) + (nobjs - 1) * pool->obj_size);

	} else {
		struct page *pg;
		u32 *pg_addr;
		unsigned long *bmask;
		int number;

		pg = klist0_entry(pool->pages_active.next, struct page, list);
		pg_addr = page_to_addr(pg);
		DPRINT("pg = 0x%x, addr = 0x%x\n", pg, pg_addr);

		bmask = (unsigned long *) (((char *)pg_addr) +
			PAGE_SIZE - BITS_TO_LONGS(nobjs) * sizeof(unsigned long));

		DPRINT("bmask = 0x%x\n", bmask);
		/* bitmask_print(bmask, nobjs); */

		number = bitmask_seek_unset(bmask, nobjs);
		DPRINT("number = %d\n", number);
	
		bug_on(number == -1);

		bitmask_bit_set(bmask, number);

		if (bitmask_all_set(bmask, nobjs)) {
			
			klist0_unlink(&pg->list);
			klist0_append(&pg->list, &pool->pages_inactive);
		}

		return (u32 *) (((char *)pg_addr) + number * pool->obj_size);
	}
	
	return NULL;
}

void slice_free(u32 *slice, struct slice_pool *pool)
{
	u32 *pg_addr;
	struct page *pg;
	struct klist0_node *tmp;
	int slice_idx;
	int nobjs;
	unsigned long *bmask;
	
	nobjs = PAGE_SIZE / pool->obj_size;

	DPRINT("Free. slice = 0x%x, pool = 0x%x, nobjs = %d\n",
		(u32) slice, (u32) pool, nobjs);

	pg_addr = (u32 *) (((unsigned long) slice) & (~0UL << PAGE_SHIFT));
	DPRINT("page = 0x%x\n", (u32) pg_addr);

	klist0_for_each(tmp, &pool->pages_inactive) {
		pg = klist0_entry(tmp, struct page, list);
		if (page_to_addr(pg) == pg_addr) {
			DPRINT("Found slice in pages_inactive\n");
			klist0_unlink(tmp);
			klist0_append(tmp, &pool->pages_active);

			slice_idx = (((char *) slice) - ((char *) pg_addr));
			DPRINT("slice in page = 0x%x\n", slice_idx);

			slice_idx = slice_idx / pool->obj_size;

			DPRINT("slice idx = %d\n", slice_idx);

			bmask = (unsigned long *) (((char *)pg_addr) + PAGE_SIZE -
				BITS_TO_LONGS(nobjs) * sizeof(unsigned long));

			DPRINT("bmask = 0x%x\n", (u32) bmask);

			bitmask_bit_clear(bmask, slice_idx);

			return;
		}
	}

	klist0_for_each(tmp, &pool->pages_active) {
		pg = klist0_entry(tmp, struct page, list);
		if (page_to_addr(pg) == pg_addr) {
			DPRINT("Found slice in pages_active\n");

			slice_idx = (((char *)slice) - ((char *)pg_addr));
			DPRINT("slice in page = 0x%x\n", slice_idx);

			slice_idx = slice_idx / pool->obj_size;

			DPRINT("slice idx = %d\n", slice_idx);

			bmask = (unsigned long *) (((char *)pg_addr) + PAGE_SIZE -
				BITS_TO_LONGS(nobjs) * sizeof(unsigned long));

			DPRINT("bmask = 0x%x\n", (u32) bmask);

			bitmask_bit_clear(bmask, slice_idx);

			/* Maybe, try to shrink this page. */
			
			return;
		}
	}

	bug();
}



