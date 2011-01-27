/*
 * kernel/brk.c
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

#include <koowaldah.h>
#include <mem_area.h>
#include <thread.h>
#include <error.h>

void *sbrk(size_t inc)
{
	struct thread *t = CURRENT();
	struct mem_area *mma = t->map->m_mma[3]; /* heap mma */

	/* tupo increment heap sizelimit */
	if (mma->m_sizelim + inc <= USERMEM_HEAPLIM)
		mma->m_sizelim += inc;
	else
		return (void *)-1;

	return (void *)(mma->m_start + mma->m_sizelim);
}

unsigned long brk(unsigned long end)
{
	struct thread *t = CURRENT();
	struct mem_area *mma = t->map->m_mma[3]; /* heap mma */
	size_t delta;
	
	if (!end)
		return mma->m_start;

	delta = end - mma->m_start;

	return (unsigned long)sbrk(delta);
}

