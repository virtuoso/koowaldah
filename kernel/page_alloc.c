/*
 * kernel/page_alloc.c
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
 */

 /*
  * Page allocation is done with a buddy allocator like the one found in Linux.
  * The core structure here is mem_zone, describing a contiguous memory zone,
  * from which pages are allocated. We can allocate 2^order pages at a time with
  * order from 0 to MAX_ORDER, so we can allocate 1, 2, 4, 8 and so on
  * contiguous pages. Mem_zone has MAX_ORDER allocation levels, each holding
  * a list of physically contiguous page piles of size 2^level pages. To reduce
  * fragmentation, each pile's opening page's index (posistion inside the mem zone)
  * is 2^order aligned.
  *
  * When an allocation is requested, we at first look at the level, corresponding to the
  * requested order. If no piles are found there, we look at the next higher level
  * to see if there is a pile of bigger order. If we find a pile there, the pile is divided
  * into two parts. One part is inserted at the lower level and the other is returned
  * and allocation succeeds. If no piles are found on the order+1 level, the
  * operation is repeaden on higher levels until it reaches the last (MAX_ORDER)
  * level. If no piles are found there, the allocation will fail. When freeing a pile,
  * the corresponding level is searched for a "neighbour" - a pile, located next or
  * before the one being freeed. If such pile is found, it is excluded from its level,
  * a new pile or order+1 is formed from those two and the operation is repeated
  * on the next higher level.
  */

#include <koowaldah.h>
#include <textio.h>
#include <mm_zone.h>
#include <page_alloc.h>
#include <bug.h>

#if 0
#define DEBUG
#endif

#include <debug.h>

static int idx_order(int index)
{
	int i;

	for (i = 0; !(index & 1) && i < MAX_ORDER - 1; i++, index >>= 1);

	return i;
}

struct mem_info global_mem_info = {
	.zone_list = KLIST0_EMPTY(global_mem_info.zone_list)
};

/*
 * Print out all allocation levels of a given zone
 */
void print_alloc_info(struct mem_zone *zone)
{
	int i;
	int total = 0;

	for(i = 0; i < MAX_ORDER; i++) {
		int j = 0;

		struct klist0_node *tmp;
		struct page *pg;

		kprintf("Alloc level %d:", i);
		klist0_for_each(tmp, &zone->alloc_levels[i]) {
			pg = klist0_entry(tmp, struct page, list);
			kprintf(" [0x%x - 0x%x(lvl %d)]",
				(uintptr_t)zone->base + pg->index * PAGE_SIZE,
				(uintptr_t)zone->base + (pg->index + (1 << i) - 1) * PAGE_SIZE,
				idx_order(pg->index));

			if (pg->private.order != i)
				bug();

			if (idx_order(pg->index) < i)
				bug();

			j++;
		}

		total += j * (1 << i);

		kprintf("(%d piles)\n", j);
	}
	kprintf("Total %d pages\n", total);
}

/*
 * Initialize a memory zone
 * Note: zone->base and zone->total_pages must be set before calling this
 * @zone -- zone to be initialized
 */
void mem_zone_init(struct mem_zone *zone)
{
	unsigned long i;
	unsigned long consumed_pages;
	struct page *p_pool = (struct page *) zone->base;

#ifdef OPT_CPU_ARCH_DUMMY
	bug_on(zone == &mem_zone[ZONE_USER]);
#endif
	/*
	 * Init a struct page for each page in the zone.
	 * A number of pages is consumed to hold the array
	 * of struct page's.
	 */
	for (i = 0; i < zone->total_pages; i++) {
		KLIST0_INIT(&p_pool[i].list);
		KLIST0_INIT(&p_pool[i].area_list);
		p_pool[i].private.order = 0;
		p_pool[i].zone = zone;
		p_pool[i].index = i;
		p_pool[i].virt = NOPAGE_ADDR;
	}

	for (i = 0; i < MAX_ORDER; i++)
		KLIST0_INIT(&zone->alloc_levels[i]);

	KLIST0_INIT(&zone->list);

	consumed_pages = (sizeof(struct page) * zone->total_pages) / PAGE_SIZE + 1;

	zone->free_pages = zone->total_pages - consumed_pages;

	/*
	 * Insert the pages into the corresponding levels in the mem_zone struct.
	 * A pile may be inserted at a level if a) it's size is at least 2^level and
	 * b) it's opening page index is aligned to at least 2^level.
	 */
	for (i = consumed_pages; i < zone->total_pages;) {
		int pages_left = zone->total_pages - i;
		/* Find out the page index alignment */
		int order = idx_order(i);

		DPRINT("Dealign with page 0x%x, order = %d, left %d pages.\n",
				i, order, pages_left);

		/*
		 * Reduce the order until 2^order becomes not
		 * bigger than the number of pages left unmanaged.
		 */
		while ((1 << order) > pages_left)
			order--;

		DPRINT("Page pile of order %d inserted into te list.\n", order);

		/* We store the order only in the opening page's struct page. */
		p_pool[i].private.order = order;
		klist0_prepend(&p_pool[i].list, &zone->alloc_levels[order]);

		i += (1 << order);
	}

	kprintf("### last page base: %x\n", page_to_addr(&p_pool[zone->total_pages-1]));
	klist0_append(&zone->list, &global_mem_info.zone_list);

#if DEBUG
	print_alloc_info(zone);
#endif
}

