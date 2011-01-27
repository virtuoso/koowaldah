/*
 * include/i386/fork.h
 *
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

#include <thread.h>

#ifndef __I386_FORK_H__
#define __I386_FORK_H__

/*
 * Put fork's specific addresses to the bottom of child's stack
 * The return address is stored 6 dwords below current thread's
 * stack top.
 * @thread -- newborn child thread
 */
static __inline void prepare_fork(struct thread *thread)
{
	struct thread *me = CURRENT();
	u32 *p = (u32 *)me - 6;
	u32 *d;

	d = (u32 *)((u32)thread & NOPAGE_MASK);
	d[0] = p[0];
	d[1] = p[3];
}

#endif

