/*
 * include/dummy/loader.h
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
void *dummy_get_mem_base();

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

