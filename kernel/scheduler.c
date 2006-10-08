
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
#include <klist.h>
#include <error.h>
#include <scheduler.h>
#include <thread.h>

static int scheduler_is_active = 0;
static struct klist * thread_list = NULL;

int scheduler_add_to_queue(struct thread_t * thread)
{
        struct klist * tmp;

	disable_interrupts();

        tmp = klist_new();
        if(!tmp)
                return ERR_NOMEMORY;

        tmp->data = thread;
        klist_add(tmp, &thread_list);

	enable_interrupts();

        return 0;
}

struct thread_t * scheduler_get_next_thread()
{
        klist_iter(&thread_list);

        return thread_list->data;
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
	
}

void scheduler_wake()
{
	if(scheduler_is_active)
		thread_reschedule();
}

