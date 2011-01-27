
/*
 * kernel/galloc.c
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
#include <page_alloc.h>
#include <slice.h>
#include <galloc.h>
#include <bug.h>


#if 0
#define DEBUG
#endif

#include <debug.h>

struct chunk_info {
	u8 origin;
	u8 nop;
	u16 info;
};

#define FROM_SLICE 1
#define FROM_PAGES 2


/* 
 * Chunk Info is places before the returned chunk to
 * let gfree() know from where it came.
 */
#define CI_SIZE (((sizeof (struct chunk_info) + 3) & ~3))


#define NPOOLS 9


static struct slice_pool *galloc_sp[NPOOLS];

static __inline u32 *galloc_from_slice(u32 flags, size_t size)
{
	int i;
	struct chunk_info *ci;

	for (i = 0; i < NPOOLS; i++) {
		if (size <= (4 << i)) {
			DPRINT("Allocating from the %d pool.\n",
				(4 << i) + CI_SIZE);

			ci = (struct chunk_info *) slice_alloc(galloc_sp[i]);
			if (!ci)
				return NULL;

			ci->origin = FROM_SLICE;
			ci->info = i;

			return (u32 *) (++ci);
		}
	}

	bug();

	return NULL;
}

static __inline u32 *galloc_from_pages(u32 flags, size_t size)
{

	int i;
	struct chunk_info *ci;


	for (i = 0; i < MAX_ORDER; i++) {
		if (size + CI_SIZE <= (PAGE_SIZE << i)) {
			DPRINT("Allocating from level %d.\n", i);

			ci = (struct chunk_info *) get_pages(0, i);
			if (!ci)
				return NULL;

			ci->origin = FROM_PAGES;

			ci->info = i;

			return (u32 *) (++ci);
		}
	}

	bug();

	return NULL;

}


u32 * galloc(u32 flags, size_t size)
{
	DPRINT("galloc(0x%x)\n", size);

	size = (size + 3) & ~3;

	DPRINT("size = 0x%x\n", size);

	if (size <= (4 << (NPOOLS - 1))) {
		return galloc_from_slice(flags, size);
	}
		
	return galloc_from_pages(flags, size);

}

void gfree(u32 *chunk)
{
	struct chunk_info *ci = (struct chunk_info *) chunk;

	ci--;

	DPRINT("Free: origin = %d, info = %d\n",  ci->origin, ci->info);

	switch(ci->origin) {
		case FROM_SLICE:
			slice_free((u32 *) ci, galloc_sp[ci->info]);
			break;
		case FROM_PAGES:
			put_pages((u32 *) ci);
			break;

		default:
			bug();
	}
}

void galloc_init()
{
	int i;

	for (i = 0; i < NPOOLS; i++) {
		DPRINT("Creating a slice pool of size %d\n",
			(4 << i) + CI_SIZE);
		galloc_sp[i] = slice_pool_create(0, (4 << i) + CI_SIZE);
		if (!galloc_sp[i])
			panic("Alarma!");
	}
}


