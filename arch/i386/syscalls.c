/*
 * arch/i386/syscalls.c
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
			return open((char *)ebx, (u32)ecx);

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

		default:
			kprintf("syscall %d not implemented\n", eax);
			display_thread();
			return -ENOSYS;
	}
}

