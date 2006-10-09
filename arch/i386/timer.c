
/*
 * arch/i386/timer.c
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#include <kuca.h>
#include <irq.h>
#include <arch/asm.h>
#include <textio.h>
#include <arch/asm.h>

u64 jiffies;

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


void timer_set_divisor(u16 divisor){
#if 0
	struct timer_command command = {
		.bcd = 0, 	/* bcd		- 16-bit counter */
		.mode = 3, 	/* moce		- Square Wave Mode */
		.rw_mode = 3, 	/* rw_mode	- LSB and MSB */
		.counter = 0  	/* counter	- counter 0 */
	};
#endif
	io_port_out(TIMER_COMMAND_PORT, 0x36);	
	io_port_out(TIMER_COUNTER_0_PORT, divisor & 0xFF);
	io_port_out(TIMER_COUNTER_0_PORT, divisor >> 8);

	jiffies = 0;
}

extern void update_timers();
/*
 * First of all, we should maintain internal timer(s)
 */
static void timer_interrupt_handler()
{
	jiffies++;
	update_timers();
}

int __init timer_init(){
	timer_set_divisor(4096);
	register_irq_handler(0, timer_interrupt_handler);
	return 0;
}


