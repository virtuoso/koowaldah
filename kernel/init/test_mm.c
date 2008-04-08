/*
 * kernel/init/test_mm.c
 *
 * Copyright (C) 2006 Alexey Zaytsev
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
#include <textio.h>
#include <page_alloc.h>

struct page *pages[2048];

void test_mm()
{
#ifdef OPT_PAGE_ALLOCATOR_TEST
	struct page *pg[7];

	int i;

	kprintf("Testing the page allocator...\n");
	print_alloc_info();

	pg[1] = alloc_pages(0, 0);
	kprintf("got page 0x%x, order %d\n", pg[1]->index * PAGE_SIZE, pg[1]->private.order);
	print_alloc_info();

	kprintf("\nTest 1 passed\n\n");

	pg[2] = alloc_pages(0, 1);
	kprintf("got page 0x%x, order %d\n", pg[2]->index * PAGE_SIZE, pg[2]->private.order);
	print_alloc_info();

	kprintf("\nTest 2 passed\n\n");


	kprintf("free 1\n");
	free_pages(pg[1]);
	print_alloc_info();
	
	pg[3] = alloc_pages(0, 3);
	kprintf("got page 0x%x, order %d\n", pg[3]->index * PAGE_SIZE, pg[3]->private.order);
	print_alloc_info();
	kprintf("\nTest 3 passed\n\n");

	pg[4] = alloc_pages(0, 1);
	kprintf("got page 0x%x, order %d\n", pg[4]->index * PAGE_SIZE, pg[4]->private.order);
	print_alloc_info();
	kprintf("\nTest 4 passed\n\n");

	kprintf("free 2\n");
	free_pages(pg[2]);
	print_alloc_info();

	pg[5] = alloc_pages(0, 4);
	kprintf("got page 0x%x, order %d\n", pg[5]->index * PAGE_SIZE, pg[5]->private.order);
	print_alloc_info();
	kprintf("\nTest 5 passed\n\n");

	pg[6] = alloc_pages(0, 2);
	kprintf("got page 0x%x, order %d\n", pg[6]->index * PAGE_SIZE, pg[6]->private.order);
	print_alloc_info();
	kprintf("\nTest 6 passed\n\n");

	kprintf("free 4\n");
	free_pages(pg[4]);
	print_alloc_info();

	kprintf("free 3\n");
	free_pages(pg[3]);
	print_alloc_info();
	kprintf("free 6\n");
	free_pages(pg[6]);
	print_alloc_info();

	kprintf("free 5\n");
	free_pages(pg[5]);
	print_alloc_info();

	pg[0] = alloc_pages(0, 1);
	pg[1] = alloc_pages(0, 2);
	pg[2] = alloc_pages(0, 3);
	pg[3] = alloc_pages(0, 4);
	pg[4] = alloc_pages(0, 5);
	pg[5] = alloc_pages(0, 6);
	
	kprintf("Now allocating 2048 0-order pages.\n");

	for (i = 0; i < 2048; i++) {
		pages[i] = alloc_page(0);
	}

	kprintf("Done allocating 2048 0-roder pages.\n");

	print_alloc_info();

	kprintf("Freeing even pages\n");
	
	for (i = 0; i < 1048; i = i + 2) {
		free_page(pages[i]);
	}

	kprintf("Done freeing even pages.\n");

	print_alloc_info();



	kprintf("pg[0] = 0x%x\npg[1] = 0x%x\npg[2] = 0x%x\npg[3] = 0x%x\n"
		"pg[4] = 0x%x\npg[5] = 0x%x\npg[6] = 0x%x\n",
		pg[0], pg[1], pg[2], pg[3], pg[4], pg[5], pg[6]);

	kprintf("page_to_addr(pg[0]) = 0x%x\n", page_to_addr(pg[0]));
	kprintf("page_to_addr(pg[1]) = 0x%x\n", page_to_addr(pg[1]));
	kprintf("page_to_addr(pg[2]) = 0x%x\n", page_to_addr(pg[2]));
	kprintf("page_to_addr(pg[3]) = 0x%x\n", page_to_addr(pg[3]));
	kprintf("page_to_addr(pg[4]) = 0x%x\n", page_to_addr(pg[4]));
	kprintf("page_to_addr(pg[5]) = 0x%x\n", page_to_addr(pg[5]));
	kprintf("page_to_addr(pg[6]) = 0x%x\n", page_to_addr(pg[6]));

	
	kprintf("addr_to_page(page_to_addr(pg[0])) = 0x%x\n",
		addr_to_page(page_to_addr(pg[0])));
	kprintf("addr_to_page(page_to_addr(pg[1])) = 0x%x\n",
		addr_to_page(page_to_addr(pg[1])));
	kprintf("addr_to_page(page_to_addr(pg[2])) = 0x%x\n",
		addr_to_page(page_to_addr(pg[2])));
	kprintf("addr_to_page(page_to_addr(pg[3])) = 0x%x\n",
		addr_to_page(page_to_addr(pg[3])));
	kprintf("addr_to_page(page_to_addr(pg[4])) = 0x%x\n",
		addr_to_page(page_to_addr(pg[4])));
	kprintf("addr_to_page(page_to_addr(pg[5])) = 0x%x\n",
		addr_to_page(page_to_addr(pg[5])));
	kprintf("addr_to_page(page_to_addr(pg[6])) = 0x%x\n",
		addr_to_page(page_to_addr(pg[6])));

	free_page(pg[0]);
	free_page(pg[1]);
	free_page(pg[2]);
	free_page(pg[3]);
	free_page(pg[4]);
	free_page(pg[5]);
	free_page(pg[6]);

	kprintf("Done.\n");
#endif
}
