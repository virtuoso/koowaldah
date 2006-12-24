
/*
 * include/koowaldah.h
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

#ifndef __KOOWALDAH_H__
#define __KOOWALDAH_H__

#include <types.h>
#include <config.h>

extern u64 __volatile__ jiffies;
extern u8 kernel_started;

/* kernel component name length */
#define KCOMPONENT_NAME_LEN 16

/* Generic MIN()/MAX() implementation */
#define MIN(a,b) ({ \
	typeof(a) _x = (a); \
	typeof(b) _y = (b); \
	(void) (&_x == &_y); \
	_x < _y ? _x : _y; })
#define MAX(a,b) ({ \
	typeof(a) _x = (a); \
	typeof(b) _y = (b); \
	(void) (&_x == &_y); \
	_x > _y ? _x : _y; })

#ifdef __builtin_offsetof
#define offsetof(TYPE, MEMBER) __builtin_offsetof(TYPE, MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)NULL)->MEMBER)
#endif

#define __init   __attribute__((section(".init")))
#define __future __attribute__((unused))
#define __inline __attribute__((always_inline)) inline
#define __noprof __attribute__((no_instrument_function))
#define __regparm(n) __attribute__((regparm(n)))

typedef int (*initfn)(void);

#define late_init(f) \
	__attribute__((section(".init.late"))) initfn __ ##f## _late = f;

#endif /* __KOOWALDAH_H_ */

