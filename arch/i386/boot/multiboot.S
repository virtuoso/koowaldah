/*
 * arch/i386/boot/multiboot.S
 *
 * Copyright (C) 2005 Alexey Zaytsev
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

