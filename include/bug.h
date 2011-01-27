
/*
 * include/bug.h
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

#ifndef __BUG_H__
#define __BUG_H__


#include <textio.h>
#include <arch/bug.h>
#include <arch/asm.h>


void display_thread();

/*
 * First of all, we dump all the registers on the stack. Now we can mess with them calling
 * fprintf and other stuff and get them unchanged in arch_display_regs().
 */

#define info(fmt, args ...) do {		\
	arch_dump_registers();			\
	kprintf("\nDebug info: " fmt "\b", ## args);	\
	display_thread();			\
	kprintf("Registers:\n");		\
	arch_display_registers();		\
	kprintf("Stack:\n");			\
	arch_display_stack();			\
}while (0)

#define panic(fmt, args ... ) do {		\
	local_irq_disable();			\
	arch_dump_registers();			\
	kprintf("\nPanic: " fmt "\n", ## args);	\
	display_thread();			\
	kprintf("Registers:\n");		\
	arch_display_registers();		\
	kprintf("Stack:\n");			\
	arch_display_stack();			\
	for(;;) {				\
		kprintf("R.I.P.\n");		\
		arch_halt();			\
	}					\
} while (0)


#ifndef __HAVE_ARCH_BUG
#warning "Have no arch BUG(), using the default one."
#define bug() do {				\
	panic("Bug in file %s, line %d", __FILE__, __LINE__); \
} while (0)
#endif /* __HAVE_ARCH_BUG */

#ifndef __HAVE_ARCH_BUGON
#define bug_on(cond) do {			\
	if (cond) bug();			\
} while (0);
#endif /* __HAVE_ARCH_BUGON */

#endif /* __BUG_H__ */
