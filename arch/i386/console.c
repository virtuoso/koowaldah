
/*
 * arch/i386/console.c
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
 * 3. Neither the name of the Koowaldah developers nor the names of theyr 
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

#include <kuca.h>
#include <console.h>

extern void early_serial_init(void);
extern void early_vga_init(void);

/*
 * This is an early console in fact, we'll have a proper console
 * once mm is initialized
 */
int __init early_console_init()
{
#if CONFIG_SERIAL_CONSOLE
	early_serial_init();
#endif /* CONFIG_SERIAL_CONSOLE */

#if CONFIG_VGA_CONSOLE
	early_vga_init();
#endif /* CONFIG_VGA_CONSOLE */
	return 0;
}

extern void early_serial_put_char(char c);
extern void early_vga_put_char(char c);
void console_put_char(char c)
{	
#if CONFIG_SERIAL_CONSOLE
	early_serial_put_char(c);
#endif /* CONFIG_SERIAL_CONSOLE */

#if CONFIG_VGA_CONSOLE
	early_vga_put_char(c);
#endif /* CONFIG_VGA_CONSOLE */
}
	
