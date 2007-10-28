
/*
 * include/page_alloc.h
 *
 * Copyright (C) 2005 Alexey Zaytsev
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

#ifndef __PAGE_ALLOC_H__
#define __PAGE_ALLOC_H__

#include <koowaldah.h>
#include <klist0.h>
#include <arch/pages.h>

#define MAX_ORDER 11

struct mem_zone;

struct page {
	u32 index;
	unsigned long virt;
	struct klist0_node list;
	struct klist0_node area_list;
	union {
		u32 order;
	} private;
	struct mem_zone *zone;
};

struct page *alloc_pages(u32 flags, u32 order);
u32 *get_pages(u32 flags, u32 order);
void *page_to_addr(struct page *page);
struct page *addr_to_page(u32 *page);

#define alloc_page(flags) alloc_pages(flags, 0) 
#define get_page(flags) get_pages(flags, 0)

void free_pages(struct page *pg);
void put_pages(void *addr);

#define free_page(page) free_pages(page)
#define put_page(addr) put_pages(addr)

void print_alloc_info();

#endif /* __MM_H__ */

