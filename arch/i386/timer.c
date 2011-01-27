
/*
 * arch/i386/timer.c
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
#include <irq.h>
#include <machine.h>
#include <i386/asm.h>
#include <textio.h>
#include <i386/asm.h>

u64 __volatile__ jiffies;

#define TIMER_COMMAND_PORT 0x43
#define TIMER_COUNTER_0_PORT 0x40
#define TIMER_COUNTER_1_PORT 0x41
#define TIMER_COUNTER_2_PORT 0x42

#if 0
struct timer_command{
	int bcd:1;
	int mode:3;
	int rw_mode:2;
	int counter:2;
};
#endif


void timer_set_divisor(u16 divisor)
{
#if 0
	struct timer_command command = {
		.bcd = 0, 	/* bcd		- 16-bit counter */
		.mode = 3, 	/* moce		- Square Wave Mode */
		.rw_mode = 3, 	/* rw_mode	- LSB and MSB */
		.counter = 0  	/* counter	- counter 0 */
	};
#endif
	outb(TIMER_COMMAND_PORT, 0x36);	
	outb(TIMER_COUNTER_0_PORT, divisor & 0xFF);
	outb(TIMER_COUNTER_0_PORT, divisor >> 8);

	jiffies = 0;
}

extern void update_timers();
/*
 * First of all, we should maintain internal timer(s)
 */
static void timer_interrupt_handler()
{
	jiffies++;
	if (mach_state == MACH_RUNNING)
		update_timers();
}

int __init timer_init()
{
	timer_set_divisor(4096);
	register_irq_handler(0, timer_interrupt_handler);
	return 0;
}

