/*
 * kernel/fork.c
 *
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
#include <mm_zone.h>
#include <thread.h>
#include <textio.h>
#include <uabi/errno.h>
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

	mem_area_attach(thread->map, me->map->m_mma[0]);
	thread->map->m_mma[1] = mem_area_clone(thread->map, me->map->m_mma[1]);
	thread->map->m_mma[2] = mem_area_clone(thread->map, me->map->m_mma[2]);
	thread->map->m_mma[3] = mem_area_clone(thread->map, me->map->m_mma[3]);
	thread->map->m_nmma = 4;

	prepare_fork(thread);

	tq_init(&tmp_q);
	tq_insert_head(thread, &tmp_q);

	bug_on(!scheduler_enqueue(&tmp_q));

	return thread->pid;
}

