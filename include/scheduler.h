
/*
 * include/scheduler.h
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <kuca.h>
#include <thread.h> /* for struct thread_t */
#include <klist0.h>

#define SCHED_MAX_QUANTUM 100
#define SCHED_MIN_QUANTUM 1

extern struct klist0_node thread_list;

/*
 * Methods of a cpu scheduler and common data structures
 */
struct cpu_scheduler {
	const char *name[KCOMPONENT_NAME_LEN];
	void (*enqueue)(struct thread_t *thread);
	void (*dequeue)(struct thread_t *thread);
	void (*tick)();
	void (*yield)();
	void (*schedule)();
	void (*start)();
	void (*stop)();
	void (*init)();
	void (*exit)();
};

int register_cpusched(struct cpu_scheduler *sched);
/* int deregister_cpusched() to come as soon as it's needed */
int scheduler_enqueue(struct thread_t *thread);
int scheduler_dequeue(struct thread_t *thread);
void scheduler_yield();
void schedule();

#endif /* __SCHEDULER_H__ */
