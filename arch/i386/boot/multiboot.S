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

#include <i386/asm.h>

.extern kern_start
.extern kernel_stack
.extern multiboot_info
.globl multiboot_start

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
	movl $(multiboot_info), %ebx
	movl $(kernel_stack), %esp
	lgdt gdt_desc		/* load gdt */
	ljmp $KERN_CODE, $l1	/* set %cs */
l1:	movl $KERN_DATA, %eax	/* set the rest */
	movl %eax, %ds
	movl %eax, %es
	movl %eax, %fs
	movl %eax, %gs
	movl %eax, %ss
	jmp kern_start
	cli
	hlt

/* GDT size and ptr */
gdt_desc:
	.word GDT_ENTRIES * 8 - 1
	.long gdt_table

/* the GDT */
.globl gdt_table
gdt_table:
	/* null entry */
	.quad 0x0000000000000000
	/* kernel CS (KERN_CODE) */
	.word 0xffff
	.word 0x0000
	.byte 0x00
	.byte 0x9a
	.byte 0xcf
	.byte 0x00
	/* kernel DS (KERN_DATA) */
	.word 0xffff
	.word 0x0000
	.byte 0x00
	.byte 0x92
	.byte 0xcf
	.byte 0x00
	/* user CS (USER_CODE) */
	.word 0xffff
	.word 0x0000
	.byte 0x00
	.byte 0xfa
	.byte 0xcf
	.byte 0x00
	/* user DS (USER_DATA) */
	.word 0xffff
	.word 0x0000
	.byte 0x00
	.byte 0xf2
	.byte 0xcf
	.byte 0x00

