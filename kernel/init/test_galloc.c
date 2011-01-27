/*
 * kernel/init/test_bug.c
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
#include <bug.h>
#include <textio.h>
#include <galloc.h>

u32 * tmp[2044];

void test_galloc()
{
#ifdef OPT_TEST_GALLOC

	int i;

	for (i = 0; i < 2044; i++) {
		kprintf("Allocating chunk, size = %d\n", i);

		tmp[i] = galloc(0, i);
		if (!tmp[i]) {
			kprintf("Failed!\n");
			bug();
		}
	}

	for (i = 0; i < 2044; i = i + 2) {

		kprintf("Freeing chunk %d (0x%x) \n", i, (u32) tmp[i]);
		

		gfree(tmp[i]);
	
	}
	for (i = 1; i < 2044; i = i + 2) {

		kprintf("Freeing chunk %d\n", i);

		gfree(tmp[i]);
	
	}

	for (i = 0; i < 100; i++) {
		tmp[i] = galloc(0, PAGE_SIZE * i);
		if (!tmp[i]) {
			kprintf("Failed\n");
			bug();
		}
	}

	for (i = 0; i < 100; i++) {
		kprintf("Freeing chunk %d\n", i);
		gfree(tmp[i]);
	}

	kprintf("Done.");



#endif
}

