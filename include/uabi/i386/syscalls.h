
/*
 * include/uabi/i386/syscall.h
 *
 * Copyright (C) 2006 Alexander Shishkin
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


#ifndef __UABI_I386_SYSCALLS_H__
#define __UABI_I386_SYSCALLS_H__

/*
 * System call numbers and functions
 */

#define __SYS_debug 0
#define __SYS_open 1
#define __SYS_close 2
#define __SYS_read 3
#define __SYS_write 4
#define __SYS_tsleep 5
#define __SYS_fork 6
#define __SYS_yield 7
#define __SYS_getpid 8

int __sys_debug(const char *str);
int __sys_open(const char *name, unsigned int mode);
int __sys_close(int fd);
int __sys_read(int fd, const char *buf, unsigned int len);
int __sys_write(int fd, const char *buf, unsigned int len);
int __sys_tsleep(unsigned int t);
int __sys_fork();
int __sys_yield();
int __sys_getpid();

#endif /* __UABI_I386_SYSCALLS_H__ */

