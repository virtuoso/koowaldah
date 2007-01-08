/*
 * include/mem_area.h
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

#ifndef __MEM_AREA_H__
#define __MEM_AREA_H__

#include <page_alloc.h>

/*
 * Memory area holds a group of contiguous pages of a similar nature:
 * processes' code, data, shared mappings and so on.
 */
struct mem_area {
	unsigned long      m_start;    /* where the area starts */
	unsigned long      m_end;      /* where the area ends */
	u32                m_pages;    /* how many pages are involved */
	u32                m_users;    /* how many threads share this area */
	u32                m_prot;     /* protection */
	u32                m_flags;    /* this area's flags */
	struct klist0_node m_plist;    /* list of struct page objects */
	struct mapping     *m_map;     /* our mapping */
};

/*
 * mem_area flags
 */
#define MMA_RO    (0x00) /* yet another syntax sugar for zero */
#define MMA_RW    (0x01) /* read-write protection; readonly otherwise */
#define MMA_GROWS (0x02) /* mem_area can grow up */
#define MMA_STACK (0x04) /* mem_area can grow down (stack) */

struct mem_area *mem_area_alloc(struct mapping *map, unsigned long start,
		struct klist0_node *page_list, u32 flags);
struct mem_area *mem_area_alloc_new(struct mapping *map, unsigned long start,
		u32 pages, u32 flags);
struct mem_area *mem_area_clone(struct mapping *map, struct mem_area *mma);
void mem_area_attach(struct mapping *dst, struct mem_area *mma);
void mem_area_kill(struct mem_area *mma, struct mapping *map);
void mem_area_put(struct mem_area *mma, struct mapping *map);
void mem_area_add_page(struct mem_area *mma, struct page *pg);
void mem_area_remove_page(struct mem_area *mma, struct page *pg);

static inline void  __mem_area_add_page(struct mem_area *mma, struct page *pg)
{
	pg->virt = mma->m_end;
	mma->m_pages++;
	mma->m_end += PAGE_SIZE;
}

static inline void  __mem_area_remove_page(struct mem_area *mma, struct page *pg)
{
	pg->virt = NOPAGE_ADDR;
	mma->m_pages--;
	mma->m_end -= PAGE_SIZE;
}

/* XXX: dynamic arrays will obsolete this */
#define MMA_MAX 8

/* memory mapping/page directory */
struct mapping {
	u32               *m_pgdir;   /* ptr to arch-specific data struct */
	u32               *m_pgtable; /* likewise, redundant btw */
	struct slice_pool *m_mmapool;
	struct mem_area   *m_mma[MMA_MAX];
	u32               m_nmma;
};

extern struct mapping root_map;

void clone_map(struct mapping *dst, struct mapping *map);
int init_user_map(struct mapping *map, u32 cp, u32 dp, u32 hp);
void switch_map(struct mapping *from, struct mapping *to);
void map_page(struct mapping *map, u32 virt, u32 phys, u16 flags);
void unmap_page(struct mapping *map, u32 virt);
void map_pages(struct mapping *map, u32 virt, u32 phys, u32 n, u16 flags);
void free_map(struct mapping *map);

#endif

