
/*
 * arch/i386/isr.c
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

extern void general_protection(void);
extern void page_fault(void);

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
