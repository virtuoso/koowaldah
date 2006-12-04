/*
 * kernel/thread.c
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
 * Koowaldah kernel threads implementation
 * 
 */


#include <koowaldah.h>
#include <mm.h>
#include <textio.h>
#include <thread.h>
#include <bug.h>
#include <klist0.h>
#include <scheduler.h>
#include <lib.h>
#include <page_alloc.h>

/* each architecture should define this */
extern void thread_init_stack(struct thread *t, thread_t func, void *data);

/* omgwtf */
u32 get_free_pid()
{
	static u32 i = 0;
	return i++;
}

void dump_thread(struct thread *thread)
{
	kprintf("Thread %x \"%s\":\n"
		"\tpid: %d; stack: [%x..%x]\n"
		"\tstate: %s\n"
		"\tneighbours: %x, %x\n",
		thread, thread->name, thread->pid,
		tctx(thread).esp,
		tctx(thread).stack_base,
		(thread->state & THREAD_RUNNABLE ? "runnable" : "waiting"),
		thread->kthreads.prev, thread->kthreads.next
	);
}

struct thread *thread_create(thread_t func, char *name, void *data)
{
	void *page;
	struct thread *thread;
	
	/* allocate stack space */
	page = get_pages(/*THREAD_STACK_LIMIT/PAGE_SIZE*/0, 0);
	if (!page) {
		kprintf("Thread allocation screwed up. Don't panic!\n");
		return NULL;
	}

	/* now, place the task descriptor */
	thread = THREAD(page);
	thread->state = THREAD_NEW;
	/*kprintf("page=%x, thread=%x\n", page, thread);*/

	/* stack pointers */
	tctx(thread).stack_base = (u32 *)thread - 1;
	tctx(thread).esp = (u32)tctx(thread).stack_base;
	thread_init_stack(thread, func, data);

	thread->pid = get_free_pid();
	thread->state = THREAD_RUNNABLE;

	/* No problem if we copy a little more. */
	memory_copy(thread->name, name, THREAD_NAME_LEN);
	KLIST0_INIT(&thread->kthreads);
	KLIST0_INIT(&thread->krunq);
	klist0_append(&thread->kthreads, &thread_list);
	thread->ns = &root_ns;
	thread->last_fd = 0;
	KLIST0_INIT(&thread->files);

	/* use root memory mapping */
	thread->map = &root_map;

	/*kprintf("created thread, stack_base = %x, esp = %x, pid = %d\n", 
			(u32) (tctx(thread).stack_base),
			(u32) (tctx(thread).esp), thread->pid);*/

	return thread;
}

struct thread *thread_create_user(thread_t func, char *name, void *data,
		u32 cp, u32 dp)
{
	struct thread *thread;

	thread = thread_create(func, name, data);
	if (thread) {
		thread->map = memory_alloc(sizeof(struct mapping));
		if (init_user_map(thread->map, cp, dp)) {
			memory_release(thread->map);
			/*thread_destroy(thread)*/
			return NULL;
		}
	}

	return thread;
}

