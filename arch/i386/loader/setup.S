/*
 * arch/i386/loader/setup.S
 *
 * Copyright (C) 2007 Alexander Shishkin
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Koowaldah developers nor the names of their
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE DEVELOPERS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE DEVELOPERS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
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

