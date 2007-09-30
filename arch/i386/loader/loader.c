/*
 * arch/i386/loader/loader.c
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
