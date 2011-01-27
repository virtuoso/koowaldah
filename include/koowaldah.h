
/*
 * include/koowaldah.h
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

#ifndef __KOOWALDAH_H__
#define __KOOWALDAH_H__

#include <types.h>
#include <koptions.h>

extern u64 __volatile__ jiffies;
extern u8 kernel_started;

/* kernel component name length */
#define KCOMPONENT_NAME_LEN 16

/* Generic MIN()/MAX() implementation */
#define MIN(a,b) ({ \
	__typeof__(a) _x = (a); \
	__typeof__(b) _y = (b); \
	(void) (&_x == &_y); \
	_x < _y ? _x : _y; })
#define MAX(a,b) ({ \
	__typeof__(a) _x = (a); \
	__typeof__(b) _y = (b); \
	(void) (&_x == &_y); \
	_x > _y ? _x : _y; })

#define ARRSZ(arr) (sizeof(arr) / sizeof(__typeof__(*arr)))

#ifdef __builtin_offsetof
#define offsetof(TYPE, MEMBER) __builtin_offsetof(TYPE, MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)NULL)->MEMBER)
#endif

#define __init   __attribute__((section(".init")))
#define __data   __attribute__((section(".data")))
#define __alias(x) __attribute__((weak,alias(#x)))
#define ALIAS(x, p) extern __typeof__(p) x __alias(p)
#define __future __attribute__((unused))
#define __inline __attribute__((always_inline)) __inline__
#define __noprof __attribute__((no_instrument_function))
#define __regparm(n) __attribute__((regparm(n)))
#define __noreturn __attribute__((noreturn))

typedef int (*initfn)(void);

#define late_init(f) \
	__attribute__((section("InitLate"))) initfn __ ##f## _late = f;

#include <debug.h>

#endif /* __KOOWALDAH_H_ */

