/*
 * kernel/init/test_mm.c
 *
 * Copyright (C) 2006 Alexey Zaytsev
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
