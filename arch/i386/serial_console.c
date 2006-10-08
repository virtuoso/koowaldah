
/*
 * arch/i386/serial_console.c
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#include <kuca.h>
#include <arch/serial_console.h>
#include <arch/asm.h>

#define SERIO_1_BASE	0x3F8

void __init early_serial_init()
{

	io_port_out(SERIO_1_BASE + 3, 0x80); /* DLAB = 1 */

	io_port_out(SERIO_1_BASE + 0, 12);
	io_port_out(SERIO_1_BASE + 1, 0); /* set speed 115200 / 12 = 9600 */
	
	io_port_out(SERIO_1_BASE + 3, 0x03); 	/* Bits = 8
					 * Stopbits = 1
					 * Flow control = none
					 * DLAB = 0
					 */
	io_port_out(SERIO_1_BASE + 1, 0x00); /* Disable serial interrupts */
	io_port_out(SERIO_1_BASE + 4, 0x00); /* No modem control */

} 

void early_serial_put_char(char c)
{
	io_port_out(SERIO_1_BASE, c);
}

