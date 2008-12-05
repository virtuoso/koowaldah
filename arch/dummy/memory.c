/*
 * arch/dummy/memory.c
 *
 * Copyright (C) 2007 Alexander Shishkin
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

#include <koowaldah.h>
#include <dummy/loader.h>

void arch_init_boot_zone(struct mem_zone *zone, struct mem_zone *user)
{
	u32 phys_mem, total_mem;
	
	phys_mem = total_mem = dummy_get_mem_size();
	kprintf("Got %dM (0x%x) bytes of memory\n",
			phys_mem >> 20, total_mem);

	zone->base = NULL;
	zone->total_pages = 0xf0000 >> PAGE_SHIFT;
	kprintf("Total pages: %d, phys_pgs: %d\n", zone->total_pages);

	mem_zone_init(zone);

	user->total_pages = (total_mem >> PAGE_SHIFT) - zone->total_pages;
	user->base = (u32 *)((unsigned long)zone->base + (zone->total_pages << PAGE_SHIFT));

	kprintf("Total pages: %d, base: %x\n", user->total_pages, user->base);
	mem_zone_init(user);
}

void paging_init()
{
}

void map_page(struct mapping *map, u32 virt, u32 phys, u16 flags)
{
}

void unmap_page(struct mapping *map, u32 virt)
{
}

void map_pages(struct mapping *map, u32 virt, u32 phys, u32 n, u16 flags)
{
}

void display_map(struct mapping *map)
{
}

struct mapping root_map;

void switch_map(struct mapping *from, struct mapping *to)
{
}

void clone_map(struct mapping *dst, struct mapping *map)
{
}

void free_map(struct mapping *map)
{
}


