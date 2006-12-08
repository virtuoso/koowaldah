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
#include <thread.h>
#include <textio.h>
#include <sys/errno.h>
#include <bug.h>
#include <scheduler.h>
#include <lib.h>

/* vvv big fat XXX */
#include <i386/segments.h>
extern struct tss_segment root_tss;
/* ^^^ big fat XXX */

static void __attribute__((noreturn)) forker(void *data)
{
	u32 *p = (u32 *)data;

	/* disregard everything on the current stack
	 * as start_user() never returns */
	root_tss.esp0 = (u32)CURRENT() - 4;
	start_user_forked(p[1], 0x40001ffb, p[0], 0);
	bug();
}

pid_t fork()
{
	struct thread *thread;
	struct thread *me = CURRENT();
	struct thread_queue tmp_q;
	u32 virt = USERMEM_VIRT, phys, i;
	u32 *p = __builtin_frame_address(1);

	thread = thread_create_user(&forker, me->name, (void *)p[0],
			me->map->m_cp, me->map->m_dp);
	if (!thread)
		return -ENOMEM;

	/* XXX: copy process' pages */
	for (i = 0; i < 2; i++) {
		switch_map(me->map, thread->map);
		phys = __virt2physpg(virt);
		switch_map(thread->map, me->map);
		memory_copy((char *)phys, (char *)virt, PAGE_SIZE);
		virt += PAGE_SIZE;
	}

	tq_init(&tmp_q);
	tq_insert_head(thread, &tmp_q);

	bug_on(!scheduler_enqueue(&tmp_q));

	return thread->pid;
}

