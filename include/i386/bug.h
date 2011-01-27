
/*
 * include/i386/bug.h
 *
 * Copyright (C) 2006 Alexey Zaytsev
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

#ifndef __I386_BUG_H__
#define __I386_BUG_H__

#include <koowaldah.h>
#include <arch/asm.h>

void i386_display_regs(struct register_frame frame);
void i386_dump_stack(u32 * stack);
void local_irq_disable();

#define arch_idle() \
	__asm__ __volatile__ ("hlt");

#define arch_halt() \
	do { 					\
		for (;;) { 			\
			local_irq_disable(); 	\
			arch_idle();            \
		} 				\
	} while (0)


#define arch_dump_registers() 			\
	do { 					\
		__asm__ __volatile__ (			\
			"pushl %esp\n"		\
			"pushl %ebp\n"		\
			"pushl %edi\n"		\
			"pushl %esi\n"		\
			"pushl %edx\n"		\
			"pushl %ecx\n"		\
			"pushl %ebx\n"		\
			"pushl %eax\n"		\
		);				\
	} while (0)

#define arch_display_registers()		\
	do {					\
		__asm__ __volatile__ (			\
			"call i386_display_regs\n"			\
			"addl $32, %esp" /* remove all the dumped	\
					    regs from the stack */ 	\
		);				\
	} while (0)

#define arch_display_stack() do {		\
		__asm__ __volatile__ (			\
			"pushl %esp\n"		\
			"call i386_dump_stack\n"\
			"addl $4, %esp"		\
		);				\
	} while (0)

#define arch_dump_stack(s) i386_dump_stack(s)

#define __HAVE_ARCH_BUG

#define bug() do { 			\
	__asm__ __volatile__ (		\
			"ud2\n"		\
			".word %c0\n"	\
			".long %c1\n"	\
			:: "i" (__LINE__), "i" (__FILE__)); \
} while (0)

#endif /* __I386_BUG_H__ */


