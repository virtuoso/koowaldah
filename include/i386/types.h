
/*
 * include/i386/types.h
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#ifndef __ARCH_TYPES_H__
#define __ARCH_TYPES_H__

typedef unsigned long size_t;

typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef signed long long s64;
typedef unsigned long long u64;

#define NULL (void *) 0 

#endif /* __ARCH_TYPES_H__ */
