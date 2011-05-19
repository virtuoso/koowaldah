/*
 * include/mem_area.h
 *
 * Copyright (C) 2006 Alexander Shishkin
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

#ifndef __MEM_AREA_H__
#define __MEM_AREA_H__

#include <page_alloc.h>
#include <arch/memory.h>

/*
 * Memory area holds a group of contiguous pages of a similar nature:
 * processes' code, data, shared mappings and so on.
 */
struct mem_area {
	unsigned long      m_start;    /* where the area starts */
	unsigned long      m_end;      /* where the area ends */
	u32                m_sizelim;  /* size limit */
	u32                m_pages;    /* how many pages are involved */
	u32                m_users;    /* how many threads share this area */
	u32                m_prot;     /* protection */
	u32                m_flags;    /* this area's flags */
	union {
		/* linked pages */
		struct klist0_node	m_plist;
		/* original mma in case of MMA_CLONE */
		struct mem_area		*m_original;
	};
	struct mapping     *m_map;     /* our mapping */
};

/*
 * mem_area flags
 */
#define MMA_RO    (0x00) /* yet another syntax sugar for zero */
#define MMA_RW    (0x01) /* read-write protection; readonly otherwise */
#define MMA_GROWS (0x02) /* mem_area can grow up */
#define MMA_STACK (0x04) /* mem_area can grow down (stack) */
#define MMA_EXEC  (0x08) /* executable */
#define MMA_CONT  (0x10) /* physically contiguous */
#define MMA_CLONE (0x20) /* this mma is a clone of another one */

struct mem_area *mem_area_alloc(struct mapping *map, unsigned long start,
		struct klist0_node *page_list, u32 flags);
struct mem_area *mem_area_alloc_new(struct mapping *map, unsigned long start,
		u32 pages, u32 flags);
struct mem_area *mem_area_clone(struct mapping *map, struct mem_area *mma);
struct mem_area *mem_area_copy(struct mapping *map, struct mem_area *mma);
void mem_area_remap(struct mem_area *mma, unsigned int flags);
void mem_area_grow(struct mem_area *mma, u32 pages);
void mem_area_put(struct mem_area *mma);
void mem_area_add_page(struct mem_area *mma, struct page *pg);
void mem_area_remove_page(struct mem_area *mma, struct page *pg);

static __inline struct mem_area *mma_original(struct mem_area *mma)
{
	struct mem_area *org = mma;

	for (org = mma; org->m_flags & MMA_CLONE; org = org->m_original)
		;

	return org;
}

static __inline void  __mem_area_add_page(struct mem_area *mma, struct page *pg)
{
	if (mma->m_flags & MMA_STACK) {
		mma->m_start -= PAGE_SIZE;
		pg->virt = mma->m_start;
	} else {
		pg->virt = mma->m_end;
		mma->m_end += PAGE_SIZE;
	}

	mma->m_pages++;
}

static __inline void  __mem_area_remove_page(struct mem_area *mma, struct page *pg)
{
	pg->virt = NOPAGE_ADDR;
	mma->m_pages--;
	mma->m_end -= PAGE_SIZE;
}

/*
 * Check if an address is inside a mma or is within its possible size.
 * Interesting for fault handlers.
 * XXX: return value
 */
static __inline int mem_area_is_hit(struct mem_area *mma, u32 addr)
{
	if (addr >= mma->m_start && addr < mma->m_end)
		return 1; /* exact hit */
	return 0;
}

/* returns absolute offset */
static __inline unsigned long mem_area_hit_offset(struct mem_area *mma, u32 addr)
{
	/* check if we still hit within the size limit */
	if (mma->m_flags & MMA_STACK)
		return ((addr >= mma->m_end - mma->m_sizelim) &&
			(addr < mma->m_end) ? (mma->m_start - addr)
			: NOPAGE_ADDR);

	if (mma->m_flags & MMA_GROWS)
		return ((addr >= mma->m_start) &&
			(addr < mma->m_start + mma->m_sizelim) ? 
			(addr - mma->m_end)
			: NOPAGE_ADDR);

	/* miss */
	return NOPAGE_ADDR;
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

void display_map(struct mapping *map);
void clone_map(struct mapping *dst, struct mapping *map);
void switch_map(struct mapping *from, struct mapping *to);
void map_page(struct mapping *map, u32 virt, u32 phys, u16 flags);
void unmap_page(struct mapping *map, u32 virt);
void map_pages(struct mapping *map, void *virt, u32 phys, u32 n, u16 flags);
void free_map(struct mapping *map);

/* brk.c */
void *sbrk(size_t inc);
unsigned long brk(unsigned long end);

#endif

