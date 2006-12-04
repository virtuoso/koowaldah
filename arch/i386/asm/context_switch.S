/*
 *
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
 *  3. Neither the name of the Koowaldah developers nor the names of theyr 
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

.globl do_thread_switch_context
do_thread_switch_context:
	pushf 
	pusha

	movl 0x28(%esp), %eax
	movl %esp, (%eax)
	movl 0x2c(%esp), %eax
	movl (%eax), %esp
	popa
	popf 
	ret

/*
 * This functions does not save the current conteext. Its only purpose is to 
 * run main kernel thread during system initialization.
 */

.globl do_thread_switch_to
do_thread_switch_to:
	pushf
	pusha
	movl 0x28(%esp), %eax
	movl %eax, %esp
	popa
	popf
	ret
	
/* XXX: stuff hardcoded until we have a proper elf parser */
.globl start_user
start_user:
	pushl %ebp
	movl %esp, %ebp

	movl $(USER_DATA)+3, %eax
	pushl %eax

	movl $0x40001ffb, %eax
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

