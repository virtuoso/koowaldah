/*
 * usr/init/init.c
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

