/*
 * kernel/sched0.c
 *
 * Author: Alexander Shishkin, 2006
 *
 * Trivial cpu scheduler implementation and a best skeleton ever.
 */

#include <kuca.h>
#include <scheduler.h>
#include <bug.h>
#include <textio.h>
#include <thread.h>
#include <klist0.h>

extern u64 jiffies;

struct sched0_runqueue {
	/* struct thread_t, linked via krunq */
	struct klist0_node queue;
	u32 count;
};

static struct sched0_runqueue rq;

void sched0_enqueue(struct thread_t *thread)
{
	thread->quantum = SCHED_MAX_QUANTUM;
	klist0_append(&thread->krunq, &rq.queue);
	rq.count++;
}

void sched0_dequeue(struct thread_t *thread)
{
	rq.count--;
	thread->quantum = SCHED_MIN_QUANTUM;
}

/* fairly simple */
void sched0_tick()
{
	struct thread_t *thread = CURRENT();
	struct thread_t *next;


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
					struct thread_t, krunq);
		else
			next = klist0_entry(thread->krunq.next,
					struct thread_t, krunq);
		thread->last_tick = jiffies;
		/* in fact, we always have at least idle thread available */
		thread_switch_context(thread, next);
	}
}

void sched0_yield()
{
	struct thread_t *cur = CURRENT();
	struct thread_t *next = klist0_entry(rq.queue.next,
			struct thread_t, krunq);

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

