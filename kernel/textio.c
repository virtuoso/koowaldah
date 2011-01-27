
/*
 * kernel/textio.c
 *
 * Copyright (C) 2005 Alexander Shishckin
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


#include <koowaldah.h>
#include <textio.h>
#include <lib.h>
#include <console.h>

static void __noprof print(const char *message)
{
	while (*message)
		console_put_char(*(message++));
}

void __noprof hex_dump(char *ptr, u32 len)
{
	int i;
	u32 *lbuf;

	ptr = (char *)((u32)ptr & ~0xfUL);
	len = (len + 15) & ~0xfUL;

	lbuf = (u32 *)ptr;
	for (i = 0; i < len/4; i++) {
		if (!(i % 4)) /*0, 16, 32...*/
			kprintf("\n[%08x]: ", ptr + i * 4);
		
		kprintf("%08x ", lbuf[i]);
#if 0
		if (!(i % 4))  /* 0, 4, 8, 12...*/
			kprintf("  %02x", ptr[i] & 0xff);
		else if (!((i+ 1) % 16)) /* 15, 31, 47...*/
			kprintf(" %02x\n", ptr[i] & 0xff);
		else
			kprintf(" %02x", ptr[i] & 0xff);
#endif
	}
	kprintf("\n");
}

int __noprof kprintf(const char *fmt, ...)
{
	va_list args;
	int i;
	char buf[OPT_KPRINTF_BUFSIZE];

	va_start(args, fmt);
	i = vsnprintf(buf, OPT_KPRINTF_BUFSIZE, fmt, args);
	va_end(args);
	print(buf);

	return i;
}

