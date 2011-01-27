
/*
 * arch/i386/pic.c
 *
 * Copyright (C) 2005 Alexey Zaytsev
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
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

__inline void pic_do_eoi(u32 number){
	if(number > 15)
		return;
	
	outb(PIC1_BASE_PORT, PIC_EOI);
	
	if(number >= 8)
		outb(PIC2_BASE_PORT, PIC_EOI);
}


