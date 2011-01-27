
/*
 * include/i386/thread.h
 *
 * Copyright (C) 2005 Alexey Zaytsev
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
 * Architecture-specific context-switching data
 *
 */

#ifndef __ARCH_THREAD_H__
#define __ARCH_THREAD_H__

#include <i386/i386.h>
#include <i386/asm.h>
#include <i386/segments.h>

struct thread_context {
	u32 stack_base;
	u32 esp;
};

#define THREAD_STACK_LIMIT PAGE_SIZE
#define THREAD(s) ( { \
	unsigned long __q = (unsigned long)s; \
	__q &= ~(PAGE_SIZE - 1); \
	__q = __q + PAGE_SIZE - sizeof(struct thread); \
	(struct thread *) __q; } )
#define CURRENT() ( { \
	unsigned long __r; \
	__asm__ __volatile__("mov %%esp, %0" : "=m"(__r)); \
	THREAD(__r); } )

/* initial value for per-thread eflags register */
#define EFLAGS_IF	0x0200

#define reset_stack() \
	do { \
		CURRENT()->context.esp = (u32)CURRENT() - 4; \
		root_tss.esp0 = (u32)CURRENT() - 4; \
	} while (0);

#endif
