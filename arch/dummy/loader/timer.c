/*
 * arch/dummy/loader/timer.c
 *
 * Copyright (C) 2007 Alexander Shishkin
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
 * Timer function emulation: use a SIGALRM handler to simulate ticks.
 */

#include <stdlib.h>
#include <signal.h>
#include <dlfcn.h>
#include <sys/time.h>
#include "loader.h"

typedef int (*inthandler_t)(void);

static inthandler_t timer_handler = NULL;

static void sigalrm_handler(int signum);

static void install_handler()
{
	struct itimerval t;

	signal(SIGALRM, sigalrm_handler);

	t.it_interval.tv_sec =
	t.it_value.tv_sec = 0;

	t.it_interval.tv_usec =
	t.it_value.tv_usec = 5000;

	setitimer(ITIMER_REAL, &t, NULL);
}

static void sigalrm_handler(int signum)
{
	if (!timer_handler)
		return;

	(*timer_handler)();

	signal(SIGALRM, sigalrm_handler);
}

/*
 * Find timer "interrupt handler" function in kernel .so
 * and setup timer function
 */
PRIVATE void init_timers(void)
{
	timer_handler = dlsym(H, "timer_handler");
	if (!timer_handler) {
		perror(dlerror());
		exit(EXIT_FAILURE);
	}

	install_handler();
}
