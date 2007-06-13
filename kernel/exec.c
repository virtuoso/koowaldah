/*
 * kernel/exec.c
 *
 * Copyright (C) 2007 Alexander Shishkin
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
 * THIS SOFTWARE IS PROVIDED BY THE DEVELOPERS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE DEVELOPERS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <koowaldah.h>
#include <thread.h>
#include <scheduler.h>
#include <debug.h>
#include <error.h>
#include <namespace.h>
#include <mem_area.h>
#include <aout.h>

/*
 * This function starts in kernel on userspace thread's behalf
 * to prepare stacks and registers and jump to userspace.
 * Clobbers it's own stack frame, never returns.
 */
static void __noreturn user_thread(void *data)
{
	/* disregard everything on the current stack
	 * as start_user() never returns */
	reset_stack();
	start_user(USERMEM_START,
			USERMEM_STACK + PAGE_SIZE - 4,
			USERMEM_STACK + PAGE_SIZE - 4,
			0);
	bug();
}

/*
 * Load a binary with given path into a thread
 * @thread -- thread to be blessed
 * @inode  -- inode containing the executable
 * Note: it is strongly advised to remove any existing mem_area objects
 *       from the thread's mapping before calling this.
 */
int thread_exec_new(struct thread *thread, struct inode *inode)
{
	struct mapping *map = thread->map;
	int ret;

	/* load the binary
	 * this normally allocates 2 mmas: .text and .data [0] [1] */
	ret = aout_load(inode, map);
	if (ret)
		return ret;
	
	/* allocate stack [2] */
	map->m_mma[map->m_nmma] = mem_area_alloc_new(map,
			USERMEM_STACK, 1, MMA_RW | MMA_STACK);
	map->m_mma[map->m_nmma]->m_sizelim = USERMEM_STACKLIM;
	map->m_nmma++;
	
	/* allocate heap [3] */
	map->m_mma[map->m_nmma] = mem_area_alloc_new(map,
			USERMEM_HEAP, 1, MMA_RW | MMA_GROWS);
	map->m_nmma++;

	return 0;
}

/*
 * Start a new userspace process with the given binary
 * @path -- where the binary is located
 * This is needed for starting processes on kernel's demand,
 * like init.
 */
int spawn(char *path)
{
	struct thread *thread;
	struct thread_queue tmp_q;
	struct direntry *dent;
	struct inode *inode;
	int ret;

	dent = lookup_path(path);
	if (!dent) {
		DPRINT("%s not found.\n", path);
		return -ENOENT;
	}

	inode = dent->d_inode;

        thread = thread_create_user(&user_thread, path, NULL);
        if (!thread) {
                DPRINT("failed to create thread\n");
		return -ENOMEM;
        }

	ret = thread_exec_new(thread, inode);
	if (ret)
		return ret;

	tq_init(&tmp_q);
	tq_insert_head(thread, &tmp_q);

        bug_on(!scheduler_enqueue(&tmp_q));
	return 0;
}

/*
 * Execute a binary in the given thread.
 * Most commonly used in the exec* syscall family where thread == CURRENT().
 * @thread -- the thread to be substituted
 * @path   -- path to binary to be executed
 */
int thread_exec(struct thread *thread, char *path)
{
	struct direntry *dent;
	struct inode *inode;
	int i;

	dent = lookup_path(path);
	if (!dent) {
		DPRINT("%s not found.\n", path);
		return -ENOENT;
	}

	inode = dent->d_inode;

	/* get rid of current memory areas */
	for (i = 0; i < thread->map->m_nmma; i++) {
		mem_area_put(thread->map->m_mma[i], thread->map);
		thread->map->m_mma[i] = NULL;
	}

	thread->map->m_nmma = 0;

	i = thread_exec_new(thread, inode);
	if (i) return i;

	if (thread == CURRENT()) {
		/* ensure TLB flush since the mapping has changed badly */
		switch_map(CURRENT()->map, thread->map);
		user_thread(NULL);
	} else
		thread_init_stack(thread, user_thread, NULL);
	
	return 0;
}

