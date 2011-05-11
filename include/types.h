/*
 * include/types.h
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
 */

#ifndef __TYPES_H__
#define __TYPES_H__

#include <khui/types.h>

typedef __s8 s8;
typedef __u8 u8;

typedef __s16 s16;
typedef __u16 u16;

typedef __s32 s32;
typedef __u32 u32;

typedef __s64 s64;
typedef __u64 u64;

typedef __s8 int8_t;
typedef __u8 uint8_t;

typedef __s16 int16_t;
typedef __u16 uint16_t;

typedef __s32 int32_t;
typedef __u32 uint32_t;

typedef __s64 int64_t;
typedef __u64 uint64_t;

typedef __gid_t gid_t;
typedef __uid_t uid_t;
typedef __mode_t mode_t;
typedef __off_t off_t;
typedef __size_t size_t;
typedef __pid_t pid_t;

typedef long intptr_t;
typedef unsigned long uintptr_t;

#define NULL ((void *) 0)

#endif
