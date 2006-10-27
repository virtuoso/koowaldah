
/*
 * kernel/page_alloc.c
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

static inline int idx_order(u32 index)
{
	int i;
	
	for (i = 0; !(index & 1) && i < MAX_ORDER - 1; i++, index = index >> 1);

	return i;
}


extern struct mem_zone boot_zone;

struct mem_info global_mem_info;

void print_alloc_info()
{
	int i;
	int total = 0;

	for(i = 0; i < MAX_ORDER; i++) {
		int j = 0;
		
		struct klist0_node * tmp;
		struct page * pg;
		
		kprintf("Alloc level %d:", i);
		klist0_for_each(tmp, &boot_zone.alloc_levels[i]) {
			pg = klist0_entry(tmp, struct page, list);
			kprintf(" [0x%x - 0x%x(lvl %d)]",
				pg->index * PAGE_SIZE,
				(pg->index + (1 << i) - 1) * PAGE_SIZE,
				idx_order(pg->index));

			if (pg->private.order != i) {
				bug();
			}

			if (idx_order(pg->index) < i){
				bug();
			}

			j++;
		}

		total += j * (1 << i);

		kprintf("(%d piles)\n", j);
	}
	kprintf("Total %d pages\n", total);
}


void mem_zone_init(struct mem_zone * zone)
{
	u32 i;
	u32 consumed_pages;
	struct page * p_pool = (struct page *) zone->base;

	/* Init a struct page for each page in the zone. 
	 * A number of pages is consumed to hold the array
	 * of struct page's. */
	for (i = 0; i < zone->total_pages; i++) {
		KLIST0_INIT(&p_pool[i].list);
		p_pool[i].private.order = 0;
		p_pool[i].zone = zone;
		p_pool[i].index = i;
	}

	for (i = 0; i < MAX_ORDER; i++) {
		KLIST0_INIT(&zone->alloc_levels[i]);
	}

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
	
	klist0_append(&zone->list, &global_mem_info.zone_list);
	
#if DEBUG
	print_alloc_info();
#endif
	
}

struct page * __alloc_pages(u32 flags, struct mem_zone * zone, u32 order)
{
	int i = order;
	struct page * pile;
	
	do { /* Find an non-empty level with sufficiently big piles in it. */
		if (i >= MAX_ORDER)
			return NULL;
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
	DPRINT("Returning %d pages starting from 0x%x (struct page = 0x%x)\n",
		1 << order, pile->index * PAGE_SIZE, (u32) pile);

	return pile;
}
/*
 * Allocate 2^order pages.
 */
inline struct page * alloc_pages(u32 flags, u32 order)
{
	return __alloc_pages(flags, &boot_zone, order);
}

u32 * page_to_addr(struct page * page)
{
	return (u32 *) (((char *) page->zone->base) + page->index * PAGE_SIZE);
}


struct page * addr_to_page(u32 * addr)
{
	struct klist0_node * tmp;
	struct mem_zone * zone;
	/*
	 * Traverse the global memory zone list to find a zone from which this
	 * page was allocated.
	 */
	klist0_for_each(tmp, &global_mem_info.zone_list) {
		zone = klist0_entry(tmp, struct mem_zone, list);
		if (addr > zone->base &&
			((char *)addr) < (((char *)zone->base) + zone->total_pages * PAGE_SIZE)) {

			return (struct page *) ((char *)zone->base) + 
				((((char *)addr) - ((char *)zone->base)) / PAGE_SIZE);
			}
	}

	return NULL;
}

inline u32 * get_pages(u32 flags, u32 order)
{
	return page_to_addr(alloc_pages(flags, order));
}

inline void put_pages(void * addr)
{
	free_pages(addr_to_page(addr));
}


void free_pages(struct page * pg)
{
	struct mem_zone * zone = pg->zone;
	struct page * neighbour;
	struct klist0_node * tmp;
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
}

