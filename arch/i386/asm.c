
/*
 * arch/i386/asm.c
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
#include <machine.h>
#include <console.h>
#include <i386/asm.h>
#include <i386/irq.h>
#include <scheduler.h>

u32 mach_state = MACH_BOOTUP;

void interrupts_init(void);
void trap_init(void);
void early_console_init(void);
void init_root_tss(void);
int timer_init(void);

/*
 * Finalize boot-time hw initialization
 */
void __init mach_start()
{
	/* + means of output */
	early_console_init();

	/* + x86 segmentation */
	init_root_tss();

	/* + irq handlers */
        interrupts_init();

	/* + cpu traps */
	trap_init();

	/* + hw timer */
        timer_init();
}

void mach_running()
{
	mach_state = MACH_RUNNING;
	local_irq_enable();
	scheduler_start();
}

