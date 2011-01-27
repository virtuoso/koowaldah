/*
 * arch/i386/loader/loader.c
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

const char __initdata verstr[] = "Koowaldah BSD 0.1";

const char *loaders[] = {
	"LILO",
	"LoadLin",
	"bootsect-loader",
	"SYSLINUX",
	"EtherBoot",
	"ELILO",
	"undef", /* not defined in documentation */
	"GRUB",
	"U-BOOT",
	"Xen",
	"Gujin",
	"Qemu"
};
#define LOADERSSZ (sizeof loaders / sizeof (char *))

void start_loader()
{
	unsigned long reg;
	unsigned long ldr = type_of_loader >> 4;
	unsigned long ldrver = type_of_loader & 0x0f;

	lo_print("Anus!\n");
	lo_printf("%s loader starting.\r\n", verstr);

	if (ldr < LOADERSSZ)
		lo_printf("Loader: %s, version: %x\n", loaders[ldr], ldrver);
	else
		lo_printf("Unknown bootloader (%x).\n", ldr);

	if (realmode_switch)
		__asm__ __volatile__("lcallw *%0" : : "m"(realmode_switch) 
				: "eax", "ebx", "ecx", "edx");
	prot_switch();

	while (1)
		__asm__ __volatile__("hlt");
}

/*
 * vim:tw=78:ts=8:sw=8:noexpandtab
 */
