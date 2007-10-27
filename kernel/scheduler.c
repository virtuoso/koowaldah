
/*
 * kernel/scheduler.c
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
 */


#include <koowaldah.h>
#include <mem_area.h> /* for display_map() */
#include <irq.h>
#include <textio.h>
#include <lib.h>
#include <klist0.h>
#include <khui/errno.h>
#include <scheduler.h>
#include <spinlock.h>
#include <thread.h>
#include <bug.h>

/* vvv unused till we have mm */
struct my_sched {
	struct cpu_scheduler *sched;
	struct klist0_node   sched_list;
};

/* list of registered schedulers */
static struct klist0_node __future sched_list;

static struct my_sched __future *active_sched = NULL;
/* ^^^ */

static struct cpu_scheduler *reg_sched = NULL;
static int sched_active = 0;

/* system-wide list of threads */
struct thread_queue thread_list;

struct klist0_node krunqueue;

u32 need_reschedule;
struct thread *next_thread;

/*
 * List all threads in the system
 * Prints a lot of rubbish.
 */
void list_threads(u32 ops)
{
	struct klist0_node *t;
	struct thread *thread;
	int i = 0;
	u32 flags;

	spin_lock_irqsave(&thread_list.lock, flags);
	klist0_for_each(t, &thread_list.threads) {
		thread = klist0_entry(t, struct thread, kthreads);
		
		if (ops & LIST_CURRENT && thread != CURRENT())
			continue;

		kprintf("# [%s] pid=%d, state=%x, last_tick=%d",
				thread->name, thread->pid,
				thread->state, thread->last_tick);

		if (ops & LIST_BACKTRACE)
			arch_dump_stack((u32 *)thread->context.esp);

		if (ops & LIST_MAPPING)
			display_map(thread->map);

		kprintf("\n");

		if (i++ > MAX_THREADS) bug();
	}
	spin_unlock_irqrestore(&thread_list.lock, flags);
}

/*
 * Register a cpu scheduler within the kernel.
 * There can be just one scheduler at the moment,
 * because there's no memory management yet.
 */
int register_cpusched(struct cpu_scheduler *sched)
{
	if (!sched           ||
	    !sched->name     ||
	    !sched->enqueue  ||
	    !sched->dequeue  ||
	    !sched->tick     ||
	    !sched->yield    ||
	    !sched->schedule ||
	    !sched->start    ||
	    !sched->stop     ||
	    !sched->init     ||
	    !sched->exit) {
		kprintf("%s: not a valid cpu scheduler\n", __FUNCTION__);
		return -EINVAL;
	}

	reg_sched = sched;
	reg_sched->init();

	return 0;
}

int scheduler_enqueue(struct thread_queue *from_q)
{
	bug_on(!reg_sched->enqueue);
	return reg_sched->enqueue(from_q);
}

/*
 * Move the CURRENT thread to the tail of to_q.
 */
void scheduler_dequeue(struct thread_queue *to_q)
{
	bug_on(!reg_sched->dequeue);

	reg_sched->dequeue(to_q);
}

void scheduler_tick()
{
	bug_on(!reg_sched->tick);

	reg_sched->tick();
}

void scheduler_yield()
{
	bug_on(!reg_sched->yield);

	reg_sched->yield();
}

void schedule()
{
	bug_on(!reg_sched->schedule);

	reg_sched->schedule();
}

void scheduler_start()
{
	bug_on(!reg_sched->start);

	sched_active = 1;
	reg_sched->start();
}

void scheduler_stop()
{
	bug_on(!reg_sched->stop);

	reg_sched->stop();
	sched_active = 0;
}

void scheduler_init()
{
	KLIST0_INIT(&thread_list.threads);
	spinlock_init(&thread_list.lock);
	need_reschedule = 0;
}

