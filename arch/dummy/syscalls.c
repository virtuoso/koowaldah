/*
 * arch/dummy/syscalls.c
 *
 * Copyright (C) 2006-2008 Alexander Shishkin
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

#include <koowaldah.h>
#include <textio.h>
#include <bug.h>
#include <khui/errno.h>
#include <khui/syscalls.h>
#include <file.h>
#include <timers.h>
#include <scheduler.h>
#include <message.h>
#include <mem_area.h>
#include <mpipe.h>
#include <stat.h>

unsigned int sys_call(
	unsigned int sys,
	unsigned int arg0,
	unsigned int arg1,
	unsigned int arg2,
	unsigned int arg3,
	unsigned int arg4,
	unsigned int arg5
)
{
	switch (sys) {
		case __SYS_debug:
			kprintf("# %s\n", (char *)arg0);
			return 0;

		case __SYS_open:
			return open((char *)arg0, (u32)arg1, (u32)arg2);

		case __SYS_close:
			return close((int)arg0);

		case __SYS_read:
			return read((int)arg0, (char *)arg1, (size_t)arg2);

		case __SYS_write:
			return write((int)arg0, (char *)arg1, (size_t)arg2);

		case __SYS_readdir:
			return readdir((int)arg0, (struct udirentry *)arg1,
					(int)arg2);

		case __SYS_tsleep:
			return tsleep(arg0);

		case __SYS_fork:
			return fork();

		case __SYS_yield:
			scheduler_yield();
			return 0;

		case __SYS_getpid:
			return CURRENT()->pid;

		case __SYS_exec:
			return thread_exec(CURRENT(), (char *)arg0);

		case __SYS_msg_send:
			return msg_send((pid_t)arg0, (char *)arg1, (size_t)arg2,
					(u32)arg3);

		case __SYS_msg_retrieve:
			return msg_retrieve((pid_t)arg0, (char **)arg1,
					(size_t)arg2, (u32)arg3);

		case __SYS_mpipe_open:
			return mpipe_open((char *)arg0);

		case __SYS_exit:
			exit((int)arg0);

		case __SYS_sbrk:
			return (u32)sbrk((size_t)arg0);

		case __SYS_brk:
			return brk((unsigned long)arg0);

		case __SYS_stat:
			return stat((char *)arg0, (struct stat *)arg1);

		case __SYS_mpipe_open2:
			return mpipe_open2((int)arg0);

		default:
			kprintf("syscall %d not implemented\n", sys);
			display_thread();
			return -ENOSYS;
	}
}

