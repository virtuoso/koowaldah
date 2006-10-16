
/*
 * include/i386/thread.h
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
 * Architecture-specific context-switching data
 *
 */

#ifndef __ARCH_THREAD_H__
#define __ARCH_THREAD_H__

#include <i386/i386.h>
#include <i386/asm.h>

struct x86_thread_context {
	u32 *stack_base;
	u32 esp;
};

#define tctx(t) ((t)->ctx.x86)

#define THREAD_STACK_LIMIT PAGE_SIZE
#define THREAD(s) ( { \
	unsigned long __q = (unsigned long)s; \
	__q &= ~(PAGE_SIZE - 1); \
	__q = __q + PAGE_SIZE - sizeof(struct thread_t); \
	(struct thread_t *) __q; } )
#define CURRENT() ( { \
	unsigned long __r; \
	__asm__ __volatile__("mov %%esp, %0" : "=m"(__r)); \
	THREAD(__r); } )

#endif
