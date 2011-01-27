
/*
 * kernel/thread_queue.c
 *
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
 */

#include <koowaldah.h>
#include <spinlock.h>
#include <thread.h>
#include <bug.h>

/*
 * Insert a thread into a queue's head.
 */
void tq_insert_head(struct thread *t, struct thread_queue *q)
{
	u32 flags;

	spin_lock_irqsave(&q->lock, flags);
	__tq_insert_head(t, q);
	spin_unlock_irqrestore(&q->lock, flags);
}

/*
 * Insert a thread into a queue's tail.
 */
void tq_insert_tail(struct thread *t, struct thread_queue *q)
{
	u32 flags;

	spin_lock_irqsave(&q->lock, flags);
	__tq_insert_tail(t, q);
	spin_unlock_irqrestore(&q->lock, flags);
}

struct thread *tq_remove_head(struct thread_queue *q)
{
	u32 flags;
	struct thread *t;

	spin_lock_irqsave(&q->lock, flags);
	t = __tq_remove_head(q);
	spin_unlock_irqrestore(&q->lock, flags);

	return t;
}

struct thread *tq_remove_tail(struct thread_queue *q)
{
	u32 flags;
	struct thread *t;

	spin_lock_irqsave(&q->lock, flags);
	t = __tq_remove_tail(q);
	spin_unlock_irqrestore(&q->lock, flags);

	return t;
}

/*
 * Move a thread from one queue's head to an other queue's head.
 */
int tq_transfer_head_to_head(struct thread_queue *from_q, struct thread_queue *to_q)
{
	u32 flags;
	struct thread *t;

	local_irq_save(flags);

	spin_lock(&from_q->lock);
	t = __tq_remove_head(from_q);
	spin_unlock(&from_q->lock);

	if (t) {

		spin_lock(&to_q->lock);
		__tq_insert_head(t, to_q);
		spin_unlock(&to_q->lock);
	}

	local_irq_restore(flags);

	return !!t;
}

/*
 * Move a thread from one queue's head to an other queue's tail.
 */
int tq_transfer_head_to_tail(struct thread_queue *from_q, struct thread_queue *to_q)
{
	u32 flags;
	struct thread *t;

	local_irq_save(flags);

	spin_lock(&from_q->lock);
	t = __tq_remove_head(from_q);
	spin_unlock(&from_q->lock);

	if (t) {
		spin_lock(&to_q->lock);
		__tq_insert_tail(t, to_q);
		spin_unlock(&to_q->lock);
	}

	local_irq_restore(flags);

	return !!t;
}

/*
 * Move a thread from one queue's tail to an other queue's head.
 */
int tq_transfer_tail_to_head(struct thread_queue *from_q, struct thread_queue *to_q)
{
	u32 flags;
	struct thread *t;

	local_irq_save(flags);

	spin_lock(&from_q->lock);
	t = __tq_remove_tail(from_q);
	spin_unlock(&from_q->lock);

	if (t) {
		spin_lock(&to_q->lock);
		__tq_insert_head(t, to_q);
		spin_unlock(&to_q->lock);
	}

	local_irq_restore(flags);

	return !!t;
}
/*
 * Move a thread from one queue's tail to an other queue's tail.
 */
int tq_transfer_tail_to_tail(struct thread_queue *from_q, struct thread_queue *to_q)
{
	u32 flags;
	struct thread *t;

	local_irq_save(flags);

	spin_lock(&from_q->lock);
	t = __tq_remove_tail(from_q);
	spin_unlock(&from_q->lock);

	if (t) {
		spin_lock(&to_q->lock);
		__tq_insert_tail(t, to_q);
		spin_unlock(&to_q->lock);
	}

	local_irq_restore(flags);

	return !!t;
}

