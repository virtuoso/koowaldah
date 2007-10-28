
/*
 * include/scheduler.h
 *
 * Copyright (C) 2005 Alexey Zaytsev
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
 * 3. Neither the name of the Koowaldah developers nor the names of their 
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
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
