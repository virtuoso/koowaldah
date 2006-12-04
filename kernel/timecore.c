/*
 * kernel/timecore.c
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
 * Basic kernel timer implementation
 * (at least I can sleep after this)
 *
 */

#include <koowaldah.h>
#include <klist0.h>
#include <textio.h>
#include <irq.h>
#include <error.h>
#include <mm.h>
#include <page_alloc.h>
#include <thread.h>
#include <scheduler.h>

static struct klist0_node timers;

typedef void (*timer_fn)(void *data);

struct timer {
	struct klist0_node t_list;
	timer_fn	func;
	u32		counter;
	void		*data;
};

#define _TIMER(p) ((struct timer *)p)

void __init timers_init()
{
	KLIST0_INIT(&timers);
}

int register_timer(timer_fn func, u32 delay, void *data)
{
	struct timer *timer;

	/* lock here, shall we? */
	disable_interrupts();

	timer = memory_alloc(sizeof(struct timer));
	if (!timer) {
		enable_interrupts();
		return ERR_NOMEMORY;
	}

	timer->func = func;
	timer->counter = delay;
	timer->data = data;

        klist0_append(&timer->t_list, &timers);

	enable_interrupts();

        return 0;
}

void __noprof update_timers()
{
        struct klist0_node *t, *d;
	struct timer *timer;

        if (!klist0_empty(&timers)) {
		disable_interrupts();

		t = timers.next;
		do {
			timer = klist0_entry(t, struct timer, t_list);
			if (!--timer->counter) {
				d = t; t = t->next;
				klist0_unlink(d);
				timer->func(timer->data);
				memory_release(timer);
			} else
				t = t->next;
                } while (t != &timers);

		enable_interrupts();
        }
}

static void tsleep_alarm(void *data)
{
	struct thread *thread = (struct thread *)data;

	scheduler_enqueue_nolock(thread);
	thread->state = THREAD_RUNNABLE;
}

/* sleep on timer ticks */
int tsleep(u32 delay)
{
	struct thread *thread = CURRENT();
	int ret;

	disable_interrupts();

	/* remove the thread from scheduler's candidates */
	thread->state = 0;
	scheduler_dequeue_nolock(thread);
	//thread->state &= ~THREAD_RUNNABLE;
	//thread->state = THREAD_WAIT;

	/* this will wake us up after /at least/ delay ticks */
	ret = register_timer(tsleep_alarm, delay, (void *)thread);

	/* quit running immediately */
	if (!ret) {
		enable_interrupts();
		scheduler_yield();
	} else
		enable_interrupts();

	return ret;
}

