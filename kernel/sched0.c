
/*
 * kernel/sched0.c
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

