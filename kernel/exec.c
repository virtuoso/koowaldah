/*
 * kernel/exec.c
 *
 * Copyright (C) 2007 Alexander Shishkin
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
 */

#include <koowaldah.h>
#include <lib.h>
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
			USERMEM_STACK - 4,
			USERMEM_STACK - 4,
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
#ifndef OPT_CPU_ARCH_DUMMY
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
#endif

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

	/* rename the thread accordingly */
	snprintf(thread->name, THREAD_NAME_LEN, path);

	if (thread == CURRENT()) {
		/* ensure TLB flush since the mapping has changed badly */
		switch_map(CURRENT()->map, thread->map);
		user_thread(NULL);
	} else
		thread_init_stack(thread, user_thread, NULL);
	
	return 0;
}

