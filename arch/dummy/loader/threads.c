/*
 * arch/dummy/loader/threads.c
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

