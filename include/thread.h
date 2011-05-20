
/*
 * include/thread.h
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

#ifndef __THREAD_H__
#define __THREAD_H__

#include <arch/thread.h>
#include <spinlock.h>
#include <klist0.h>
#include <namespace.h>
#include <bitmap.h>

#define THREAD_NAME_LEN 32
#define MAX_THREADS 128

/* Thread state flags. What are they for? */
#define THREAD_RUNNABLE (0x1) /* can be selected for execution */
#define THREAD_NEW      (0x2) /* being created */
#define THREAD_ILL      (0x4) /* screwed up */
#define THREAD_WAIT     (0x8) /* sleeping on condition */

typedef unsigned short prio_t;
typedef void (*thread_t)(void *);

struct thread_queue {
	spinlock_t lock;
	struct klist0_node threads;
};

struct thread {
	struct klist0_node			kthreads;
	struct thread_queue			*krunq_head;
	struct klist0_node			krunq;
	u32					quantum;
	u64 					last_tick;
	prio_t					prio;

	/* architecture-dependent thread info. */
	struct thread_context			context;

	/* memory mapping */
	struct mapping				*map;

	/* process related */
	u32					pid;
	u32					state;
	char					name[THREAD_NAME_LEN];
	/* fs related */
	struct namespace			*ns;
	struct klist0_node			files; /* or array? */

	int					last_fd;
	BITMAP(fdset, FS_MAX_FDS);

	struct klist0_node                      mbox;
};

static __inline void tq_init(struct thread_queue *q)
{
	KLIST0_INIT(&q->threads);
	spinlock_init(&q->lock);
}

static __inline int tq_is_empty(struct thread_queue *q)
{
	bug_on(!q);
	return klist0_empty(&q->threads);
}

/*
 * Put a thread into a thread queue'a head.
 */
static __inline void __tq_insert_head(struct thread *t, struct thread_queue *q)
{
	bug_on(!klist0_empty(&t->krunq));
	bug_on(spin_trylock(&q->lock));

	klist0_append(&t->krunq, &q->threads);
	t->krunq_head = q;
}

/*
 * Put a thread into a thread queue's tail.
 */
static __inline void __tq_insert_tail(struct thread *t, struct thread_queue *q)
{
	bug_on(!klist0_empty(&t->krunq));
	bug_on(spin_trylock(&q->lock));

	klist0_prepend(&t->krunq, &q->threads);
	t->krunq_head = q;
}

/*
 * Remove a thread from it's queue.
 */
static __inline void __tq_remove_thread(struct thread *t)
{
	bug_on(!t->krunq_head);
	bug_on(klist0_empty(&t->krunq));

	klist0_unlink(&t->krunq);
	t->krunq_head = NULL;
}

/*
 * Dequeue a thread from the queue's head.
 */
static __inline struct thread * __tq_remove_head(struct thread_queue *q)
{
	struct thread *t = NULL;

	if (!klist0_empty(&q->threads)) {
		t = klist0_entry(q->threads.next, struct thread, krunq);
		__tq_remove_thread(t);
	}

	return t;
}

/*
 * Dequeue a thread from the queue's tail.
 */
static __inline struct thread * __tq_remove_tail(struct thread_queue *q)
{
	struct thread *t = NULL;

	if (!klist0_empty(&q->threads)) {
		t = klist0_entry(q->threads.prev, struct thread, krunq);
		__tq_remove_thread(t);
	}

	return t;
}

void tq_insert_head(struct thread *t, struct thread_queue *q);
void tq_insert_tail(struct thread *t, struct thread_queue *q);
struct thread *tq_remove_head(struct thread_queue *q);
struct thread *tq_remove_tail(struct thread_queue *q);
int tq_transfer_head_to_head(struct thread_queue *from_q, struct thread_queue *to_q);
int tq_transfer_head_to_tail(struct thread_queue *from_q, struct thread_queue *to_q);
int tq_transfer_tail_to_head(struct thread_queue *from_q, struct thread_queue *to_q);
int tq_transfer_tail_to_tail(struct thread_queue *from_q, struct thread_queue *to_q);

void dump_thread(struct thread *thread);
struct thread *thread_get(pid_t pid);
struct thread *thread_create(thread_t func, char *name, void *data);
struct thread *thread_create_user(thread_t func, char *name, void *data);
pid_t fork();
int thread_exec(struct thread *thread, char *path);
int thread_exec_new(struct thread *thread, struct inode *inode);
int spawn(char *path);
void exit(int code);

/* arch */
void thread_init_stack(struct thread *t, thread_t func, void *data);
void thread_switch_to(struct thread *thread) __noreturn;
void thread_switch_context(struct thread *from, struct thread *to);
void __noreturn start_user(u32 eip, u32 ebp, u32 esp, u32 val);

#endif /* __THREAD_H__ */
