/*
 * kernel/timecore.c
 *
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
 */

#include <koowaldah.h>
#include <irq.h>
#include <thread.h>
#include <spinlock.h>
#include <scheduler.h>
#include <slice.h>
#include <klist0.h>
#include <error.h>
#include <timers.h>

struct time_interval {
	struct klist0_node interval_list;
	struct thread_queue wake_list;
	u32 ticks_left;
};

static struct klist0_node future;
static SPINLOCK(future_lock);

struct slice_pool *interval_pool;
struct slice_pool *tq_pool;

void __init timers_init()
{
	KLIST0_INIT(&future);
	spinlock_init(&future_lock);
	interval_pool = slice_pool_create(0, sizeof(struct time_interval));
	bug_on(!interval_pool);
}

void __noprof update_timers()
{
	struct time_interval *interval = NULL;

	spin_lock(&future_lock);

	if (!klist0_empty(&future)) {
		interval = klist0_entry(future.next, struct time_interval,
			interval_list);

		if (!interval->ticks_left--) {
			klist0_unlink(&interval->interval_list);
		} else
			interval = NULL;
	}
	spin_unlock(&future_lock);

	/* Maybe we want some bottom halve mechanism? */
	if (interval) {
		while(scheduler_enqueue(&interval->wake_list));
		slice_free(interval, interval_pool);
	}


}

int tsleep(u32 delay)
{
	struct time_interval *interval, *new_interval;
	struct klist0_node *tmp;

	u32 flags;

	spin_lock_irqsave(&future_lock, flags);

	klist0_for_each(tmp, &future) {
		interval = klist0_entry(tmp, struct time_interval,
			interval_list);
		if (delay < interval->ticks_left) {
			interval->ticks_left -= delay;

			new_interval = slice_alloc(interval_pool);
			if (!new_interval)
				return -ENOMEM;

			KLIST0_INIT(&new_interval->interval_list);
			tq_init(&new_interval->wake_list);
			new_interval->ticks_left = delay;

			klist0_prepend(&new_interval->interval_list,
				&interval->interval_list);

			scheduler_dequeue(&new_interval->wake_list);
			goto out;
		}
		if (delay == interval->ticks_left) {
			scheduler_dequeue(&new_interval->wake_list);
			goto out;
		}

		delay -= interval->ticks_left;
	}
	new_interval = slice_alloc(interval_pool);
	if (!new_interval)
		return -ENOMEM;

	KLIST0_INIT(&new_interval->interval_list);
	tq_init(&new_interval->wake_list);
	new_interval->ticks_left = delay;

	klist0_prepend(&new_interval->interval_list, &future);
	scheduler_dequeue(&new_interval->wake_list);
out:
	spin_unlock_irqrestore(&future_lock, flags);
	scheduler_yield();

	return 0;
}

