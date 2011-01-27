/*
 * arch/i386/asm/interrupt.S
 *
 * Copyright (C) 2006 Alexander Shishkin
 * Copyright (C) 2007 Alexey Zaytsev
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

	
