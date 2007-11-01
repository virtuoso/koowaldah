/*
 * include/mutex.h
 *
 * Copyright (C) 2007 Alexander Shishkin
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
 * THIS SOFTWARE IS PROVIDED BY THE DEVELOPERS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE DEVELOPERS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <scheduler.h>
#include <atomic.h>

struct mutex {
	atomic_u32          lock;
	struct thread_queue sleepers;
};

static __inline void mutex_init(struct mutex *mutex)
{
	atomic_test_and_set_u32(&mutex->lock, 1);
	tq_init(&mutex->sleepers);
}

/*
 * Try to acquire a mutex.
 * Returns 0 on success, 1 otherwise.
 */
static __inline int mutex_trylock(struct mutex *mutex)
{
	return (!atomic_test_and_set_u32(&mutex->lock, 0));
}

/*
 * Acquire a mutex, sleep if already locked by another thread.
 */
static __inline void mutex_lock(struct mutex *mutex)
{
doors:
	if (mutex_trylock(mutex)) {
		/* mutex was locked when we came to ask */
		scheduler_dequeue(&mutex->sleepers);
		scheduler_yield();

		goto doors;
	}
}

/*
 * Release a mutex.
 */	
static __inline void mutex_unlock(struct mutex *mutex)
{
	/* bail if mutex was unlocked */
	bug_on(!mutex_trylock(mutex));

	/* wake the first sleeper */
	scheduler_enqueue(&mutex->sleepers);
	atomic_set_u32(&mutex->lock, 1);
}

#endif

