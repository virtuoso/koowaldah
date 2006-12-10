
/*
 * arch/i386/thread.c
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
 * x86-specific context-switching functionality
 *
 */

#include <koowaldah.h>
#include <textio.h>
#include <thread.h>
#include <scheduler.h>
#include <lib.h>
#include <klist.h>
#include <error.h>
#include <bug.h>
#include <mm.h>
#include <timer.h>
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

