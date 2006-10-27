
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
 * 3. Neither the name of the Koowaldah developers nor the names of theyr 
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

#include <kuca.h>
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

extern void do_thread_switch_context(u32 * from, u32 * to);
extern void do_thread_switch_to(u32 * to);

void thread_init_stack(struct thread_t * t, void (*func)(void))
{
	u32 *stack = (u32 *)(tctx(t).esp);
	int i;
	
	*stack-- = (u32)func;
	*stack = read_eflags();
	for(i = 0; i < 8; i++)
		*--stack = 0;

	tctx(t).esp = (u32)stack;
	t->state &= ~THREAD_NEW;
}

int __init thread_init()
{
        return 0;
}

struct thread_t * thread_get_current()
{
	struct thread_t * current;
	u32 esp;
	
	asm volatile ("movl %%esp, %0":"=m"(esp));

	esp &= ~0xFFF;

	if(!page_is_allocated((u32 *) esp))
		return NULL;
	
	current = THREAD(esp);

	/*kprintf("got current, stack_base = %x, esp = %x, pid = %d\n",
			(u32) current->ctx.stack_base, (u32) current->ctx.esp, current->pid);*/

	return CURRENT();
}

void thread_switch_context(struct thread_t * from, struct thread_t * to)
{

	u32 * esp_from;
	u32 * esp_to;

/*
	kprintf("Switching context\n");
	
	kprintf("from: stack_base = %x, esp = %x, pid = %d\n",
			(u32) from->ctx.stack_base, from->ctx.esp, from->pid);
	kprintf("to: stack_base = %x, esp = %x, pid = %d\n",
			(u32) to->ctx.stack_base, to->ctx.esp, to->pid);
*/
	
	if(from == to)
		return;

	esp_from = &tctx(from).esp;
	esp_to = &tctx(to).esp;

	if (from->map != to->map)
		switch_map(from->map, to->map);
	
	do_thread_switch_context(esp_from, esp_to);
}


void thread_switch_to(struct thread_t * thread)
{
	do_thread_switch_to((u32 *)tctx(thread).esp);
}

