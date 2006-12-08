
/*
 * include/bug.h
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

#define bug() do {				\
	panic("Bug in file %s, line %d", __FILE__, __LINE__); \
} while (0)

#define bug_on(cond) do {			\
	if (cond) bug();			\
} while (0);

#endif /* __BUG_H__ */
