/*
 * include/dummy/thread.h
 *
 * Copyright (C) 2006, 2007 Alexander Shishkin
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
 * THIS SOFTWARE IS PROVIDED BY THE DEVELOPERS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE DEVELOPERS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#ifndef __ARCH_THREAD_H__
#define __ARCH_THREAD_H__

struct thread_context {
	char placeholder[UCONTEXT_SIZE];
	u32 stack_base;
	u32 esp;
};

#define THREAD_STACK_LIMIT PAGE_SIZE

#define THREAD(s) ( { \
		        unsigned long __q = (unsigned long)s; \
		        __q &= ~(PAGE_SIZE - 1); \
		        __q = __q + PAGE_SIZE - sizeof(struct thread); \
		        (struct thread *) __q; } )

#define CURRENT() ( { \
		        unsigned long __r = (unsigned long)__builtin_frame_address(0); \
		        THREAD(__r); } )

#define reset_stack() \
	do { CURRENT()->context.esp = (u32)CURRENT() - 4; } while (0);

#endif

