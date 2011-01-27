/*
 * arch/dummy/loader/threads.c
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ucontext.h>
#include <setjmp.h>
#include "loader.h"

/*
 * I must be the first pshycho to ever use ucontext stuff ever since
 * pthread library was introduced. This, however, works much better
 * than pthread for our in-kernel threading.
 */

/* Warning: this conflicts with kernel definition */
typedef void (*thread_t)(void);

/*
 * Create a thread context
 * @threadptr -- struct thread (on the stack page); this is needed
 *               to calculate remaining part of that page available
 *               for stack
 * @ucp       -- we keep ucontext_t data inside thread->context for
 *               convenience
 * @func      -- thread function
 * @data      -- argument to func
 */
EXPORT void dummy_create_thread(void *threadptr, ucontext_t *ucp,
		thread_t func, void *data)
{
	unsigned long stackaddr = (unsigned long)threadptr & 0xfffff000;

	memset(ucp, 0, sizeof(ucontext_t));
	ucp->uc_stack.ss_sp = (void *)stackaddr;
	ucp->uc_stack.ss_size = (unsigned long)threadptr - stackaddr;
	ucp->uc_link = NULL;
	makecontext(ucp, func, 1, data);
}

/*
 * Start a thread with given context
 * @ucp -- ucontext_t context descriptor of the thread
 */
EXPORT void dummy_start(ucontext_t *ucp)
{
	int r;

	r = setcontext(ucp);
	if (r == -1)
		perror("setcontext");
}

/*
 * Switch from one thread to another
 * @ucp_from -- context of current thread
 * @ucp_to   -- context of the thread we're switching to
 */
EXPORT void dummy_switch_context(ucontext_t *ucp_from, ucontext_t *ucp_to)
{
	int r;

	ucp_to->uc_link = NULL;
	r = swapcontext(ucp_from, ucp_to);
	if (r == -1)
		perror("swapcontext");
}

/*
 * Switch to userland execution
 * @pc -- instruction address
 * @sp -- stack pointer
 */
EXPORT void dummy_start_user(unsigned long pc, unsigned long sp)
{
	dummy_load_from_cpio(NULL);
}

