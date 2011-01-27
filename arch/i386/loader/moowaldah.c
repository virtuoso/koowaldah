/*
 * arch/i386/loader/moowaldah.c
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
 */

#include "loader.h"

#include "multiboot_info.h"
struct multiboot_info_t __data32 multiboot_info;

void __text32 setup_multiboot()
{
	multiboot_info.mem_upper = 32768;
}

#include SYMTAB_PATH

/*
 * Prepare to execute the kernel
 */
void __text32 start_kernel()
{
	unsigned long *src = 0x11000;
	unsigned long *dst = 0x100000;
	int count = KERNSZ >> 2;

	/* copy the kernel to 1mb */
	for (count = KERNSZ * 4; count--; *dst++ = *src++);

	setup_multiboot();

	__asm__ __volatile__("jmp *%1" :
			: "b"(&multiboot_info), "r"(KOS_ENTRY));
}

