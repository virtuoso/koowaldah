
/*
 * kernel/sched0.c
 *
 * Copyright (C) 2006 Alexander Shishkin
 * Copyright (C) 2006 Alexey Zaytsev
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

struct sched0_runqueue { /* Not mush stuff here, yet. */
	struct thread_queue queue;
};

static struct sched0_runqueue rq;

int sched0_enqueue(struct thread_queue *from_q)
{
	struct thread *t;
	struct thread_queue *to_q = &rq.queue;
	u32 flags;

	local_irq_save(flags);

	spin_lock(&from_q->lock);
	t = __tq_remove_head(from_q);
	spin_unlock(&from_q->lock);

	if (t) {
		t->quantum = SCHED_MAX_QUANTUM;
		spin_lock(&to_q->lock);
		__tq_insert_head(t, &rq.queue);
		spin_unlock(&to_q->lock);
	}

	local_irq_restore(flags);

	return !!t;
}

void sched0_dequeue(struct thread_queue *to_q)
{
	u32 flags;
	struct thread *current, *next;
	struct thread_queue *from_q = &rq.queue;

	local_irq_save(flags);

	current = CURRENT();
	bug_on(current->krunq_head != from_q);

	spin_lock(&from_q->lock);
	__tq_remove_thread(current);
	next = __tq_remove_head(from_q);
	bug_on(!next);
	__tq_insert_tail(next, from_q);
	spin_unlock(&from_q->lock);

	current->quantum = SCHED_MAX_QUANTUM;
	current->last_tick = jiffies;

	spin_lock(&to_q->lock);
	__tq_insert_tail(current, to_q);
	spin_unlock(&to_q->lock);

	local_irq_restore(flags);
}

/* Is only called from interrupt context. */
void __noprof sched0_tick()
{
	struct thread *prev, *next;
	prev = CURRENT();

	bug_on(tq_is_empty(&rq.queue));

	if (!--prev->quantum) {
		prev->quantum = SCHED_MAX_QUANTUM;

		spin_lock(&rq.queue.lock);
		next = __tq_remove_head(&rq.queue);
		bug_on(!next);
		__tq_insert_tail(next, &rq.queue);
		spin_unlock(&rq.queue.lock);

		if (prev != next) {
			need_reschedule = 1;
			next_thread = next;
		}
	}

	prev->last_tick = jiffies;
}

void sched0_yield()
{
	struct thread *next, *prev;
	u32 flags;

	prev = CURRENT();

	prev->quantum = SCHED_MAX_QUANTUM;
	prev->last_tick = jiffies;

	spin_lock_irqsave(&rq.queue.lock, flags);
	next = __tq_remove_head(&rq.queue);
	bug_on(!next);
	__tq_insert_tail(next, &rq.queue);
	spin_unlock_irqrestore(&rq.queue.lock, flags);

	thread_switch_context(prev, next);
}

void sched0_schedule()
{
}

void sched0_start()
{
	/* sched0_yield(); */
}

void sched0_stop()
{
}

void sched0_init()
{
	tq_init(&rq.queue);
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

