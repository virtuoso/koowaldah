
/*
 * include/spinlock.h
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

/*
 * On UP systems, we don't need to lock anything. If the kernel were preemptive,
 * we would have to disable the preemption, but that's all. The spinlock holding
 * thread may only be preempted with an irq handler, and we use spinlock_irq_save
 * to lock data accessed by irq handlers. The counter is there only for debugging,
 * so it isn't even atomic.
 */

#ifndef __ARCH_SPINLOCK_H__
#define __ARCH_SPINLOCK_H__

#include <koowaldah.h>
#include <bug.h>

typedef struct {
#ifdef OPT_DEBUG_SPINLOCKS
	u32 counter;
#endif
} spinlock_t;



#ifdef OPT_DEBUG_SPINLOCKS

#define SPINLOCK(name) spinlock_t name = {0};
#define spinlock_init(lock) do { (lock)->counter = 0; } while (0)

#define spin_lock(lock) do {		\
	__typeof__(lock) __lock = (lock);	\
	bug_on(__lock->counter != 0);	\
	__lock->counter++;		\
} while (0)

#define spin_unlock(lock) do {		\
	__typeof__(lock) __lock = (lock);	\
	bug_on(__lock->counter != 1);	\
	__lock->counter--;		\
} while (0)

static __inline int spin_trylock(spinlock_t *lock)
{
	bug_on(lock->counter != 0 && lock->counter != 1);

	if (lock->counter)
		return 0;

	lock->counter++;
	return 1;
}

#else /* !OPT_DEBUG_SPINLOCKS */

#define SPINLOCK(name) spinlock_t name;
#define spinlock_init(lock) do {	\
	spinlock_t __future *l = lock;	\
} while (0)

#define spin_lock(lock) do {		\
	spinlock_t __future *l = lock;	\
} while (0)

#define spin_unlock(lock) do {		\
	spinlock_t __future *l = lock;	\
} while (0)

static __inline int spin_trylock(spinlock_t *lock)
{
	return 1;
}

#endif /* OPT_DEBUG_SPINLOCKS */

#endif /* __ARCH_SPINLOCK_H__ */
