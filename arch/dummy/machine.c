/*
 * arch/dummy/machine.c
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

#include <koowaldah.h>
#include <machine.h>
#include <dummy/loader.h>

u64 __volatile__ jiffies;

/* IO */
void console_put_char(char c)
{
	dummy_putc(c);
}

void mach_start()
{
}

void mach_running()
{
}

int register_irq_handler(u32 number, void (*handler)(u32 number))
{
	return 1;
}

void unregister_irq_handler(u32 number)
{
}

extern void update_timers();

/*
 * First of all, we should maintain internal timer(s)
 */
int timer_handler(void)
{
        jiffies++;
        /*XXX: if (mach_state == MACH_RUNNING)*/
                update_timers();

	return 0;
}

