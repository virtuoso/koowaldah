
/*
 * kernel/thread_queue.c
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

