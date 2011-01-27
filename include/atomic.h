/*
 * include/atomic.h
 *
 * Copyright (C) 2006 Alexander Shishkin
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
 * Atomic types and operations
 * 
 */

#ifndef __ATOMIC_H__
#define __ATOMIC_H__

/* For signed types, please submit a patch with a good reason */
typedef struct { volatile u32 v; } atomic_u32;
typedef struct { volatile u16 v; } atomic_u16;
typedef struct { volatile u8 v; } atomic_u8;

#include <arch/atomic.h>
#ifndef __ARCH_ATOMIC
#error No atomic operations on your arch? Too bad.
#endif

#define ATOMIC_INIT(p) ((p)->v = 0)
#define atomic_read(p) ((p)->v)

#endif
