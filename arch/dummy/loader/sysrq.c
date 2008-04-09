/*
 * arch/dummy/loader/sysrq.c
 *
 * Copyright (C) 2008 Alexander Shishkin
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

