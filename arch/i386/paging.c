
/*
 * arch/i386/paging.c
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
 * 
 */

#include <koowaldah.h>
#include <lib.h>
#include <i386/asm.h>
#include <page_alloc.h>
#include <mm_zone.h>
#include <textio.h>
#include <error.h>

struct mapping root_map;

/* these are calculated in bootmem_zone.c */
u32 phys_mem;
u32 phys_pgs;

static __inline void display_pe_flags(int flags)
{
	if (flags & PTF_PRESENT)
		kprintf("PRESENT ");

	if (flags & PTF_RW)
		kprintf("RW ");

	if (flags & PTF_USER)
		kprintf("USER ");

	if (flags & PTF_PWT)
		kprintf("PTW ");

	if (flags & PTF_PCD)
		kprintf("PCD ");

	if (flags & PTF_ACCESSED)
		kprintf("ACCESSED ");

	if (flags & PTF_DIRTY)
		kprintf("DIRTY ");

	if (flags & PTF_PGSIZE)
		kprintf("PGSIZE ");

	if (flags & PTF_GLOBAL)
		kprintf("GLOBAL ");
}

void display_map(struct mapping *mp)
{
	int __future pgt_idx, pg_idx;


	kprintf("Mapping: [%x]\n", mp->m_pgdir);
	for (pgt_idx = 0; pgt_idx < PGD_ENTRIES; pgt_idx++) {
		if (PE_FLAGS(mp->m_pgdir[pgt_idx]) & PTF_PRESENT) {
			kprintf("pgdir[%x] 0x%08x ", pgt_idx,  pgt_idx * PGT_ENTRIES * PAGE_SIZE);
			display_pe_flags(PE_FLAGS(mp->m_pgdir[pgt_idx]));
			kprintf("\n");
#if 0
			for (pg_idx = 0; pg_idx < PGT_ENTRIES; pg_idx++) {
				kprintf("pgtable[%x] 0x%08x ", pg_idx,
				pgt_idx * PGT_ENTRIES * PAGE_SIZE + pg_idx * PAGE_SIZE);
				display_pe_flags(PE_FLAGS(mp->m_pgdir[pgt_idx]));
				kprintf("\n");
			}
#endif
		}
	}
}


/*
 * Initialize a flat 1:1 mapping
 * which is stored in global root_map
 */
static void init_rootmap()
{
	u32 pgaddr = 0;
	int pg_idx = 0;
	int pgt_idx = 0;
	u32 tablesz;
	u32 *pgtable;
	u32 *pgdir;

	tablesz = phys_pgs / PGT_ENTRIES;
	if (phys_pgs % PGT_ENTRIES)
		tablesz++;

	root_map.m_pgdir =
	pgdir = get_pages(0, 0); /* just one page for pgdir */

	root_map.m_pgtable =
	pgtable = get_pages(0, log2(tablesz));


	/* Fill the page tables */
	while (pg_idx < phys_pgs)
		pgtable[pg_idx++] = PTE_DESC(pgaddr++, PTF_PRESENT | PTF_RW);

	/* Clear the remainder of the last page table */
	while (pg_idx % PGT_ENTRIES)
		pgtable[pg_idx++] = 0 | PTF_USER | PTF_RW;

	/* Add the filled page tables to the page directory */
	for (; pgt_idx < tablesz; pgt_idx++) {
		pgdir[pgt_idx] = (u32)(pgtable + pgt_idx * PGT_ENTRIES) | PTF_PRESENT | PTF_RW | PTF_GLOBAL;
	}

	while (pgt_idx < PGT_ENTRIES)
		pgdir[pgt_idx++] = 0 | PTF_USER | PTF_RW;
#ifdef DEBUG
	display_map(&root_map);
#endif
}

/*
 * Copy existing mapping into a newly created one
 */
void clone_map(struct mapping *dst, struct mapping *map)
{
	u32 *src;
	u32 *pgtable;
	int d, t;

	dst->m_nmma = 0;
	dst->m_pgdir = get_pages(0, 0);
	dst->m_pgtable = pgtable = get_pages(0,
			log2((phys_pgs + PGT_ENTRIES - 1)/PGT_ENTRIES));
	for (d = 0; d < PGD_ENTRIES; d++) {
		/* copy flags as-is */
		dst->m_pgdir[d] = map->m_pgdir[d] & PAGE_MASK;
		dst->m_pgdir[d] |= (u32)pgtable;
		src = (u32 *)(map->m_pgdir[d] & NOPAGE_MASK);

		if (dst->m_pgdir[d] & PTF_PRESENT) {
			/* copy all ptes */
			for (t = 0; t < PGT_ENTRIES; t++)
				pgtable[t] = src[t];
		}

		/* leave holes in page table for quicker access */
		pgtable += PAGE_SIZE / PTE_BYTES;
	}
}

void free_map(struct mapping *map)
{
	put_pages((void *)map->m_pgtable);
	put_pages((void *)map->m_pgdir);
}

/*
 * Add a page to a mapping
 * @map   -- the mapping
 * @virt  -- virtual address inside the mapping (page aligned)
 * @phys  -- physical address of the page
 * @flags -- read/write markers
 */
void map_page(struct mapping *map, u32 virt, u32 phys, u16 flags)
{
	u32 *pgtable = (u32 *)(map->m_pgdir[PGDIDX(virt)] & NOPAGE_MASK);

	map->m_pgdir[PGDIDX(virt)] |= PTF_PRESENT;
	pgtable[PGTIDX(virt)] =
		phys | PTF_PRESENT | PTF_USER | flags;
}

void unmap_page(struct mapping *map, u32 virt)
{
	u32 *pgtable = (u32 *)(map->m_pgdir[PGDIDX(virt)] & NOPAGE_MASK);

	pgtable[PGTIDX(virt)] = 0;
}

/*
 * Add multiple pages to a mapping
 * @map   -- the mapping
 * @virt  -- virtual address inside the mapping (page aligned)
 * @phys  -- physical address of the page
 * @n     -- amount of pages to be mapped
 * @flags -- read/write markers
 */
void map_pages(struct mapping *map, void *virt, u32 phys, u32 n, u16 flags)
{
	u32 i;

	for (i = 0; i < n; i++)
		map_page(map, (uintptr_t)virt + PAGE_SIZE*i, phys + PAGE_SIZE*i,
			 flags);
}

/*
 * Switch from one memory mapping to another
 * Part of context switch
 * @from -- current mapping (to be removed)
 * @to   -- new memory mapping
 */
void switch_map(struct mapping *from, struct mapping *to)
{
	write_cr3((u32)to->m_pgdir);
}

void __init paging_init()
{
	init_rootmap();

	kprintf("# root pgdir=%x\n", root_map.m_pgdir);
	kprintf("# root pgtable=%x\n", root_map.m_pgtable);
	write_cr3((u32)root_map.m_pgdir);
	write_cr0(read_cr0() | 0x80000000);
}

