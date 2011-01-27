/*
 * arch/dummy/loader/timer.c
 *
 * Copyright (C) 2007 Alexander Shishkin
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
 * Timer function emulation: use a SIGALRM handler to simulate ticks.
 */

#include <stdlib.h>
#include <stdio.h>
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

