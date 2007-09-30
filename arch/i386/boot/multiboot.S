/*
 * arch/i386/boot/multiboot.S
 *
 * Copyright (C) 2005 Alexey Zaytsev
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

.extern kern_start
.extern multiboot_info
.globl multiboot_start
.type multiboot_start, @function

/* setting up the Multiboot header - see GRUB docs for details */
#define MODULEALIGN (0x01)           /* align loaded modules on page boundaries */
#define MEMINFO     (0x02)           /* provide memory map */
#define FLAGS       (MODULEALIGN | MEMINFO) /* this is the Multiboot 'flag' field */
#define MAGIC       0x1BADB002       /* 'magic number' lets bootloader find the header */
#define CHECKSUM    -(MAGIC + FLAGS) /* checksum required */

.section ".text","ax"
.align 4

MultiBootHeader:
	.long MAGIC
	.long FLAGS
	.long CHECKSUM

multiboot_start:
	cli
	movl %ebx, (multiboot_info)
	movl $(kernel_stack), %esp
	lgdt gdt_desc		/* load gdt */
	ljmp $KERN_CODE, $l1	/* set %cs */
l1:	movl $KERN_DATA, %eax	/* set the rest */
	movl %eax, %ds
	movl %eax, %es
	movl %eax, %fs
	movl %eax, %gs
	movl %eax, %ss
	lidt idt_desc
	jmp kern_start
	cli
	hlt
	.size multiboot_start, . - multiboot_start

/* GDT size and ptr */
gdt_desc:
	.word GDT_ENTRIES * 8 - 1
	.long gdt_table

/* the GDT */
.globl gdt_table
.align 16
gdt_table:
	/* null entry */
	SEGDESC_EMPTY
	/* kernel CS (KERN_CODE) */
	SEGDESC(0, 0x0fffff, ST_RX_CODE, DPL_KERN)
	/* kernel DS (KERN_DATA) */
	SEGDESC(0, 0x0fffff, ST_RW_DATA, DPL_KERN)
	/* user CS (USER_CODE) */
	SEGDESC(0, 0x0fffff, ST_RX_CODE, DPL_USER)
	/* user DS (USER_DATA) */
	SEGDESC(0, 0x0fffff, ST_RW_DATA, DPL_USER)
	/* TSS */
	SEGDESC_EMPTY
	/* call gate (CALL_GATE) */
	SEGDESC_EMPTY

/* IDT size and ptr */
idt_desc:
	.word IDT_ENTRIES * 8 - 1
	.long idt_table

/* the IDT */
.globl idt_table
idt_table:
	.fill IDT_ENTRIES,8,0

.align 4
__kernel_stack_start:
	.fill 0x1000,1,0
.equ kernel_stack,. - 4

