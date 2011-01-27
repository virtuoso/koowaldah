/*
 * kernel/aout.c
 *
 * Copyright (C) 2007 Alexander Shishkin
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

#include <koowaldah.h>
#include <inode.h>
#include <mem_area.h>
#include <page_alloc.h>
#include <bug.h>
#include <error.h>
#include <lib.h>
#include <debug.h>
#include <aout.h>

#define PGALIGN(x) \
	(((x) + PAGE_SIZE - 1) & NOPAGE_MASK)
#define INPAGES(x) \
	(PGALIGN(x) / PAGE_SIZE)

void aout_dump(struct exec *aouth)
{
	u32 cpg, dpg;

	cpg = PGALIGN(aouth->a_text);
	dpg = PGALIGN(aouth->a_data + aouth->a_bss);
	DPRINT(
		"text:  %d/%d\n"
		"data:  %d/%d\n"
		"bss:   %d\n"
		"syms:  %d\n"
		"entry: %x\n",
		aouth->a_text, cpg,
		aouth->a_data, dpg,
		aouth->a_bss,
		aouth->a_syms,
		aouth->a_entry
	      );
}

/*
 * Copy binary's section into a given mem_area.
 * @mma   -- memory area to hold the section
 * @inode -- inode containing the binary
 * @off   -- offset of the section in the file
 * @len   -- length of the section
 * Note1: section has to be created beforehand.
 * Note2: it is assumed that all the file's pages are loaded in
 *        inode's buffers.
 */
void aout_copy_section(struct mem_area *mma, struct inode *inode,
		off_t off, size_t len)
{
	struct page *page;
	struct klist0_node *tmp;
	struct page *destpg;
	void *to = NULL;
	u32 pgnum, pgoff;
	size_t left, tocopy;

	/* where we start */
	pgnum = (off & NOPAGE_MASK) >> PAGE_SHIFT;
	pgoff = off & PAGE_MASK;
	tmp = mma->m_plist.next;
	left = len;

	while (left > 0) {
		page = inode->i_map.i_pages[pgnum];
		if (!((u32)to & PAGE_MASK)) {
			destpg = klist0_entry(tmp, struct page, area_list);
			to = page_to_addr(destpg);
		}

		if (pgoff)
			tocopy = MIN(left, (size_t)PAGE_SIZE - pgoff);
		else
			tocopy = MIN(left,
					(size_t)(PAGE_SIZE - ((u32)to & PAGE_MASK)));

		/*DPRINT("### copy: %x[%x] to %x[%x], %d bytes\n",
				page_to_addr(page) + pgoff, page->virt, to, destpg->virt,
				tocopy);*/
		memory_copy(to, page_to_addr(page) + pgoff,
				tocopy);

		/* switch to next source page */
		if (pgoff + tocopy == PAGE_SIZE)
			pgnum++;
		else
			pgoff = tocopy;

		left -= tocopy;
		to += tocopy;
		
		if (!((u32)to & PAGE_MASK))
			tmp = tmp->next;
		else
			pgoff = 0;
	}
}

/*
 * Load an a.out format binary into given mapping
 * @inode -- inode of the file containing the binary
 * @map   -- memory mapping into which the binary should
 *           be loaded.
 * Note1: map does not have to be CURRENT()'s mapping
 * Note2: this adds mem_area objects to map and modifies
 *        map->m_nmma accordingly.
 */
int aout_load(struct inode *inode, struct mapping *map)
{
	struct page **pages = inode->i_map.i_pages;
	struct exec *aouth;
	u32 cpg, dpg;

	/* get aout header from the beginning of first inode's page */
	aouth = AOUTH(page_to_addr(pages[0]));

	/* we only support OMAGIC aout at the moment */
	if (!AOUT_isOMAGIC(aouth)) {
		DPRINT("Binary format not supported\n");
		return -ENOEXEC;
	}

	/* calculate text and data section sizes in pages */
	cpg = INPAGES(aouth->a_text);
	dpg = INPAGES(aouth->a_data + aouth->a_bss);

	aout_dump(aouth);

	/* do text section */
	map->m_mma[0] = mem_area_alloc_new(map, aouth->a_entry, cpg, MMA_RO);
	aout_copy_section(map->m_mma[0], inode, AOUT_TEXTOFF(aouth),
			aouth->a_text);

	/* do data section */
	map->m_mma[1] = mem_area_alloc_new(map, map->m_mma[0]->m_end,
			dpg, MMA_RW);
	aout_copy_section(map->m_mma[1], inode, AOUT_DATAOFF(aouth),
			aouth->a_data);

	map->m_nmma = 2;
	
	return 0;
}

