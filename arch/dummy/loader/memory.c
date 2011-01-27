/*
 * arch/dummy/loader/memory.c
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

#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include "loader.h"

static unsigned long mem_size = 8 * 1024 * 1024;

/*
 * Return the amount of 'physical' memory
 */
EXPORT int dummy_get_mem_size()
{
	return mem_size;
}

/*
 * mmap required amount of 'physical' memory
 */
PRIVATE int init_memory()
{
	void *p;

	p = mmap(NULL, mem_size,
			PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_ANONYMOUS | MAP_FIXED, 0, 0);
	if (p == MAP_FAILED) {
		perror("mmap");
		return -1;
	}

	return 0;
}

