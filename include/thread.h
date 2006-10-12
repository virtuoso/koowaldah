
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
#include <klist0.h>

#define THREAD_NAME_LEN 32

/* thread state flags */
#define THREAD_RUNNABLE (0x1) /* can be selected for execution */
#define THREAD_NEW      (0x2) /* being created */
#define THREAD_ILL      (0x4) /* screwed up */
#define THREAD_WAIT     (0x8) /* sleeping on condition */

struct thread_t {
	/* all threads on the system */
	struct klist0_node			kthreads;
	/* threads in our runqueue */
	struct klist0_node			krunq;
	union {
		struct x86_thread_context	x86;
		/* whatever else thread_context */
	} ctx;
	u32					pid;
	u32					state;
	char					name[THREAD_NAME_LEN];
};

//int thread_init();
struct thread_t * thread_get_current();
struct thread_t * thread_create(void (*func)(), char *name);
void thread_switch_to(struct thread_t * thread);
	
void thread_reschedule();


#endif /* __THREAD_H__ */
