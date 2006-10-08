
/*
 * arch/i386/console.c
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#include <kuca.h>
#include <console.h>

/* this is BAD and subject to be FIXED */
#include <arch/serial_console.h>
#include <arch/vga_console.h>

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

void console_put_char(char c)
{	
#if CONFIG_SERIAL_CONSOLE
	early_serial_put_char(c);
#endif /* CONFIG_SERIAL_CONSOLE */

#if CONFIG_VGA_CONSOLE
	early_vga_put_char(c);
#endif /* CONFIG_VGA_CONSOLE */
}
	
