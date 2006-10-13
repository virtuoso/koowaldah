
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
#define MAX_THREADS 128

/* thread state flags */
#define THREAD_RUNNABLE (0x1) /* can be selected for execution */
#define THREAD_NEW      (0x2) /* being created */
#define THREAD_ILL      (0x4) /* screwed up */
#define THREAD_WAIT     (0x8) /* sleeping on condition */

typedef unsigned short prio_t;

struct thread_t {
	/* scheduler data */
	struct klist0_node			kthreads;
	struct klist0_node			krunq;
	u32					quantum;
	u64 					last_tick;
	prio_t					prio;

	/* architecture-dependent thread info */
	union {
		struct x86_thread_context	x86;
		/* whatever else thread_context */
	} ctx;

	/* process related */
	u32					pid;
	u32					state;
	char					name[THREAD_NAME_LEN];
};

void dump_thread(struct thread_t *thread);
struct thread_t *thread_create(void (*func)(), char *name);

/* arch */
void thread_switch_to(struct thread_t *thread);
void thread_switch_context(struct thread_t * from, struct thread_t * to);

#endif /* __THREAD_H__ */
