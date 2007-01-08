/*
 * arch/i386/asm/interrupt.S
 *
 * Copyright (C) 2006 Alexander Shishkin
 * Copyright (C) 2007 Alexey Zaytsev
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

.extern fault_code

/*
 * from syscalls.c:
 * u32 sys_call_gate(u32 eax, u32 ebx, u32 ecx, u32 edx)
 */
.extern sys_call_gate
.globl sys_call_entry
sys_call_entry:
	/* save stack and prepare arguments for sys_call_gate() */
	pushl %esp
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax

	call sys_call_gate

	/* leave eax intact for it holds the return value */
	popl %ebx		/* remove arguments */
	popl %ebx		/* remove arguments */
	popl %ecx		/* remove arguments */
	popl %edx		/* remove arguments */
	popl %esi		/* remove arguments */
	popl %edi		/* remove arguments */
	popl %ebp		/* remove arguments */
	popl %esp		/* remove arguments */
	iret

.extern ud2_handler
.globl ud2_entry
ud2_entry:
	pushl %esp
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax

1:	call ud2_handler
	/* They never return, but just in case */
	jmp 1

.extern pf_handler
.globl pf_entry
pf_entry:
	popl fault_code
	pushl %esp
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax

	call pf_handler

	popl %eax
	popl %ebx
	popl %ecx
	popl %edx
	popl %esi
	popl %edi
	popl %ebp
	popl %esp
	iret

	
