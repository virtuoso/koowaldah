
/*
 * include/thread.h
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

#ifndef __THREAD_H__
#define __THREAD_H__
#include <arch/thread.h>
#include <klist0.h>
#include <namespace.h>

#define THREAD_NAME_LEN 32
#define MAX_THREADS 128

/* thread state flags */
#define THREAD_RUNNABLE (0x1) /* can be selected for execution */
#define THREAD_NEW      (0x2) /* being created */
#define THREAD_ILL      (0x4) /* screwed up */
#define THREAD_WAIT     (0x8) /* sleeping on condition */

typedef unsigned short prio_t;
typedef void (*thread_t)(void *);

struct thread {
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
};

void dump_thread(struct thread *thread);
struct thread *thread_create(thread_t func, char *name, void *data);
struct thread *thread_create_user(thread_t func, char *name, void *data,
		u32 cp, u32 dp);
pid_t fork();

/* arch */
void thread_switch_to(struct thread *thread);
void thread_switch_context(struct thread *from, struct thread *to);
void start_user(void);
void start_user_forked(u32 eip, u32 ebp, u32 esp, u32 val);

#endif /* __THREAD_H__ */
