
/*
 * include/khui/i386/syscalls.h
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
 */


#ifndef __KHUI_I386_SYSCALLS_H__
#define __KHUI_I386_SYSCALLS_H__

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
#define __SYS_exec 9
#define __SYS_msg_send 10
#define __SYS_msg_retrieve 11
#define __SYS_readdir 12
#define __SYS_mpipe_open 13
#define __SYS_exit 14
#define __SYS_sbrk 15
#define __SYS_brk 16
#define __SYS_stat 17
#define __SYS_mpipe_open2 18

int __sys_debug(const char *str);
int __sys_open(const char *name, unsigned int flags, unsigned int mode);
int __sys_close(int fd);
int __sys_read(int fd, const char *buf, unsigned int len);
int __sys_write(int fd, const char *buf, unsigned int len);
int __sys_tsleep(unsigned int t);
int __sys_fork();
int __sys_yield();
int __sys_getpid();
int __sys_exec();
int __sys_msg_send(int recp, char *buf, int len, unsigned int flags);
int __sys_msg_retrieve(int recp, char **buf, int len, unsigned int flags);
int __sys_mpipe_open(const char *name);
void __attribute__((noreturn)) __sys_exit(int code);
void *__sys_sbrk(unsigned int inc);
unsigned long __sys_brk(unsigned long end);
int __sys_stat(const char *, void *);
int __sys_mpipe_open2(int fd);

#endif /* __KHUI_I386_SYSCALLS_H__ */

