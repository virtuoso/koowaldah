/*
 * arch/i386/loader/vga.c
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

//#include <stdarg.h>
#include "loader.h"

void lo_putchar(char c)
{
	unsigned short ax = 0x0e00 | c;

	__asm__ __volatile__(
			"pusha; int $0x10; popa"
			: : "b"(0x0001), "c"(1), "a"(ax)
			);
}

void lo_print(const char *s)
{
	do {
		lo_putchar(*s);
		if (*s == '\n')
			lo_putchar('\r');
	} while (*++s);
}

#if 1
#include "klib.c"
#include "vsnprintf.c"
#endif

#define LO_PRINTF_BUFSIZE 1024

int lo_printf(const char *fmt, ...)
{
	va_list args;
	int i;
	char buf[LO_PRINTF_BUFSIZE];

	va_start(args, fmt);
	i = vsnprintf(buf, LO_PRINTF_BUFSIZE, fmt, args);
	va_end(args);
	lo_print(buf);

	return i;
}

