
/*
 * arch/i386/serial_console.c
 *
 * Copyright (C) 2005 Alexey Zaytsev
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
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
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

