/*
 * include/dummy/bug.h
 *
 * Copyright (C) 2006 Alexey Zaytsev
 * Copyright (C) 2007 Alexander Shishkin
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
#ifndef __DUMMY_BUG_H__
#define __DUMMY_BUG_H__

#include <koowaldah.h>
#include <arch/asm.h>

void dummy_abort();
void dummy_dump_stack(void);
void dummy_bug(const char *file, const int line);
void dummy_delay();

#define arch_idle() dummy_delay()

#define arch_halt() \
	dummy_abort();

#define arch_dump_registers() do {} while (0)

#define arch_display_registers() do {} while (0)

#define arch_display_stack() do {} while (0)

#define arch_dump_stack(s) \
	dummy_dump_stack();

#define __HAVE_ARCH_BUG

#define bug()      			\
	dummy_bug(__FILE__, __LINE__);

#endif /* __DUMMY_BUG_H__ */

