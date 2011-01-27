
/*
 * arch/i386/irq.c
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

