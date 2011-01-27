/*
 * kernel/init/test_pf.c
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
#include <bug.h>

void test_pf()
{
#ifdef OPT_TEST_PF
	u32 * address = (u32 *) 0xDEADBEEF;
	
	kprintf("We are going to have a page fault at 0x%x\n",
		(u32) address);

	*address = 0;

	/* We should be already dead now. */
	bug();
#endif
}

