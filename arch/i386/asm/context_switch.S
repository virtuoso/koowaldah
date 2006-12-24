
/*
 *  arch/i386/context_switch.S
 *
 *  Copyright (C) 2006 Alexey Zaytsev
 *  
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 * 
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the Koowaldah developers nor the names of their 
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 * 
 *  THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
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

/* XXX: stuff hardcoded until we have a proper elf parser */
.globl start_user
start_user:
	pushl %ebp
	movl %esp, %ebp

	movl $(USER_DATA)+3, %eax
	pushl %eax

	movl $0x40002ffb, %eax
	pushl %eax

	movl $(USER_CODE)+3, %eax
	pushl %eax

	movl $0x40000000, %eax
	pushl %eax

	movl $(USER_DATA)+3, %ebx
	mov %bx, %ds
	mov %bx, %es

	lret

/*
 * start_user_forked(eip, ebp, esp, val)
 */
.globl start_user_forked
.type start_user_forked,%function
.align 4
start_user_forked:
	pushl %ebp
	movl %esp, %ebp

	movl $(USER_DATA)+3, %eax
	pushl %eax

	movl 16(%ebp), %eax /* stack ptr */
	pushl %eax

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

	lret

