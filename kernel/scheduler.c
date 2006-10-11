
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

static int scheduler_is_active = 0;
static struct klist0_node thread_list;
static struct thread_t *current_thread = NULL;

int scheduler_add_to_queue(struct thread_t * thread)
{
	disable_interrupts();

	klist0_append(&thread->kthreads, &thread_list);
	if (/*unlikely*/!current_thread)
		current_thread = thread;

	enable_interrupts();

        return 0;
}

struct thread_t *scheduler_get_next_thread()
{
	struct klist0_node *t;
	struct thread_t *thread;

	/* start searching from the thread which is next to current;
	 * at least one thread should be runnable */
	klist0_for_each(t, &current_thread->kthreads) {
		thread = klist0_entry(t, struct thread_t, kthreads);
		if (thread->state & THREAD_RUNNABLE)
			break;
	}

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
}

void scheduler_wake()
{
	if(scheduler_is_active)
		thread_reschedule();
}

