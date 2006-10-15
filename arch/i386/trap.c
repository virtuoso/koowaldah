
/*
 * arch/i386/trap.c
 *
 * Copyright (C) 2006 Alexander Shishkin
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
 * Ok, now we can generate exceptions, so let's fucking handle 'em right.
 */

#include <kuca.h>
#include <textio.h>
#include <irq.h>
#include <console.h>
#include <bug.h>
#include <mm.h>
#include <thread.h>

void psod()
{
	u32 frame;
	struct thread_t *thread;
	int i;

	for (i = 0; i < 80; i++) console_put_char('-');

	__asm__ __volatile__("mov %%esp, %0" : "=m"(frame));
	thread = CURRENT();

	kprintf("Task %d, stack base: %x, esp: %x\n",
			thread->pid,
			tctx(thread).stack_base,
			tctx(thread).esp);

	frame = tctx(thread).esp;
	kprintf("Stack length: %d dwords\n", ((u32)thread - frame)/4);
	i = 0;
	//while ((frame + i*4) < (u32)thread) {
	for (i = 0; i < ((u32)thread - frame)/4; i++) {
		kprintf(" (%d) %x", i, (u32)((u32 *)frame)[i]);
		if (i && !(i % 8)) kprintf("\n");
	}
}

void general_protection()
{
	kprintf("\nGENERAL PROTECTION FAULT\n");
	psod();
	for (;;) __asm__ __volatile__("hlt; nop");
}

void page_fault()
{
	kprintf("\nPAGE FAULT\n");
	psod();
	for (;;) __asm__ __volatile__("hlt; nop");
}

void __init trap_init(void)
{
//	set_trap_handler(13, general_protection);
}

