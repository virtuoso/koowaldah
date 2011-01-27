/*
 * arch/i386/loader/setup.S
 *
 * Copyright (C) 2007 Alexander Shishkin
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

#include <i386/segments.h>

.code16

.section ".init.code", "ax"

/*
 * Reset segment registers and stack before jumping to C code
 */
.globl pre_start_loader
.type  pre_start_loader, @function
pre_start_loader:
	xorl %eax, %eax
	movw %cs, %ax
	movw %ax, %es
	movw %ax, %ss
	cld

	mov $0xfffc, %sp

	call start_loader

/*
 * Switch to protected mode and jump to 32-bit part
 */
.globl prot_switch
.type  prot_switch, @function
prot_switch:
	cli

	/* disable NMI */
	mov 0x80, %al
	outb %al, $0x70

	/* enable a20 */
	pushfl
	movw 0x2401, %ax
	int $0x15
	popfl

	/* setup boot-time descriptor tables */
	lidtl (boot_idt_desc)
	lgdtl (boot_gdt_desc)

	xorl %ecx, %ecx
	movl KERN_DATA, %ecx
	mov %cx, %ds
        mov %cx, %es
        mov %cx, %fs
        mov %cx, %gs
        mov %cx, %ss

	/* enable protected mode */
	movl %cr0, %ebx
	orl  0x01, %ebx
	movl %ebx, %cr0

	/* jump to protected-mode code */
	data32 ljmp $KERN_CODE, $start_kernel

.section ".init.data", "a"

/*
 * Descriptor tables
 */
.align 16
boot_gdt_table:
	SEGDESC_EMPTY
	SEGDESC(0, 0x0fffff, ST_RX_CODE, DPL_KERN)
	SEGDESC(0, 0x0fffff, ST_RW_DATA, DPL_KERN)

boot_gdt_desc:
	.word BOOT_GDT_ENTRIES * 8 - 1
	.word boot_gdt_table
	.word 0x0009 /* real-mode segment address */

boot_idt_desc:
	.word 0
	.long 0

