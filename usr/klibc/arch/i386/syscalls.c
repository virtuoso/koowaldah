/*
 * usr/libc/sys/i386/syscall.c
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
 *
 */

#include <klibc/compiler.h>
#include <khui/syscalls.h>
#include <khui/types.h>
#include <khui/fs.h>

/*
 * System calls. This will most likely be automatically generated
 * in future.
 */

typedef unsigned int u32;
extern u32 sys_call(u32,u32,u32,u32,u32,u32,u32);
typedef u32 (*sys_call_gate_fn_t)(u32,u32,u32,u32,u32,u32,u32);
sys_call_gate_fn_t _sys_call = sys_call;

#define SYSCALL0(__fn, __t0) \
__t0 __sys_ ## __fn() { \
	return (__t0)_sys_call(__SYS_ ## __fn, 0, 0, 0, 0, 0, 0); \
}

#define SYSCALL1(__fn, __t0, __t1) \
__t0 __sys_ ## __fn(__t1 arg1) { \
	return (__t0)_sys_call(__SYS_ ## __fn, (u32)arg1, 0, 0, 0, 0, 0); \
}

#define SYSCALL1_NORET(__fn, __t0, __t1) \
__t0 __sys_ ## __fn(__t1 arg1) { \
	_sys_call(__SYS_ ## __fn, (u32)arg1, 0, 0, 0, 0, 0); \
	__sys_ ## __fn(arg1); \
}

#define SYSCALL2(__fn, __t0, __t1, __t2) \
__t0 __sys_ ## __fn(__t1 arg1, __t2 arg2) { \
	return (__t0)_sys_call(__SYS_ ## __fn, (u32)arg1, (u32)arg2, 0, 0, 0, 0); \
}

#define SYSCALL3(__fn, __t0, __t1, __t2, __t3) \
__t0 __sys_ ## __fn(__t1 arg1, __t2 arg2, __t3 arg3) { \
	return (__t0)_sys_call(__SYS_ ## __fn, (u32)arg1, (u32)arg2, (u32)arg3, \
			0, 0, 0); \
}

#define SYSCALL4(__fn, __t0, __t1, __t2, __t3, __t4) \
__t0 __sys_ ## __fn(__t1 arg1, __t2 arg2, __t3 arg3, __t4 arg4) { \
	return (__t0)_sys_call(__SYS_ ## __fn, (u32)arg1, (u32)arg2, (u32)arg3, \
			(u32)arg4, 0, 0); \
}

#define SYSCALL5(__fn, __t0, __t1, __t2, __t3, __t4, __t5) \
__t0 __sys_ ## __fn(__t1 arg1, __t2 arg2, __t3 arg3, __t4 arg4, __t5 arg5) { \
	return (__t0)_sys_call(__SYS_ ## __fn, (u32)arg1, (u32)arg2, (u32)arg3, \
			(u32)arg4, (u32)arg5, 0); \
}

#define SYSCALL6(__fn, __t0, __t1, __t2, __t3, __t4, __t5, __t6) \
__t0 __sys_ ## __fn(__t1 arg1, __t2 arg2, __t3 arg3, __t4 arg4, __t5 arg5, __t6 arg6) { \
	return (__t0)_sys_call(__SYS_ ## __fn, (u32)arg1, (u32)arg2, (u32)arg3, \
			(u32)arg4, (u32)arg5, (u32)arg6); \
}

SYSCALL1(debug, int, const char *)
SYSCALL3(open, int, const char *, unsigned int, unsigned int)
SYSCALL1(close, int, int)
SYSCALL3(read, int, int, const char *, unsigned int)
SYSCALL3(write, int, int, const char *, unsigned int)
SYSCALL1(tsleep, int, unsigned int)
SYSCALL0(fork, __pid_t)
SYSCALL0(yield, int)
SYSCALL0(getpid, __pid_t)
SYSCALL1(exec, int, char *)
SYSCALL4(msg_send, int, int, char *, int, u32)
SYSCALL4(msg_retrieve, int, int, char **, int, u32)
SYSCALL1(mpipe_open, int, const char *)
SYSCALL1_NORET(exit, __noreturn, int)
SYSCALL1(sbrk, void *, unsigned int)
SYSCALL1(brk, unsigned long, unsigned long)
SYSCALL2(stat, int, const char *, void *)
//SYSCALL6(mmap, void *, void *, int, int, int, int, int)
SYSCALL1(mpipe_open2, int, int)

