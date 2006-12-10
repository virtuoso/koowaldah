/*
 * sys/types.h
 *
 * Copyright (C) 2006 Alexander Shishkin
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Koowaldah developers nor the names of their
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
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
 */

#ifndef __SYS_TYPES_H__
#define __SYS_TYPES_H__

#ifndef __ARCH_TYPES_H__
#  include <machine/int_types.h>
#endif

/* BSD sources compatibility */
#if defined(_BSD_SOURCE)
typedef unsigned char  u_char;
typedef unsigned short u_short;
typedef unsigned int   u_int;
typedef unsigned long  u_long;

/* SysV compatibility */
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;
#endif

/* ansi definitions */
typedef __uint32_t __gid_t;
typedef __uint32_t __uid_t;
typedef __uint32_t __mode_t;
typedef __int32_t __off_t; /* XXX: should be s64 */
typedef __uint32_t __size_t;
typedef __int32_t __pid_t;

#ifndef gid_t
typedef __gid_t  gid_t;
#define gid_t    __gid_t
#endif

#ifndef uid_t
typedef __uid_t  uid_t;
#define uid_t    __uid_t
#endif

#ifndef mode_t
typedef __mode_t mode_t;
#define mode_t   __mode_t
#endif

#ifndef off_t
typedef __off_t  off_t;
#define off_t    __off_t
#endif

#ifndef size_t
typedef __size_t size_t;
#define size_t   __size_t
#endif

#ifndef pid_t
typedef __pid_t  pid_t;
#define pid_t    __pid_t
#endif

#endif
