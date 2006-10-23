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
 * 3. Neither the name of the Koowaldah developers nor the names of theyr
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

#include <syscalls.h>

/*
 * System calls. This will most likely be automatically generated
 * in future.
 */

typedef unsigned int u32;
extern u32 sys_call(u32,u32,u32,u32);

int sys_debug(char *str)
{
	return sys_call(SYS_debug, (u32)str, 0, 0);
}

int sys_open(char *name, unsigned int mode)
{
	return sys_call(SYS_open, (u32)name, mode, 0);
}

int sys_close(int fd)
{
	return sys_call(SYS_close, (u32)fd, 0, 0);
}

int sys_read(int fd, char *buf, unsigned int len)
{
	return sys_call(SYS_read, (u32)fd, (u32)buf, len);
}

int sys_tsleep(unsigned int t)
{
	return sys_call(SYS_tsleep, t, 0, 0);
}

