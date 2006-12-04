
/*
 * arch/i386/timer.c
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
#include <irq.h>
#include <machine.h>
#include <i386/asm.h>
#include <textio.h>
#include <i386/asm.h>

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

