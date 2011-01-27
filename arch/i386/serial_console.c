
/*
 * arch/i386/serial_console.c
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
#include <arch/asm.h>

#define SERIO_1_BASE	0x3F8

void __init early_serial_init()
{
	outb(SERIO_1_BASE + 3, 0x80); /* DLAB = 1 */

	outb(SERIO_1_BASE + 0, 12);
	outb(SERIO_1_BASE + 1, 0); /* set speed 115200 / 12 = 9600 */
	
	outb(SERIO_1_BASE + 3, 0x03); 	/* Bits = 8
					 * Stopbits = 1
					 * Flow control = none
					 * DLAB = 0
					 */
	outb(SERIO_1_BASE + 1, 0x00); /* Disable serial interrupts */
	outb(SERIO_1_BASE + 4, 0x00); /* No modem control */
} 

void early_serial_put_char(char c)
{
	outb(SERIO_1_BASE, c);
}

