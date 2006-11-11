
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
 */

#include <koowaldah.h>

#include <irq.h>
#include <lib.h>
#include <arch/pic.h>
#include <arch/isr.h>
#include <irq.h>
#include <scheduler.h>
#include <textio.h>

extern void irq_pre_handler0();
extern void irq_pre_handler1();
extern void irq_pre_handler2();
extern void irq_pre_handler3();
extern void irq_pre_handler4();
extern void irq_pre_handler5();
extern void irq_pre_handler6();
extern void irq_pre_handler7();
extern void irq_pre_handler8();
extern void irq_pre_handler9();
extern void irq_pre_handler10();
extern void irq_pre_handler11();
extern void irq_pre_handler12();
extern void irq_pre_handler13();
extern void irq_pre_handler14();
extern void irq_pre_handler15();
extern void irq_pre_handler16();

static void (*irq_handler_table[16])(unsigned int number);

void __init interrupts_init()
{

	memory_set(irq_handler_table, 0, sizeof(irq_handler_table));

	disable_interrupts();

	pic_clear();

	isr_insert_entry(32, irq_pre_handler0);
	isr_insert_entry(33, irq_pre_handler1);
	isr_insert_entry(34, irq_pre_handler2);
	isr_insert_entry(35, irq_pre_handler3);
	isr_insert_entry(36, irq_pre_handler4);
	isr_insert_entry(37, irq_pre_handler5);
	isr_insert_entry(38, irq_pre_handler6);
	isr_insert_entry(39, irq_pre_handler7);
	isr_insert_entry(40, irq_pre_handler8);
	isr_insert_entry(41, irq_pre_handler9);
	isr_insert_entry(42, irq_pre_handler10);
	isr_insert_entry(43, irq_pre_handler11);
	isr_insert_entry(44, irq_pre_handler12);
	isr_insert_entry(45, irq_pre_handler13);
	isr_insert_entry(46, irq_pre_handler14);
	isr_insert_entry(47, irq_pre_handler15);
	
	enable_interrupts();
}

void main_irq_handler(u32 number){
//	kprintf("Got irq %d\n", number);
	if(irq_handler_table[number]){
//		kprintf("Found handler, executing...");
		irq_handler_table[number](number);
//		kprintf("Done\n");
	}else{
//		kprintf("No irq handler found.\n");
	}
	pic_do_eoi(number);
	scheduler_tick();
}

int register_irq_handler(u32 number, void (*handler)(u32 number)){
	
	if(irq_handler_table[number])
		return 1;		/* first unregister previous handler */
	
	irq_handler_table[number] = handler;
	pic_unmask_interrupt(number);
	return 0;
}

void unregister_irq_handler(u32 number){
	pic_mask_interrupt(number);
	irq_handler_table[number] = NULL;
}



