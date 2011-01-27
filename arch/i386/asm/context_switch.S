
/*
 *  arch/i386/context_switch.S
 *
 *  Copyright (C) 2006 Alexey Zaytsev
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

#include <i386/segments.h>

/*
 * The saved stack should look like:
 * | .......... |
 * | .......... |
 * |   EFLAGS   |
 * | SEGMENT ID |
 * |  RET ADDR  |
 * |    EAX     |
 * |    ECX     |
 * |    EDX     |
 * |    EBX     |
 * |    ESP     |
 * |    EBP     |
 * |    ESI     |
 * |    EDI     |
 * `------------
 *
 * Because we want both direct and deffered
 * context switches, and on interrupt, we get:
 *
 * | .......... |
 * | .......... |
 * |   EFLAGS   |
 * | SEGMENT ID |
 * |  RET ADDR  |
 * `------------
 *
 * we toss the stack in the direct switch case, because
 * it's considered to be the less frequent one.
 *
 */

.globl do_thread_switch_context
do_thread_switch_context:
/*
 * This function is marked with __attrubute__ ((regparm(2))), so it's
 * arguments are passed in %eax and %edx, and not on the stack.
 */
	pop %ebx
	pushf
	pushl %cs
	pushl %ebx
	pusha
	movl %esp, (%eax)	/* Save the current stack */
	movl (%edx), %esp	/* Load a new one */
	popa
	iret

/*
 * This function does not save the current context. Its only purpose is to
 * run the main kernel thread during system initialization.
 */

.globl do_thread_switch_to
do_thread_switch_to:
	/* Yes, we also use registers to pass params. */
	movl %eax, %esp
	popa
	iret

/*
 * start_user(eip, ebp, esp, val)
 */
.globl start_user
.type start_user,%function
.align 4
start_user:
	pushl %ebp
	movl %esp, %ebp

	movl $(USER_DATA)+3, %eax
	pushl %eax

	movl 16(%ebp), %eax /* stack ptr */
	pushl %eax
	pushfl

	movl $(USER_CODE)+3, %eax
	pushl %eax

	movl 8(%ebp), %eax  /* call address */
	pushl %eax

	movl 20(%ebp), %ecx /* return value */
	movl 12(%ebp), %eax /* set ebp */
	movl %eax, %ebp

	movl $(USER_DATA)+3, %ebx
	mov %bx, %ds
	mov %bx, %es
	movl %ecx, %eax

	iret

