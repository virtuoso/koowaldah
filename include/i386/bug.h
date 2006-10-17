
/*
 * include/i386/bug.h
 *
 * Copyright (C) 2006 Alexey Zaytsev
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
 * 3. Neither the name of the Koowaldah developers nor the names of theyr 
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

#ifndef __ARCH_BUG_H__
#define __ARCH_BUG_H__

#include <kuca.h>
#include <thread.h>
#include <arch/asm.h>

void i386_display_regs(u32 *regs_dump);
void i386_dump_stack(u32 * stack);

#define arch_halt() \
	do { 					\
		for (;;) { 			\
			disable_interrupts(); 	\
			asm volatile ("hlt"); 	\
		} 				\
	} while (0)


#define arch_dump_registers() 			\
	do { 					\
		asm volatile (			\
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

/*
#define arch_dump_regs() 			\
	do { 					\
		asm volatile (			\
			"pushl $0xDEAD0001\n"		\
			"pushl $0xDEAD0002\n"		\
			"pushl $0xDEAD0003\n"		\
			"pushl $0xDEAD0004\n"		\
			"pushl $0xDEAD0005\n"		\
			"pushl $0xDEAD0006\n"		\
			"pushl $0xDEAD0007\n"		\
			"pushl $0xDEAD0008\n"		\
		);				\
	} while (0)
*/
	
#define arch_display_registers()			\
	do {					\
		asm volatile (			\
			"pushl %esp\n"		\
			"call i386_display_regs\n"			\
			"addl $36, %esp" /* remove all the dumped	\
					    regs from the stack, 	\
					    including %esp */		\
		);				\
	} while (0)

#define arch_display_stack() do {		\
		asm volatile (			\
			"pushl %esp\n"		\
			"call i386_dump_stack\n"\
			"addl $4, %esp"		\
		);				\
	} while (0)

#define arch_display_thread() do { 		\
	struct thread_t *thread;		\
	thread = CURRENT();			\
	kprintf("Task: %s, pid: %d\n",		\
			thread->name,		\
			thread->pid		\
		);				\
} while (0)

#endif /* __ARCH_BUG_H__ */


