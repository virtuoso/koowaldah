/*
 * include/mutex.h
 *
 * Copyright (C) 2007 Alexander Shishkin
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

