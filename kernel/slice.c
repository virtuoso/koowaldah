
/*
 * kernel/slice.c
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
#include <klist0.h>
#include <bitmap.h>
#include <mm_zone.h>
#include <page_alloc.h>
#include <slice.h>
#include <bug.h>

#if 0
#define DEBUG
#endif

#include <debug.h>

struct slice_swamp {
	struct mem_zone *zone;
	struct klist0_node slices;
} boot_slice_swamp;

void slice_init()
{
	boot_slice_swamp.zone = &mem_zone[ZONE_BOOT];
	KLIST0_INIT(&boot_slice_swamp.slices);
}

static void slice_page_info(struct page *pg, int obj_size)
{
	unsigned long *bmask;
	char *pg_addr;
	int nobjs = PAGE_SIZE / obj_size;

	pg_addr = (char *) page_to_addr(pg);

	bmask = (unsigned long *) ((pg_addr) + PAGE_SIZE - BITMAP_SIZE(nobjs));

	kprintf("Page: %p, addr: %p\n", pg, pg_addr);
	kprintf("Bitmask (starts at %p, %d bytes long): \n",
		bmask, nobjs / 8);
	bitmap_print(bmask, nobjs);


}

/*
 * Print info about a particular slice pool
 */
void slice_pool_info(struct slice_pool *slice)
{
	struct klist0_node *tmp;
	kprintf("Slice %p:\n", slice);

	kprintf("Object size: 0x%x (%d)\n",
		slice->obj_size, slice->obj_size);
	kprintf("Objects per page (including used for bitmaping): %d\n",
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

struct slice_pool *slice_pool_create(unsigned int flags, int obj_size)
{
	struct slice_pool *slice;

	if (obj_size > PAGE_SIZE / 2 - sizeof(unsigned long)) {
		/*
		 * Can't allocate slices that big, maybe fall back to the
		 * page allocator, later.
		 */
		bug();
	}

	void *pg = get_page(flags);
	if (!pg)
		return NULL;

	slice = pg;

	DPRINT("Got page to hold the pool, addr = %p, page = %p, order = %d\n",
		pg, addr_to_page(pg), addr_to_page(pg)->private.order);

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
	int bitmap_size;
	int nobjs;


	t = pool->pages_active.next;

	while (t != &pool->pages_active) {
		nobjs = PAGE_SIZE / pool->obj_size;
		pg = klist0_entry(t, struct page, list);
		bmask = (void *)((uintptr_t)page_to_addr(pg) + PAGE_SIZE
				 - BITMAP_SIZE(nobjs));

		bitmap_size = BITMAP_SIZE(nobjs);

		if (bitmap_size > PAGE_SIZE - nobjs * pool->obj_size) {
			/* We need to use some bytes from the
			 * slice pool for the bitmap. */
			int consumed_bytes = bitmap_size -
				(PAGE_SIZE - nobjs * pool->obj_size);

			nobjs -= consumed_bytes / pool->obj_size +
				((consumed_bytes % pool->obj_size)? 1 : 0);
		}

		DPRINT("page = %p, nobjs = %d, bmask = %p\n",
		       page_to_addr(pg), nobjs, bmask);

		if (bitmap_all_unset(bmask, nobjs)) {
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

void *slice_alloc(struct slice_pool *pool)
{
	/* XXX Locking! */

	int nobjs = PAGE_SIZE / pool->obj_size;

	if (klist0_empty(&pool->pages_active)) {

		int bitmap_size;
		unsigned long *bmask;
		void *pg_addr;
		struct page *pg;

		pg = alloc_page(0);
		if (!pg)
			return NULL;
		pg_addr = page_to_addr(pg);
		DPRINT("Allocating a new page: %p\n", pg_addr);

		bitmap_size = BITMAP_SIZE(nobjs);
		DPRINT("Bitmask size: %d\n", bitmap_size);
		DPRINT("nobjs = %d, (%d) longs\n", nobjs,
			BITMAP_SIZE(nobjs) / (sizeof(unsigned long)));

		bmask = (unsigned long *)((uintptr_t)pg_addr + PAGE_SIZE
					  - BITMAP_SIZE(nobjs));

		DPRINT("bmask = %p\n", bmask);

		/* Mark all slices as used. */
		bitmap_fill(bmask, nobjs);
		/* bitmap_print(bmask, 1024); */

		/* If the bitmap does not fit in the tail gap. */
		if (bitmap_size > PAGE_SIZE - nobjs * pool->obj_size) {
			/* We need to use some bytes from the
			 * slice pool for the bitmap. */
			int consumed_bytes = bitmap_size -
				(PAGE_SIZE - nobjs * pool->obj_size);
			DPRINT("consumed bytes = %d\n", consumed_bytes);

			nobjs -= consumed_bytes / pool->obj_size +
				((consumed_bytes % pool->obj_size)? 1 : 0);
		}

		/* Mark all the left slices as free. */
		bitmap_zero(bmask, nobjs);
		DPRINT("nobjs = %d\n", nobjs);
		/* bitmap_print(bmask, 1024); */

		/* use the last avaliable slice to make further allocations
		 * a bit faster.*/
		bitmap_bit_set(bmask, nobjs - 1);

		klist0_append(&pg->list, &pool->pages_active);

		return (char *)pg_addr + (nobjs - 1) * pool->obj_size;
	} else {
		struct page *pg;
		void *pg_addr;
		unsigned long *bmask;
		int number;

		pg = klist0_entry(pool->pages_active.next, struct page, list);
		pg_addr = page_to_addr(pg);
		DPRINT("pg = %p, addr = %p\n", pg, pg_addr);

		bmask = (void *)((uintptr_t)pg_addr + PAGE_SIZE
				 - BITMAP_SIZE(nobjs));

		DPRINT("bmask = %p\n", bmask);
		/* bitmap_print(bmask, nobjs); */

		number = bitmap_seek_unset(bmask, nobjs);
		DPRINT("number = %d\n", number);

		bug_on(number == -1);

		bitmap_bit_set(bmask, number);

		if (bitmap_all_set(bmask, nobjs)) {

			klist0_unlink(&pg->list);
			klist0_append(&pg->list, &pool->pages_inactive);
		}

		return (char *)pg_addr + number * pool->obj_size;
	}

	return NULL;
}

void slice_free(void *slice, struct slice_pool *pool)
{
	void *pg_addr;
	struct page *pg;
	struct klist0_node *tmp;
	int slice_idx;
	int nobjs;
	unsigned long *bmask;

	nobjs = PAGE_SIZE / pool->obj_size;

	DPRINT("Free. slice = %p, pool = %p, nobjs = %d\n",
	       slice, pool, nobjs);

	pg_addr = (void *)((uintptr_t)slice & (~0UL << PAGE_SHIFT));
	DPRINT("page = %p\n", pg_addr);

	klist0_for_each(tmp, &pool->pages_inactive) {
		pg = klist0_entry(tmp, struct page, list);
		if (page_to_addr(pg) == pg_addr) {
			DPRINT("Found slice in pages_inactive\n");
			klist0_unlink(tmp);
			klist0_append(tmp, &pool->pages_active);

			slice_idx = (uintptr_t)slice - (uintptr_t)pg_addr;
			DPRINT("slice in page = 0x%x\n", slice_idx);

			slice_idx = slice_idx / pool->obj_size;

			DPRINT("slice idx = %d\n", slice_idx);

			bmask = (void *)((uintptr_t)pg_addr + PAGE_SIZE -
					 BITMAP_SIZE(nobjs));

			DPRINT("bmask = %p\n", bmask);

			bitmap_bit_clear(bmask, slice_idx);

			return;
		}
	}

	klist0_for_each(tmp, &pool->pages_active) {
		pg = klist0_entry(tmp, struct page, list);
		if (page_to_addr(pg) == pg_addr) {
			DPRINT("Found slice in pages_active\n");

			slice_idx = (uintptr_t)slice - (uintptr_t)pg_addr;
			DPRINT("slice in page = 0x%x\n", slice_idx);

			slice_idx = slice_idx / pool->obj_size;

			DPRINT("slice idx = %d\n", slice_idx);

			bmask = (void *)((uintptr_t)pg_addr + PAGE_SIZE -
					 BITMAP_SIZE(nobjs));

			DPRINT("bmask = %p\n", bmask);

			bitmap_bit_clear(bmask, slice_idx);

			/* Maybe, try to shrink this page. */

			return;
		}
	}

	bug();
}



