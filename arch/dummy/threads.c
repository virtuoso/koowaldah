/*
 * arch/dummy/threads.c
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
#include <dummy/loader.h>

void display_thread()
{
}

extern unsigned int sys_call(
	unsigned int sys,
	unsigned int arg0,
	unsigned int arg1,
	unsigned int arg2,
	unsigned int arg3,
	unsigned int arg4,
	unsigned int arg5
	);

void start_user(u32 eip, u32 ebp, u32 esp, u32 val)
{
	void (*func)(void *, void *, void *) __regparm(3) = (void *)eip;
	func(&sys_call, &sys_call, &sys_call);
	for (;;);
}

void thread_init_stack(struct thread *t, thread_t func, void *data)
{
	dummy_create_thread((void *)t, (void *)t->context.placeholder,
			func, data);
}

void thread_switch_context(struct thread *from, struct thread *to)
{
	dummy_switch_context((void *)from->context.placeholder,
			(void *)to->context.placeholder);
}

void thread_switch_to(struct thread *thread)
{
	dummy_start((void *)thread->context.placeholder);
}

