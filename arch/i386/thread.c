
/*
 * arch/i386/thread.c
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
 * x86-specific context-switching functionality
 *
 */

#include <koowaldah.h>
#include <textio.h>
#include <thread.h>
#include <scheduler.h>
#include <lib.h>
#include <error.h>
#include <bug.h>
#include <mm_zone.h>
#include <i386/asm.h>
#include <i386/segments.h>

struct tss_segment root_tss;
void __regparm(2) do_thread_switch_context(void *from, void *to);
void __regparm(1) do_thread_switch_to(void *to);

void display_thread()
{
	struct thread *	t = CURRENT();
	kprintf("Task: %s, pid: %d\n", t->name,	t->pid);
}

void thread_init_stack(struct thread *t, thread_t func, void *data)
{
	u32 *stack = (u32 *) t - 1;
	/* stack pointers */
	t->context.stack_base = (u32) stack;
	t->context.esp = (u32) stack;

	int i;
	/* See arch/i386/asm/context_switch.S */
	*stack-- = (u32)data;
	*stack-- = 0;
	*stack-- = read_eflags() | EFLAGS_IF;
	*stack-- = KERN_CODE;
	*stack = (u32)func;
	for (i = 0; i < 8; i++)
		*--stack = 0;

	t->context.esp = (u32) stack;
	t->state &= ~THREAD_NEW;
}

void thread_switch_context(struct thread *from, struct thread *to)
{

	u32 *esp_from;
	u32 *esp_to;

#if 0
	u32 esp;
	kprintf("Switching context\n");

	kprintf("from: stack_base = %x, esp = %x, pid = %d\n",
			from->context.stack_base,
			from.context->esp, from->pid);
	kprintf("to: stack_base = %x, esp = %x, pid = %d\n",
			to.context->stack_base, to->context.esp, to->pid);

	__asm__ __volatile__("mov %%esp, %0" : "=m"(esp));

	kprintf("esp = 0x%x\n", esp);

	kprintf("esp[0] = 0x%x\n", *((u32 *) esp));
	kprintf("esp[1] = 0x%x\n", *((u32 *) esp + 1));
	kprintf("esp[2] = 0x%x\n", *((u32 *) esp + 2));
	kprintf("esp[3] = 0x%x\n", *((u32 *) esp + 3));
#endif

	esp_from = &from->context.esp;
	esp_to = &to->context.esp;
	root_tss.esp0 = to->context.stack_base;

	if (from->map != to->map)
		switch_map(from->map, to->map);
	
	do_thread_switch_context(esp_from, esp_to);
}

u32 prepare_deffered_context_switch(u32 curr_esp)
{
	struct thread *from = CURRENT();
	struct thread *to = next_thread;

#if 0
	kprintf("Deffered context switch\n");

	kprintf("from: stack_base = %x, esp = %x, pid = %d\n",
			from->context.stack_base,
			from->context.esp, from->pid);
	kprintf("to: stack_base = %x, esp = %x, pid = %d\n",
			to->context.stack_base, to->context.esp,
			to->pid);

	kprintf("esp[0] = 0x%x\n", *((u32 *) curr_esp));
	kprintf("esp[1] = 0x%x\n", *((u32 *) curr_esp + 1));
	kprintf("esp[2] = 0x%x\n", *((u32 *) curr_esp + 2));
	kprintf("esp[3] = 0x%x\n", *((u32 *) curr_esp + 3));

#endif
	if (from->map != to->map)
		switch_map(from->map, to->map);

	from->context.esp = curr_esp;
	root_tss.esp0 = to->context.stack_base;

	return to->context.esp;
}

void thread_switch_to(struct thread *thread)
{
/*
	kprintf("esp[0] = 0x%x\n", *((u32 *)tctx(thread).esp));
	kprintf("esp[1] = 0x%x\n", *((u32 *)tctx(thread).esp + 1));
	kprintf("esp[2] = 0x%x\n", *((u32 *)tctx(thread).esp + 2));
	kprintf("esp[3] = 0x%x\n", *((u32 *)tctx(thread).esp + 3));
*/
	do_thread_switch_to((u32 *)thread->context.esp);
}

