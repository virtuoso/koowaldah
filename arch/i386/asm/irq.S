/*
 * arch/i386/asm/irq.S
 *
 * Copyright (C) 2005, 2006 Alexey Zaytsev
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

.extern main_irq_handler

.extern need_reschedule

#define irqhandle(n)		\
.globl irq_pre_handler ## n;	\
irq_pre_handler ## n:		\
	pusha;			\
	pushl $##n;		\
	jmp do_handle_irq

irqhandle(0)
irqhandle(1)
irqhandle(2)
irqhandle(3)
irqhandle(4)
irqhandle(5)
irqhandle(6)
irqhandle(7)
irqhandle(8)
irqhandle(9)
irqhandle(10)
irqhandle(11)
irqhandle(12)
irqhandle(13)
irqhandle(14)
irqhandle(15)

do_handle_irq:
	call main_irq_handler
	popl %eax
	movl need_reschedule, %eax
	test %eax, %eax
	jz quit
	xor %eax, %eax
	movl %eax, need_reschedule
	push %esp
	call prepare_deffered_context_switch
	movl %eax, %esp
quit:
	popa
	iret

