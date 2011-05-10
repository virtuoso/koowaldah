
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
#include <lib.h>
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

/* origins */
enum {
	FROM_SLICE,
	FROM_PAGES,
};

/*
 * Chunk Info is placed before the returned chunk to
 * let gfree() know from where it came.
 */
#define CI_SIZE roundup_l2((sizeof(struct chunk_info)), GALLOC_MIN_CHUNK)

#define NPOOLS 9

static struct slice_pool *galloc_sp[NPOOLS];

#define POOL_SIZE(pool) ((GALLOC_MIN_CHUNK << (pool)) + CI_SIZE)

static void *galloc_from_slice(unsigned int flags, size_t size)
{
	int i;
	struct chunk_info *ci;

	/* slice_pool == GALLOC_MIN_CHUNK * 2^i */
	i = log2(size / GALLOC_MIN_CHUNK);
	if (i > NPOOLS)
		return NULL;

	DPRINT("Allocating %d from the %d pool.\n", size, POOL_SIZE(i));

	ci = slice_alloc(galloc_sp[i]);
	if (!ci)
		return NULL;

	ci->origin = FROM_SLICE;
	ci->info = i;

	return ++ci;
}

static void *galloc_from_pages(unsigned int flags, size_t size)
{
	int i;
	struct chunk_info *ci;

	i = log2(INPAGES(size + CI_SIZE));
	ci = get_pages(flags, i);
	if (!ci)
		return NULL;

	ci->origin = FROM_PAGES;
	ci->info = i;

	return ++ci;
}


void *galloc(unsigned int flags, size_t size)
{
	void *ret = NULL;

	if (!size)
		size = GALLOC_MIN_CHUNK;
	size = roundup_l2(size, GALLOC_MIN_CHUNK);

	DPRINT("size = 0x%x\n", size);

	/* first, try slices, then pages */
	ret = galloc_from_slice(flags, size);
	if (!ret)
		ret = galloc_from_pages(flags, size);

	return ret;
}

void gfree(void *chunk)
{
	struct chunk_info *ci = chunk;

	ci--;

	DPRINT("Free: origin = %d, info = %d\n",  ci->origin, ci->info);

	switch(ci->origin) {
		case FROM_SLICE:
			slice_free(ci, galloc_sp[ci->info]);
			break;
		case FROM_PAGES:
			put_pages(ci);
			break;

		default:
			bug();
	}
}

void galloc_init(void)
{
	int i;

	for (i = 0; i < NPOOLS; i++) {
		DPRINT("Creating a slice pool of size %d\n", POOL_SIZE(i));
		galloc_sp[i] = slice_pool_create(0, POOL_SIZE(i));
		if (!galloc_sp[i])
			panic("Failed to allocate slice pool");
	}
}

