
/*
 * arch/i386/console.c
 *
 * Copyright (C) 2005 Alexey Zaytsev
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
#include <console.h>

extern void early_serial_init(void);
extern void early_vga_init(void);

/*
 * This is an early console in fact, we'll have a proper console
 * once mm is initialized
 */
void __init early_console_init()
{
#ifdef OPT_EARLY_SERIAL_CONSOLE
	early_serial_init();
#endif

#ifdef OPT_EARLY_VGA_CONSOLE
	early_vga_init();
#endif
}

extern void early_serial_put_char(char c);
extern void early_vga_put_char(char c);
void console_put_char(char c)
{
#ifdef OPT_EARLY_SERIAL_CONSOLE
	early_serial_put_char(c);
#endif

#ifdef OPT_EARLY_VGA_CONSOLE
	early_vga_put_char(c);
#endif
}

