/*
 * kernel/init/tests_irqs.c
 *
 * Copyright (C) 2006 Alexey Zaytsev
 * Copyright (C) 2006 Alexander Shishkin
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
#include <bug.h>
#include <textio.h>
#include <irq.h>
#include <scheduler.h>
#include <arch/asm.h>

#ifdef OPT_TEST_IRQS
static void keyboard_irq_handler(u32 number)
{
        unsigned char c;

	c = inb(0x60);
	switch (c) {
		case 31:
			list_threads(LIST_BACKTRACE);
			break;
		case 32:
			bug();
			break;
		case 33:
			kprintf("Timer says %d\n", (u32)jiffies);
			break;
		default:
			kprintf("keyboard, scancode = %d\n", c);
			break;
	}
}
#endif /* OPT_TEST_IRQS */

void test_irqs()
{
#ifdef OPT_TEST_IRQS
        kprintf("Registering dummy keyboard interrupt service routine...");
        register_irq_handler(1, keyboard_irq_handler);
        kprintf("Done\n");
#endif /* OPT_TEST_IRQS */
}

