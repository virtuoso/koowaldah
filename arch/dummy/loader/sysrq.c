/*
 * arch/dummy/loader/sysrq.c
 *
 * Copyright (C) 2008 Alexander Shishkin
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

/*
 * Invoking sysrq
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <dlfcn.h>
#include <termios.h>
#include <sys/poll.h>
#include "loader.h"

typedef int (*sysrq_handler_t)(int);

static sysrq_handler_t sysrq_handler = NULL;

static struct termios term_std, term_koo;

static void sysrq_idle(void *data)
{
	struct pollfd fds = { .fd = 0, .events = POLLIN };
	int ch;

	if (sysrq_handler) {
		poll(&fds, 1, 5000);

		if (fds.revents & POLLIN) {
			ch = getchar();

			/* Control-A is our hit */
			if (ch == 1) {
				poll(&fds, 1, 0);

				ch = getchar();
				(*sysrq_handler)(ch);
			}
		}
	} else
		usleep(5000);
}

static void done_sysrq(void)
{
        tcsetattr(0, TCSANOW, &term_std);
}

PRIVATE void init_sysrq(void)
{
	tcgetattr(0, &term_std);
	atexit(done_sysrq);

        term_koo = term_std;
        term_koo.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(0, TCSANOW, &term_koo);

	/* don't check if dlsym() succeeded, since we can live
	 * with sysrq_handler==NULL quite fine */
	sysrq_handler = dlsym(H, "dummy_sysrq_handler");

	push_idle_call(sysrq_idle, 5000, NULL);
}

