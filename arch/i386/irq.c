
/*
 * arch/i386/irq.c
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
 */

#include <koowaldah.h>
#include <machine.h>
#include <bug.h>
#include <irq.h>
#include <lib.h>
#include <i386/pic.h>
#include <irq.h>
#include <scheduler.h>
#include <textio.h>
#include <i386/segments.h>

void irq_pre_handler0();
void irq_pre_handler1();
void irq_pre_handler2();
void irq_pre_handler3();
void irq_pre_handler4();
void irq_pre_handler5();
void irq_pre_handler6();
void irq_pre_handler7();
void irq_pre_handler8();
void irq_pre_handler9();
void irq_pre_handler10();
void irq_pre_handler11();
void irq_pre_handler12();
void irq_pre_handler13();
void irq_pre_handler14();
void irq_pre_handler15();
void irq_pre_handler16();

static void (*irq_handler_table[16])(unsigned int number);

void __init interrupts_init()
{
	pic_clear();

	intgate_init(32, irq_pre_handler0);
	intgate_init(33, irq_pre_handler1);
	intgate_init(34, irq_pre_handler2);
	intgate_init(35, irq_pre_handler3);
	intgate_init(36, irq_pre_handler4);
	intgate_init(37, irq_pre_handler5);
	intgate_init(38, irq_pre_handler6);
	intgate_init(39, irq_pre_handler7);
	intgate_init(40, irq_pre_handler8);
	intgate_init(41, irq_pre_handler9);
	intgate_init(42, irq_pre_handler10);
	intgate_init(43, irq_pre_handler11);
	intgate_init(44, irq_pre_handler12);
	intgate_init(45, irq_pre_handler13);
	intgate_init(46, irq_pre_handler14);
	intgate_init(47, irq_pre_handler15);
}

void main_irq_handler(u32 number)
{
	if(irq_handler_table[number])
		irq_handler_table[number](number);

	pic_do_eoi(number);
	if (mach_state == MACH_RUNNING)
		scheduler_tick();
}

int register_irq_handler(u32 number, void (*handler)(u32 number))
{

	if (irq_handler_table[number])
		return 1;		/* first unregister previous handler */

	irq_handler_table[number] = handler;
	pic_unmask_interrupt(number);
	return 0;
}

void unregister_irq_handler(u32 number)
{
	pic_mask_interrupt(number);
	irq_handler_table[number] = NULL;
}

