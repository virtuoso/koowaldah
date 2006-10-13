
/*
 * arch/i386/pic.c
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#include <kuca.h>
#include <arch/asm.h>
#include <arch/pic.h>
#include <textio.h>

#define PIC1_BASE_PORT 0x20
#define PIC2_BASE_PORT 0x2A

#define PIC_EOI 0x20

static u8 pic1_mask;
static u8 pic2_mask;

void pic_clear(){
	
	io_port_out(PIC1_BASE_PORT, 0x11);
	io_port_out(PIC2_BASE_PORT, 0x11);

	io_port_out(PIC1_BASE_PORT + 1, 0x20);
	io_port_out(PIC2_BASE_PORT + 1, 0x28);

	io_port_out(PIC1_BASE_PORT + 1, 0x04);
	io_port_out(PIC2_BASE_PORT + 1, 0x02);

	io_port_out(PIC1_BASE_PORT + 1, 0x01);
	io_port_out(PIC2_BASE_PORT + 1, 0x01);

	io_port_out(PIC1_BASE_PORT + 1, 0xFF);
	io_port_out(PIC2_BASE_PORT + 1, 0xFF);

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
		io_port_out(PIC2_BASE_PORT + 1, pic2_mask);
	}else{
		tmp = 1 << number;
		pic1_mask |= tmp;
		io_port_out(PIC1_BASE_PORT + 1, pic1_mask);
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
		io_port_out(PIC2_BASE_PORT + 1, pic2_mask);
	}else{
		tmp = 1 << number;
		tmp = ~tmp;
		pic1_mask &= tmp;
		io_port_out(PIC1_BASE_PORT + 1, pic1_mask);
	}
//	kprintf("after running pic1_mask = %x, pic2_mask = %x\n", pic1_mask, pic2_mask);
}

inline void pic_do_eoi(u32 number){
	if(number > 15)
		return;
	
	io_port_out(PIC1_BASE_PORT, PIC_EOI);
	
	if(number >= 8)
		io_port_out(PIC2_BASE_PORT, PIC_EOI);
}