/*
 * Allocate a 2^order pages from a given zone.
 */
struct page *__alloc_pages(unsigned int flags, struct mem_zone *zone, int order)
{
	int i = order;
	struct page *pile;

#ifdef OPT_CPU_ARCH_DUMMY
	bug_on(zone == &mem_zone[ZONE_USER]);
#endif

	do { /* Find an non-empty level with sufficiently big piles in it. */
		if (i >= MAX_ORDER) {
			kprintf("Page allocation of order %d failed!\n", order);
			return NULL;
		}
	} while (klist0_empty(&zone->alloc_levels[i++]));
	i = i - 1;

	pile = klist0_entry(zone->alloc_levels[i].next, struct page, list);

	klist0_unlink(zone->alloc_levels[i].next);

	/*
	 * If the pile is too big,  pass the unused  part to the lower levels.
	 */
	while (i-- > order) {
		pile->private.order = i;
		DPRINT("Inserting page with index = 0x%x\n", pile->index);
		klist0_append(&pile->list, &zone->alloc_levels[i]);
		/* Deal with the upper half of the pile */
		pile += (1 << i);
	};

	pile->private.order = order;
	DPRINT("Returning %d pages starting from 0x%x (struct page = %p)\n",
		1 << order, pile->index * PAGE_SIZE, pile);

	return pile;
}

/*
 * Allocate 2^order pages.
 */
__inline struct page *alloc_pages(unsigned int flags, int order)
{
	struct mem_zone *zone = &mem_zone[flags & ZONE_MASK];
	return __alloc_pages(flags, zone, order);
}

/*
 * Obtain page's physical address from page object ptr
 */
void *page_to_addr(struct page *page)
{
	return (void *)((uintptr_t)page->zone->base + page->index * PAGE_SIZE);
}

/*
 * Obtain page object ptr by a page frame address
 */
struct page *addr_to_page(void *addr)
{
	struct klist0_node *tmp;
	struct mem_zone *zone;
	/*
	 * Traverse the global memory zone list to find a zone from which this
	 * page was allocated.
	 */
	klist0_for_each(tmp, &global_mem_info.zone_list) {
		zone = klist0_entry(tmp, struct mem_zone, list);
		if (addr > zone->base &&
		    (uintptr_t)addr < ((uintptr_t)zone->base +
				       zone->total_pages * PAGE_SIZE)) {

			return (struct page *)zone->base +
				((((char *)addr) - ((char *)zone->base)) / PAGE_SIZE);
		}
	}

	return NULL;
}

/*
 * Allocate 2^order pages and return page frame address of the first one.
 */
void *get_pages(unsigned int flags, int order)
{
	return page_to_addr(alloc_pages(flags, order));
}

/*
 * Deallocate a page range by page frame number of its first page
 */
void put_pages(void *addr)
{
	free_pages(addr_to_page(addr));
}

/*
 * Deallocate a page
 */
void free_pages(struct page *pg)
{
	struct mem_zone *zone = pg->zone;
	struct page *neighbour;
	struct klist0_node *tmp;
	int order;

	do {
		order = pg->private.order;
		klist0_for_each(tmp, &zone->alloc_levels[order]) {
			neighbour = klist0_entry(tmp, struct page, list);
			/*
			 * If we have a neighbour on the left, we need it to be at least
			 * order+1 aligned to combine a double pile with it.
			 */
			if (pg - neighbour == (1 << order) && idx_order(neighbour->index) > order) {
				klist0_unlink(tmp);
				pg = neighbour;
				pg->private.order++;
				break;
			}
			/*
			 * If we have a neighbour on the right, we need us to be
			 * order+1 aligned.
			 */
			if (neighbour - pg == (1 << order) && idx_order(pg->index) > order) {
				klist0_unlink(tmp);
				pg->private.order++;
				break;
			}
		}

	} while (pg->private.order < MAX_ORDER && order != pg->private.order);

	klist0_append(&pg->list, &zone->alloc_levels[pg->private.order]);

	if (!klist0_empty(&pg->area_list))
		klist0_unlink(&pg->area_list);
}

