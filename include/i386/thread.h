/*
 * include/i386/thread.h
 * Architecture-specific context-switching data
 * (C) blah-blah
 */

#ifndef __ARCH_THREAD_H__
#define __ARCH_THREAD_H__

#include <i386/i386.h>

struct x86_thread_context {
	u32 *stack_base;
	u32 esp;
};

#define tctx(t) ((t)->ctx.x86)

#define THREAD_STACK_LIMIT PAGE_SIZE
#define THREAD(s) ( { \
	unsigned long __q = (unsigned long)s; \
	__q &= ~(PAGE_SIZE - 1); \
	__q = __q + PAGE_SIZE - sizeof(struct thread_t); \
	(struct thread_t *) __q; } )
#define CURRENT() ( { \
	unsigned long __r; \
	__asm__ __volatile__("mov %%esp, %0" : "=m"(__r)); \
	THREAD(__r); } )

#endif
