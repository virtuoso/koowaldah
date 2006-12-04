
/*
 * arch/i386/pic.c
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
#include <arch/asm.h>
#include <arch/pic.h>
#include <textio.h>

#define PIC1_BASE_PORT 0x20
#define PIC2_BASE_PORT 0x2A

#define PIC_EOI 0x20

static u8 pic1_mask;
static u8 pic2_mask;

void pic_clear(){
	
	outb(PIC1_BASE_PORT, 0x11);
	outb(PIC2_BASE_PORT, 0x11);

	outb(PIC1_BASE_PORT + 1, 0x20);
	outb(PIC2_BASE_PORT + 1, 0x28);

	outb(PIC1_BASE_PORT + 1, 0x04);
	outb(PIC2_BASE_PORT + 1, 0x02);

	outb(PIC1_BASE_PORT + 1, 0x01);
	outb(PIC2_BASE_PORT + 1, 0x01);

	outb(PIC1_BASE_PORT + 1, 0xFF);
	outb(PIC2_BASE_PORT + 1, 0xFF);

	pic1_mask = 0xFF;
	pic2_mask = 0xFF;
}

void pic_mask_interrupt(u32 number){
	u8 tmp;
	
//	kprintf("pic_mask_interrupt(%d)\n", number);
//	kprintf("pic1_mask = %x, pic2_mask = %x\n", pic1_mask, pic2_mask);
	
	if(number > 15)
		return;
	if(number > 7){
		number -= 8;
		tmp = 1 << number;
		pic2_mask |= tmp;
		outb(PIC2_BASE_PORT + 1, pic2_mask);
	}else{
		tmp = 1 << number;
		pic1_mask |= tmp;
		outb(PIC1_BASE_PORT + 1, pic1_mask);
	}

//	kprintf("after running pic1_mask = %x, pic2_mask = %x\n", pic1_mask, pic2_mask);
	
}

void pic_unmask_interrupt(u32 number){
	u8 tmp;

//	kprintf("pic_unmask_interrupt(%d)\n", number);
//	kprintf("pic1_mask = %x, pic2_mask = %x\n", pic1_mask, pic2_mask);
	
	if(number > 15)
		return;
	if(number > 7){
		number -= 8;
		tmp = 1 << number;
		tmp = ~tmp;
		pic2_mask &= tmp;
		outb(PIC2_BASE_PORT + 1, pic2_mask);
	}else{
		tmp = 1 << number;
		tmp = ~tmp;
		pic1_mask &= tmp;
		outb(PIC1_BASE_PORT + 1, pic1_mask);
	}
//	kprintf("after running pic1_mask = %x, pic2_mask = %x\n", pic1_mask, pic2_mask);
}

inline void pic_do_eoi(u32 number){
	if(number > 15)
		return;
	
	outb(PIC1_BASE_PORT, PIC_EOI);
	
	if(number >= 8)
		outb(PIC2_BASE_PORT, PIC_EOI);
}


