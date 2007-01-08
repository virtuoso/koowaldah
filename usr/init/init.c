/*
 * usr/init/init.c
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

#include <syscalls.h>
#include <string.h>
#include <stdio.h>

/*
 * The init program.
 */

#define WELCOME "Init process starting"
#define WRONGPID "This init is not the first process, bye-bye."

char buf[48];
void _start()
{
	int fd, l;
	unsigned short c;
	int pid = sys_getpid();
		char *to = &buf;

	memset(buf, 0, 48);
	if (pid != 1) {
		char *s = WRONGPID;
		sys_debug(s);
		l = 0;
		for (;;) {
			snprintf(buf, 32, "hello, init! (%d)", l++);
			sys_msg_send(1, buf, strlen(buf)+1, 1 | 4);
			sys_tsleep(200);
		}
	}

	snprintf(buf, strlen(WELCOME), WELCOME);
	sys_debug(buf);
	fd = sys_open("/init", 0);
	if (fd < 0)
		sys_debug("can't open /init");
	else {
		l = sys_read(fd, buf, 12);
		sys_debug(buf);
		sys_close(fd);
	}

	l = sys_fork();
	if (l == 0) {
		snprintf(buf, 7, "child!");
		sys_debug(buf);
		sys_exec("/sbin/init");
	} else
		snprintf(buf, 32, "parent! child pid=%d", l);

	snprintf(buf, 16, "waking up [pid=%d]", pid);

	for (;;)
		while (sys_msg_retrieve(2, &to, 48, 4))
			sys_debug(buf);
}

