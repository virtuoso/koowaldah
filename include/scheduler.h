
/*
 * include/scheduler.h
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
 */

#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <koowaldah.h>
#include <thread.h> /* for struct thread */

#define SCHED_MAX_QUANTUM 100
#define SCHED_MIN_QUANTUM 1

extern struct thread_queue thread_list;

/*
 * Methods of a cpu scheduler and common data structures
 */
struct cpu_scheduler {
	const char *name[KCOMPONENT_NAME_LEN];
	int (*enqueue)(struct thread_queue *from_q);
	void (*dequeue)(struct thread_queue *to_q);
	void (*tick)();
	void (*yield)();
	void (*schedule)();
	void (*start)();
	void (*stop)();
	void (*init)();
	void (*exit)();
};

extern u32 need_reschedule;
extern struct thread *next_thread;

int register_cpusched(struct cpu_scheduler *sched);
/* int deregister_cpusched() to come as soon as it's needed */
int scheduler_enqueue(struct thread_queue *from_q);
void scheduler_dequeue(struct thread_queue *to);
void scheduler_yield();
void scheduler_tick();
void schedule();
void scheduler_start();
void scheduler_stop();

/* debug: print diagnostics for kernel threads */
#define LIST_CURRENT   0x00000001 /* only show current thread */
#define LIST_BACKTRACE 0x00000002 /* include full call trace */
#define LIST_MAPPING   0x00000004 /* include memory mapping */
void list_threads(u32 ops);

#endif /* __SCHEDULER_H__ */
