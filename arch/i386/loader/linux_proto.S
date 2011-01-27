/*
 * arch/i386/loader/linux_proto.S
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

