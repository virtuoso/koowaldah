/*
 * kernel/fork.c
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
 */

#include <koowaldah.h>
#include <mm_zone.h>
#include <thread.h>
#include <textio.h>
#include <khui/errno.h>
#include <bug.h>
#include <scheduler.h>
#include <lib.h>
#include <arch/asm.h>
#include <arch/fork.h>

static void __noreturn forker(void *data)
{
	u32 *p = (u32 *)((u32)CURRENT() & NOPAGE_MASK);

	/* disregard everything on the current stack
	 * as start_user() never returns */
	reset_stack();
	start_user(p[0], USERMEM_STACK + PAGE_SIZE - 4, p[1], 0);
	bug();
}

pid_t fork()
{
	struct thread *thread;
	struct thread *me = CURRENT();
	struct thread_queue tmp_q;

	thread = thread_create_user(&forker, me->name, NULL);
	if (!thread)
		return -ENOMEM;

	thread->map->m_mma[0] = mem_area_clone(thread->map, me->map->m_mma[0]);
	thread->map->m_mma[1] = mem_area_copy(thread->map, me->map->m_mma[1]);
	thread->map->m_mma[2] = mem_area_copy(thread->map, me->map->m_mma[2]);
	thread->map->m_mma[3] = mem_area_copy(thread->map, me->map->m_mma[3]);
	thread->map->m_nmma = 4;

	prepare_fork(thread);

	tq_init(&tmp_q);
	tq_insert_head(thread, &tmp_q);

	bug_on(!scheduler_enqueue(&tmp_q));

	return thread->pid;
}

