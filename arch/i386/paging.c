
/*
 * arch/i386/paging.c
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
#include <lib.h>
#include <i386/asm.h>
#include <page_alloc.h>
#include <mm_zone.h>
#include <mm.h>
#include <textio.h>
#include <error.h>

struct mapping root_map;

/* these are calculated in bootmem_zone.c */
u32 phys_mem;
u32 phys_pgs;

/*
 * Initialize a flat 1:1 mapping
 * which is stored in global root_map
 */
static void init_rootmap()
{
	u32 pgaddr = 0;
	int d, t;
	u32 tablesz;
	u32 *pgtable;
	u32 *pgdir;

	tablesz = phys_pgs / PGT_ENTRIES;
	root_map.m_pgdir =
	pgdir = get_pages(0, 0); /* just one page for pgdir */

	root_map.m_pgtable =
	pgtable = get_pages(0, log2(tablesz));

	for (d = 0; d < tablesz; d++) {
		for (t = 0; t < PGT_ENTRIES; t++)
			pgtable[t] = PTE_DESC(pgaddr++, PTF_PRESENT | PTF_RW);

		pgdir[d] = (u32)pgtable | PTF_PRESENT | PTF_RW | PTF_GLOBAL;
		pgtable += PAGE_SIZE / PTE_BYTES;
	}

	for (; d < PGD_ENTRIES; d++)
		pgdir[d] = 0 | PTF_USER | PTF_RW;
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
	dst->m_pgtable = pgtable = get_pages(0, log2(phys_pgs/PGT_ENTRIES));
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
void map_pages(struct mapping *map, u32 virt, u32 phys, u32 n, u16 flags)
{
	u32 i;

	for (i = 0; i < n; i++)
		map_page(map, virt + PAGE_SIZE*i, phys + PAGE_SIZE*i, flags);
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

