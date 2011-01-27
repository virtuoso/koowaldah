/*
 * arch/i386/syscalls.c
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

/*
 * System calls
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

u32 __regparm(0) sys_call_gate(u32 eax, u32 ebx, u32 ecx, u32 edx,
		u32 esi, u32 edi)
{
	switch (eax) {
		case __SYS_debug:
			kprintf("# %s\n", (char *)ebx);
			return 0;

		case __SYS_open:
			return open((char *)ebx, (u32)ecx, (u32)edx);

		case __SYS_close:
			return close((int)ebx);

		case __SYS_read:
			return read((int)ebx, (char *)ecx, (size_t)edx);

		case __SYS_write:
			return write((int)ebx, (char *)ecx, (size_t)edx);

		case __SYS_readdir:
			return readdir((int)ebx, (struct udirentry *)ecx,
					(int)edx);

		case __SYS_tsleep:
			return tsleep(ebx);

		case __SYS_fork:
			return fork();

		case __SYS_yield:
			scheduler_yield();
			return 0;

		case __SYS_getpid:
			return CURRENT()->pid;

		case __SYS_exec:
			return thread_exec(CURRENT(), (char *)ebx);

		case __SYS_msg_send:
			return msg_send((pid_t)ebx, (char *)ecx, (size_t)edx,
					(u32)esi);

		case __SYS_msg_retrieve:
			return msg_retrieve((pid_t)ebx, (char **)ecx,
					(size_t)edx, (u32)esi);

		case __SYS_mpipe_open:
			return mpipe_open((char *)ebx);

		case __SYS_exit:
			exit((int)ebx);

		case __SYS_sbrk:
			return (u32)sbrk((size_t)ebx);

		case __SYS_brk:
			return brk((unsigned long)ebx);

		case __SYS_stat:
			return stat((char *)ebx, (struct stat *)ecx);

		case __SYS_mpipe_open2:
			return mpipe_open2((int)ebx);

		default:
			kprintf("syscall %d not implemented\n", eax);
			display_thread();
			return -ENOSYS;
	}
}

