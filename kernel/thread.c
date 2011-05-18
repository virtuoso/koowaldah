
/*
 * kernel/thread.c
 *
 * Copyright (C) 2006 Alexander Shishkin
 * Copyright (C) 2006 Alexey Zaytsev
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
 * Koowaldah kernel threads implementation
 * 
 */


#include <koowaldah.h>
#include <galloc.h>
#include <mm_zone.h>
#include <textio.h>
#include <thread.h>
#include <bug.h>
#include <debug.h>
#include <klist0.h>
#include <spinlock.h>
#include <scheduler.h>
#include <lib.h>
#include <page_alloc.h>

/* omgwtf */
pid_t get_free_pid()
{
	static pid_t i = 0;
	return i++;
}

/* omgwtf II */
struct thread *thread_get(pid_t pid)
{
	struct klist0_node *tmp;
	struct thread *t;

	klist0_for_each(tmp, &thread_list.threads) {
		t = klist0_entry(tmp, struct thread, kthreads);
		
		if (t->pid == pid)
			return t;
	}

	return NULL;
}

void dump_thread(struct thread *thread)
{
	kprintf("Thread %x \"%s\":\n"
		"\tpid: %d; stack: [%x..%x]\n"
		"\tstate: %s\n"
		"\tneighbours: %x, %x\n",
		thread, thread->name, thread->pid,
		thread->context.esp,
		thread->context.stack_base,
		(thread->state & THREAD_RUNNABLE ? "runnable" : "waiting"),
		thread->kthreads.prev, thread->kthreads.next
	);
}

struct thread *thread_create(thread_t func, char *name, void *data)
{
	void *page;
	struct thread *thread;
	unsigned long flags;

	/* allocate stack space */
	page = get_pages(/*THREAD_STACK_LIMIT/PAGE_SIZE*/0, 0);
	if (!page) {
		DPRINT("Thread allocation screwed up. Don't panic!\n");
		return NULL;
	}

	/* now, place the task descriptor */
	thread = THREAD(page);
	thread->state = THREAD_NEW;
	DPRINT("page=%x, thread=%x\n", page, thread);

	thread_init_stack(thread, func, data);

	thread->pid = get_free_pid();
	thread->state = THREAD_RUNNABLE;

	/* No problem if we copy a little more. */
	memory_copy(thread->name, name, THREAD_NAME_LEN);
	KLIST0_INIT(&thread->kthreads);
	KLIST0_INIT(&thread->krunq);
	thread->ns = &root_ns;
	thread->last_fd = 0;
	KLIST0_INIT(&thread->files);
	KLIST0_INIT(&thread->mbox);

	/* use root memory mapping */
	thread->map = &root_map;

	bitmap_zero(thread->fdset, FS_MAX_FDS);

	thread->krunq_head = NULL;
	spin_lock_irqsave(&thread_list.lock, flags);
	klist0_append(&thread->kthreads, &thread_list.threads);
	spin_unlock_irqrestore(&thread_list.lock, flags);
	DPRINT("created thread, stack_base = %x, esp = %x, pid = %d\n",
			thread->context.stack_base,
			thread->context.esp, thread->pid);

	return thread;
}

struct thread *thread_create_user(thread_t func, char *name, void *data)
{
	struct thread *thread;

	thread = thread_create(func, name, data);
#ifndef OPT_CPU_ARCH_DUMMY
	if (thread) {
		thread->map = gobj_alloc(struct mapping);
		clone_map(thread->map, &root_map);
	}
#endif

	return thread;
}

