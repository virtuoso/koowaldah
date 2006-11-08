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
 */


#include <koowaldah.h>
#include <irq.h>
#include <textio.h>
#include <lib.h>
#include <klist0.h>
#include <sys/errno.h>
#include <scheduler.h>
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
struct klist0_node thread_list;

struct klist0_node krunqueue;

/*
 * List all threads in the system
 * Prints a lot of rubbish.
 */
void list_threads()
{
	struct klist0_node *t;
	struct thread *thread;
	int i = 0;

	klist0_for_each(t, &thread_list) {
		thread = klist0_entry(t, struct thread, kthreads);
		kprintf("# [%s] pid=%d, state=%x, last_tick=%d\n",
				thread->name, thread->pid,
				thread->state, thread->last_tick);
		if (i++ > MAX_THREADS) bug();
	}
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

/*
 * Add the thread to the execution queue.
 * Make sure it is not listed in any other run/waitqueue or
 * I will bite.
 */
int scheduler_enqueue(struct thread *thread)
{
	disable_interrupts();

	/* a thread should be removed from any run/waitqueues */
	if (!klist0_empty(&thread->krunq)) {
		dump_thread(thread);
		bug();
	}
	reg_sched->enqueue(thread);
	enable_interrupts();

	return 0;
}

/*
 * Dequeue the thread.
 */
int scheduler_dequeue(struct thread *thread)
{
	disable_interrupts();

	/* likewise */
	if (klist0_empty(&thread->krunq))
		bug();

	klist0_unlink(&thread->krunq);
	if (reg_sched->dequeue)
		reg_sched->dequeue(thread);

	enable_interrupts();

	return 0;
}

void scheduler_tick()
{
	/* disable_interrupts(); */
	reg_sched->tick();
	/* enable_interrupts(); */
}

void scheduler_yield()
{
	reg_sched->yield();
}

void schedule()
{
	reg_sched->schedule();
}

void scheduler_start()
{
	sched_active = 1;
	reg_sched->start();
}

void scheduler_stop()
{
	reg_sched->stop();
	sched_active = 0;
}

void scheduler_init()
{
	KLIST0_INIT(&thread_list);
}

