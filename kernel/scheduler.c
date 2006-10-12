
/*
 * arch/i386/scheduler.c
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#include <kuca.h>
#include <irq.h>
#include <textio.h>
#include <lib.h>
#include <klist0.h>
#include <error.h>
#include <scheduler.h>
#include <thread.h>
#include <bug.h>

static int scheduler_is_active = 0;
struct klist0_node thread_list;
struct klist0_node krunqueue;

static void list_threads()
{
	struct klist0_node *t;
	struct thread_t *thread;
	int i = 0;

	klist0_for_each(t, &thread_list) {
		thread = klist0_entry(t, struct thread_t, kthreads);
		printf("# pid=%d, state=%x\n", thread->pid, thread->state);
		if (i++ > 10) bug();
	}
}

int scheduler_add_to_queue(struct thread_t * thread)
{
	disable_interrupts();

	dump_thread(thread);
	if (!klist0_empty(&thread->krunq))
		bug();
	klist0_append(&thread->krunq, &krunqueue);
	/*list_threads();*/

	enable_interrupts();

        return 0;
}

struct thread_t *scheduler_get_next_thread()
{
	struct klist0_node *t = &(CURRENT()->krunq);
	struct thread_t *thread;

	printf("%s: current=%s\n", __FUNCTION__, CURRENT()->name);
	do {
		t = t->next;
		thread = klist0_entry(t,
				struct thread_t, krunq);
	} while (t->next != &krunqueue && !(thread->state & THREAD_RUNNABLE));
	printf("%s: next=%s\n", __FUNCTION__, thread->name);
	return thread;
	list_threads();
	printf("selecting a thread to run...");
	/* start searching from the thread which is next to current;
	 * at least one thread should be runnable */
	klist0_for_each(t, &krunqueue) {
		thread = klist0_entry(t, struct thread_t, krunq);
		if (thread->state & THREAD_RUNNABLE)
			break;
	}
	printf("pid=%d\n", thread->pid);

        return thread;
}

void scheduler_start()
{
	scheduler_is_active = 1;
}

void scheduler_stop()
{
	scheduler_is_active = 0;
}

void scheduler_init()
{
	KLIST0_INIT(&thread_list);
	KLIST0_INIT(&krunqueue);
}

void scheduler_wake()
{
	if(scheduler_is_active)
		thread_reschedule();
}

