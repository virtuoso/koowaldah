/*
 * arch/dummy/syscalls.c
 *
 * Copyright (C) 2006-2008 Alexander Shishkin
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
			return open((char *)arg0, (u32)arg1);

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

		default:
			kprintf("syscall %d not implemented\n", sys);
			display_thread();
			return -ENOSYS;
	}
}

