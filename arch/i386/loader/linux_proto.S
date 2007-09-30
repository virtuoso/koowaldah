/*
 * arch/i386/loader/linux_proto.S
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

.code16
.extern verstr

/*
 * Padding for the place where bootsector code should have been
 * and it's not.
 */
.section ".init.bs", "a"
	.fill 0x1f1, 1, 0

.section ".init.hdr", "ax"
.globl header, realmode_switch, start_sys, type_of_loader, code32_start

header:
/*
 * Following is the x86 linux boot proto header,
 * refer to linux kernel documentation
 */
	.byte 7        # setup_sects
	.word 1        # root_flags
	.word KERNSZ >> 4 # syssize
	.word 0        # padding for syssize
	.word 0        # ram_size
	.word 0        # vid_mode
	.word 0        # root_dev
	.word 0xaa55   # boot_flag
	.byte 0xeb, tramp - 1f  # jump
1:	.ascii "HdrS"  # header
	.word 0x0206   # version
realmode_switch:
	.long 0        # realmode_switch
start_sys:
	.word 0x1000   # start_sys
	.word verstr - 0x200 # kernel_version
type_of_loader:
	.byte 0        # type_of_loader
	.byte 0        # loadflags (LOADED_HIGH)
	.word 0        # setup_move_size
code32_start:
	.long 0x10000 # code32_start
	.long 0        # ramdisk_image
	.long 0        # ramdisk_size
	.long 0        # bootsect_kludge
	.word 0        # heap_end_ptr
	.word 0        # padding for heap_end_ptr
	.long 0        # cmd_line_ptr
	.long 0        # initrd_addr_max
	.long 0        # kernel_alignment
	.byte 0        # relocatable_kernel
	.byte 0
	.word 0        # padding for relocatable_kernel
	.long 0        # cmdline_size

.extern pre_start_loader
tramp:
	/* reset the harddrive */
	movw $0x0000, %ax
	movb $0x80, %dl
	int $0x13

	/* normalize %cs */
	pushw %ds
	pushw $pre_start_loader
	lretw

