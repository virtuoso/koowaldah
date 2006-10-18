
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

#include <kuca.h>
#include <mm_zone.h>
#include <page_alloc.h>
#include <bug.h>
#include <textio.h>

/* #define MM_ANAL_DEBUG 1 */
#ifdef OPT_MM_ANAL_DEBUG
# define mm_kprintf(fmt, args ...) kprintf(fmt, ## args)
#else
# define mm_kprintf(fmt, args ...) do {} while (0)
#endif

static inline int idx_order(u32 index)
{
	int i = -1;
	u32 mask = 0x01;

	while(!(index % mask) && i < MAX_ORDER - 1) {
		mask = (mask << 1);
		i++;
	}

	return i;
}


extern struct mem_zone  boot_zone;
void print_alloc_info()
{
	int i;
	int total = 0;

	for(i = 0; i < MAX_ORDER; i++) {
		int j = 0;
		
		struct klist0_node * tmp;
		struct page * pg;
		
		mm_kprintf("Alloc level %d:", i);
		klist0_for_each(tmp, &boot_zone.alloc_levels[i]) {
			pg = klist0_entry(tmp, struct page, list);
			mm_kprintf(" [0x%x - 0x%x(lvl %d)]", pg->index, pg->index + (1 << i) - 1, idx_order(pg->index));

			if (pg->private.order != i) {
				bug();
			}

			if (idx_order(pg->index) < i){
				bug();
			}

			j++;
		}

		total += j * (1 << i);

		mm_kprintf("(%d chunks)\n", j);
	}
	mm_kprintf("Total %d pages\n", total);
}


void mem_zone_init(struct mem_zone * zone)
{
	u32 i;
	u32 consumed_pages;
	struct page * p_pool = (struct page *) zone->base;

	for (i = 0; i < zone->total_pages; i++) {
		KLIST0_INIT(&p_pool[i].list);
		p_pool[i].private.order = 0;
		p_pool[i].zone = zone;
		p_pool[i].index = i;
	}

	for (i = 0; i < MAX_ORDER; i++) {
		KLIST0_INIT(&zone->alloc_levels[i]);
	}

	consumed_pages = (sizeof(struct page) * zone->total_pages) / PAGE_SIZE + 1;
	/* consumed_pages = consumed_pages / PAGE_SIZE + 1; */

	zone->free_pages = zone->total_pages - consumed_pages;

	for (i = consumed_pages; i < zone->total_pages;) {
		int pages_left = zone->total_pages - i;
		int order = idx_order(i);

		mm_kprintf("Dealign with page 0x%x, order = %d, left %d pages.\n",
				i, order, pages_left);
		
		while ((1 << order) > pages_left)
			order--;

		mm_kprintf("page order %d inserted into te list.\n", order);
		klist0_prepend(&p_pool[i].list, &zone->alloc_levels[order]);
		p_pool[i].private.order = order;

		i += (1 << order);
	}

	print_alloc_info();
}

struct page * __alloc_pages(u32 flags, struct mem_zone * zone, u32 order)
{
	int i = order;
	struct page * chunk;
	
	do { /* Find an non-empty level with sufficiently big chunks in it. */
		if (i > MAX_ORDER)
			return NULL;
	} while (klist0_empty(&zone->alloc_levels[i++]));
	i = i - 1;
	
	chunk = klist0_entry(zone->alloc_levels[i].next, struct page, list); 

	klist0_unlink(zone->alloc_levels[i].next);
	
	/*
	 * If the chunk is too big,  pass the unused  part to the lower levels.
	 */
	while (i-- > order) { 
		chunk->private.order = i;
		klist0_append(&chunk->list, &zone->alloc_levels[i]);
		chunk += (1 << i);
	};

	chunk->private.order = order;

	return chunk;
}

inline struct page * alloc_pages(u32 flags, u32 order)
{
	return __alloc_pages(flags, &boot_zone, order);
}

char * page_to_addr(struct page * page)
{
	return ((char *)page->zone->base) + page->index * PAGE_SIZE;
}

inline char * get_pages(u32 flags, u32 order)
{
	return page_to_addr(alloc_pages(flags, order));
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
			/* Really neighbour? */
			if (neighbour - pg == (1 << order) && idx_order(pg->index) > order) {
				klist0_unlink(tmp);
				pg->private.order++;
				break;
			}

			if (pg - neighbour == (1 << order) && idx_order(neighbour->index) > order) {
				klist0_unlink(tmp);
				pg = neighbour;
				pg->private.order++;
				break;
			}
		}

	} while (pg->private.order < MAX_ORDER && order != pg->private.order);
	
	klist0_append(&pg->list, &zone->alloc_levels[pg->private.order]);

}


