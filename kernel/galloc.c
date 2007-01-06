
/*
 * kernel/galloc.c
 *
 * Copyright (C) 2006 Alexey Zaytsev
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Koowaldah developers nor the names of their
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
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

static inline u32 *galloc_from_slice(u32 flags, size_t size)
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

static inline u32 *galloc_from_pages(u32 flags, size_t size)
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


