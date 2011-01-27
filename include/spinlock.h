
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

#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__

#include <koowaldah.h>
#include <arch/spinlock.h>
#include <arch/irq.h>

#define spin_lock_irqsave(lock, flags) do { 	\
	local_irq_save(flags);			\
	spin_lock(lock);			\
} while (0)

#define spin_unlock_irqrestore(lock, flags) do {\
	spin_unlock(lock);			\
	local_irq_restore(flags);		\
} while (0)


static __inline int spin_trylock_irqsave(spinlock_t *lock, u32 flags)
{
	local_irq_save(flags);
	if (!spin_trylock(lock))
		return 0;
	local_irq_restore(flags);
	return 1;
}

#endif /* __SPINLOCK_H__ */
