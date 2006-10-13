
/*
 * arch/i386/irq.c
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#include <kuca.h>

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
//	printf("Got irq %d\n", number);
	if(irq_handler_table[number]){
//		printf("Found handler, executing...");
		irq_handler_table[number](number);
//		printf("Done\n");
	}else{
//		printf("No irq handler found.\n");
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

inline void disable_interrupts()
{
        asm("cli");
}

inline void enable_interrupts()
{
        asm("sti");
}


