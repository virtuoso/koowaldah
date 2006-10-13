
/*
 * arch/i386/isr.c
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */


/* 
 * Interrupts are used for irqs, exceptions and as the system entry point.
 * Here I place all the general code.
 */ 

#include <kuca.h>
#include <textio.h>

#include "isr_vector.h"

#define ISR_TABLE_SIZE	256
#define ISR_SEGMENT	0x08

typedef struct {
	u32	dword1;
	u32	dword2;
}isr_table_entry;

extern isr_table_entry isr_table[]; /* from arch/i386/asm/isr.S */

void default_interrupt_handler();

int isr_insert_entry(unsigned int number, void ( *handler)()){
	if(number > 255)
		return 1;
	
	if(!handler)
		handler = default_isr_table[number];

        isr_table[number].dword1 = ((u32)handler & 0xFFFF) | (ISR_SEGMENT << 16);
        isr_table[number].dword2 = ((u32)handler & 0xFFFF0000) | 0x8E00;

	return 0;
}

void __init isr_init(){
	int i;
	u32	idt_desc[2];
	
	for(i = 0; i < 256; i++)
		isr_insert_entry(i, NULL);

	idt_desc[0] = ISR_TABLE_SIZE * sizeof(isr_table_entry) +
		((((u32)isr_table) & 0xFFFF) << 16);
	idt_desc[1] = ((u32)isr_table) >> 16;
	
	asm volatile("lidt %0\n":"=m"(idt_desc));
}

void default_interrupt_handler(u32 num){
	//kprintf("Unhandled interrupt %d fired\n", num);
	switch (num) {
		case 13:
			general_protection();
			break;
		case 14:
			page_fault();
			break;
		case 80:
			kprintf("SYSCALL GATE!\n");
			break;
		default:
			break;
	}
}
