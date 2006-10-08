
/*
 * include/thread.h
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#ifndef __THREAD_H__
#define __THREAD_H__
#include <arch/thread.h>
#include <mm.h>

/* thread state flags */
#define THREAD_RUNNING (0x1) /* otherwise sleeping */
#define THREAD_NEW     (0x2) /* being created */
#define THREAD_ILL     (0x4) /* screwed up */

struct thread_t {
	union {
		struct x86_thread_context	x86;
		/* whatever else thread_context */
	} ctx;
	u32					pid;
	u32					state;
};

//int thread_init();
struct thread_t * thread_get_current();
struct thread_t * thread_create(void (*func)());
void thread_switch_to(struct thread_t * thread);
	
void thread_reschedule();


#endif /* __THREAD_H__ */
