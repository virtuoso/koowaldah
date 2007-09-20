/*
 * include/dummy/loader.h
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

/*
 * kos-loader interface definition
 */

#ifndef __DUMMY_LOADER_H__
#define __DUMMY_LOADER_H__

#include <thread.h>
#include <mem_area.h>
#include <mm_zone.h>

/*
 * return the amount of available 'physical' memory allocated
 * by loader for the kernel, in bytes
 */
int dummy_get_mem_size();

/*
 * this comes right from libc: outputs a single char to stdout
 */
extern void dummy_putc(char);

/*
 * stub to make a real noreturn function
 */
__noreturn void dummy_noreturn();

/*
 * create a thread context
 * @t    -- pointer to 'struct thread' object
 * @tc   -- libc's 'ucontext_t' object (inside the thread object:
 *          we can't reliably reference it from 't' in the loader)
 * @func -- thread function pointer
 * @data -- argument to be passed to the thread function
 */
void dummy_create_thread(void *t, void *tc, thread_t func, void *data);

/*
 * start a 'userspace' code
 * @pc -- entry point address
 * @sp -- pointer to the top of user's stack
 */
__noreturn void dummy_start_user(u32 pc, u32 sp);

/*
 * start a newly created thread
 * @tc -- ucontext_t userspace context
 */
void dummy_start(void *tc);

/*
 * switch from one thread context to another
 * @tc_old -- pointer to current thread's context (ucontext_t)
 * @tc_new -- context of the thread to switch to
 */
void dummy_switch_context(void *tc_old, void *tc_new);

#endif

