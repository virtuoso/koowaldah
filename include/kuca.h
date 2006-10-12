
/*
 * include/kuca.h
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#ifndef __KUCA_H__
#define __KUCA_H__

#include <arch/types.h>
#include <config.h>

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

#define __init __attribute__((section(".init")))

#endif /* __KUCA_H_ */

