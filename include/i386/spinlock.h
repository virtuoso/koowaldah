
/*
 * include/spinlock.h
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
