
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
#include <arch/asm.h>
#include <page_alloc.h>
#include <textio.h>

struct mapping root_map;

/* these are calculated in bootmem_zone.c */
u32 phys_mem;
u32 phys_pgs;

u32 log2(u32 n)
{
	u32 l = 1;
	while ((l < 32) && (n >> l)) l++;
	return l - 1;
}

/*
 * Initialize a flat 1:1 mapping
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
		for (t = 0; t < PGT_ENTRIES; t++) {
			pgtable[t] = (pgaddr++ << PAGE_SHIFT) |
				PTF_PRESENT | PTF_RW;
		}
		pgdir[d] = (u32)pgtable | PTF_PRESENT | PTF_RW;
		pgtable += PAGE_SIZE / PTE_BYTES;
	}

	for (; d < PGD_ENTRIES; d++)
		pgdir[d] = 0 | PTF_USER | PTF_RW;
}

void copy_map(struct mapping *dst, struct mapping *map)
{
	u32 *src;
	u32 *pgtable;
	int d, t;

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

			pgtable += PAGE_SIZE / PTE_BYTES;
		}
	}
}

void init_user_map(struct mapping *map)
{
	int d, t;
	u32 *pgtable;
	u32 pgaddr = 0x00000000;

	map->m_pgdir = get_pages(0, 0);
	map->m_pgtable = pgtable = get_pages(0, 
			log2((phys_pgs)/PGT_ENTRIES));

	for (d = 0; d < 8; d++) {
		for (t = 0; t < PGT_ENTRIES; t++) {
			pgtable[t] = (pgaddr++ << PAGE_SHIFT) |
				PTF_PRESENT | PTF_RW;
		}
		map->m_pgdir[d] = (u32)pgtable |
			PTF_PRESENT | PTF_RW;
		pgtable += PAGE_SIZE / PTE_BYTES;
	}

	/* skip to 1GB */
	for (; d < 0x100; d++)
		map->m_pgdir[d] = 0;

	for (; d < 0x108; d++) {
		for (t = 0; t < PGT_ENTRIES; t++) {
			pgtable[t] = (pgaddr++ << PAGE_SHIFT) |
				PTF_USER | PTF_PRESENT | PTF_RW;
		}
		map->m_pgdir[d] = (u32)pgtable |
			PTF_USER | PTF_PRESENT | PTF_RW;
		pgtable += PAGE_SIZE / PTE_BYTES;
	}

	/* the rest is not present either */
	for (; d < PGD_ENTRIES; d++)
		map->m_pgdir[d] = 0;
}

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

