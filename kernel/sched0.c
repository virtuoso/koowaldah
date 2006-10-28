/*
 * kernel/sched0.c
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
 * 3. Neither the name of the Koowaldah developers nor the names of theyr 
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
 * Trivial cpu scheduler implementation and a best skeleton ever.
 *
 */


#include <koowaldah.h>
#include <page_alloc.h>
#include <scheduler.h>
#include <bug.h>
#include <textio.h>
#include <thread.h>
#include <klist0.h>

extern u64 jiffies;

struct sched0_runqueue {
	/* struct thread, linked via krunq */
	struct klist0_node queue;
	u32 count;
};

static struct sched0_runqueue rq;

void sched0_enqueue(struct thread *thread)
{
	thread->quantum = SCHED_MAX_QUANTUM;
	klist0_append(&thread->krunq, &rq.queue);
	rq.count++;
}

void sched0_dequeue(struct thread *thread)
{
	rq.count--;
	thread->quantum = SCHED_MIN_QUANTUM;
}

/* fairly simple */
void __noprof sched0_tick()
{
	struct thread *thread = CURRENT();
	struct thread *next;


	/* this will hardly ever happen */
	if (klist0_empty(&rq.queue))
		bug();

	/* the thread ran out of time */
	if ((thread->state & THREAD_RUNNABLE) && !--thread->quantum) {
		thread->quantum = SCHED_MAX_QUANTUM;
		klist0_unlink(&thread->krunq);
		klist0_prepend(&thread->krunq, &rq.queue);

		if (thread->krunq.next == &rq.queue)
			next = klist0_entry(rq.queue.next,
					struct thread, krunq);
		else
			next = klist0_entry(thread->krunq.next,
					struct thread, krunq);
		thread->last_tick = jiffies;
		/* in fact, we always have at least idle thread available */
		thread_switch_context(thread, next);
	}
}

void sched0_yield()
{
	struct thread *cur = CURRENT();
	struct thread *next = klist0_entry(rq.queue.next,
			struct thread, krunq);

	next->quantum = MAX(next->quantum + cur->quantum, (u32)SCHED_MAX_QUANTUM);
	cur->quantum = SCHED_MIN_QUANTUM;
	cur->last_tick = jiffies;
	thread_switch_context(cur, next);
}

void sched0_schedule()
{
}

void sched0_start()
{
	sched0_yield();
}

void sched0_stop()
{
}

void sched0_init()
{
	KLIST0_INIT(&rq.queue);
	rq.count = 0;
}

void sched0_exit()
{
}

static struct cpu_scheduler sched0 = {
	.name     = {"sched0"},
	.enqueue  = sched0_enqueue,
	.dequeue  = sched0_dequeue,
	.tick     = sched0_tick,
	.yield    = sched0_yield,
	.schedule = sched0_schedule,
	.start    = sched0_start,
	.stop     = sched0_stop,
	.init     = sched0_init,
	.exit     = sched0_exit
};

void __init sched0_load()
{
	kprintf("Registering trivial cpu scheduler.\n");
	register_cpusched(&sched0);
}

