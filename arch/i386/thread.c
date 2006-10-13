
/*
 * arch/i386/thread.c
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 * x86-specific context-switching functionality
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

	/*printf("got current, stack_base = %x, esp = %x, pid = %d\n",
			(u32) current->ctx.stack_base, (u32) current->ctx.esp, current->pid);*/

	return CURRENT();
}

void thread_switch_context(struct thread_t * from, struct thread_t * to)
{

	u32 * esp_from;
	u32 * esp_to;

/*
	printf("Switching context\n");
	
	printf("from: stack_base = %x, esp = %x, pid = %d\n",
			(u32) from->ctx.stack_base, from->ctx.esp, from->pid);
	printf("to: stack_base = %x, esp = %x, pid = %d\n",
			(u32) to->ctx.stack_base, to->ctx.esp, to->pid);
*/
	
	if(from == to)
		return;

	esp_from = &tctx(from).esp;
	esp_to = &tctx(to).esp;
	
	do_thread_switch_context(esp_from, esp_to);
}


void thread_switch_to(struct thread_t * thread)
{
	do_thread_switch_to((u32 *)tctx(thread).esp);
}

