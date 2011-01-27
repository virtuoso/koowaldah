/*
 * arch/dummy/loader/main.c
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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <execinfo.h>
#include "loader.h"

/*
 * This is 'dummy' architecture realm for koowaldah, that is,
 * user-mode kernel execution helper/loader/emulator.
 */

void *H;                   /* kernel so handle */
void (*kern_start)();      /* kernel entry point */

/* total sleep time during idle calls in microseconds */
#define IDLE_TIMEOUT 5000

/* idle function descriptor */
struct idle_call_node {
	idle_call_t           func;
	int                   timeout;
	void                  *data;
	struct idle_call_node *next;
};

/* chain of functions to be called when the system is idle */
static struct idle_call_node idle_head = { NULL, IDLE_TIMEOUT, NULL, NULL };

/*
 * Register a function within the chain of idle calls.
 * The function is allowed to sleep at most timeout microseconds;
 * all registered functions should total to no more than IDLE_TIMEOUT.
 */
PRIVATE void push_idle_call(idle_call_t fn, int timeout, void *data)
{
	struct idle_call_node *icn;

	if (!fn || timeout > idle_head.timeout)
		return;

	icn = malloc(sizeof(struct idle_call_node));
	if (!icn)
		perror("malloc");

	icn->func = fn;
	icn->timeout = timeout;
	icn->data = data;
	icn->next = idle_head.next;
	idle_head.next = icn;
	idle_head.timeout -= timeout;
}

EXPORT void __attribute__((noreturn))  dummy_abort()
{
	/* clean up */
	exit(EXIT_SUCCESS);
}

EXPORT void dummy_noreturn()
{
	printf("NORETURN!\n");
}

#define TRACE_FRAMES 32
EXPORT void dummy_dump_stack(void)
{
	void *bt[TRACE_FRAMES];
	int total, i;
	char **buf;

	total = backtrace(bt, TRACE_FRAMES);
	buf = backtrace_symbols(bt, total);
	if (buf) {
		puts("Backtrace:\n");
		for (i = 0; i < total; i++)
			puts(buf[i]);

		free(buf);
	}
}

EXPORT void dummy_bug(const char *file, const int line)
{
	printf("Kernel bug at %s:%d\n", file, line);

	dummy_dump_stack();
	dummy_abort();
}

EXPORT void dummy_putc(char c)
{
	putchar(c);
	fflush(stdout);
}

EXPORT void dummy_delay()
{
	struct idle_call_node *icn;
	
	for (icn = idle_head.next; icn; icn = icn->next) {
		(*icn->func)(icn->data);
	}

	if (idle_head.timeout)
		usleep(idle_head.timeout);
}

char *init_path = NULL;

EXPORT void dummy_load_from_cpio(const char *path)
{
	if (!init_path)
		return;

	dummy_load_so(init_path);
}

PRIVATE int main(int argc, const char **argv)
{
	if (argc != 2 && argc != 3) {
		fprintf(stderr, "Missing argument: kernel shared object\n");
		exit(EXIT_FAILURE);
	}

	if (argc == 3) {
		init_path = argv[2];
		printf("Using init %s\n", init_path);
	}

	printf("Starting koowaldah 'dummy' execution realm\n");

	if (init_memory()) {
		fprintf(stderr, "Can't allocate %d bytes of memory.\n",
				dummy_get_mem_size());
		exit(EXIT_FAILURE);
	}

	H = dlopen(argv[1], RTLD_LAZY | RTLD_GLOBAL | RTLD_DEEPBIND);
	if (!H) {
		perror(dlerror());
		exit(EXIT_FAILURE);
	}

	kern_start = dlsym(H, "kern_start");
	if (!kern_start) {
		perror(dlerror());
		exit(EXIT_FAILURE);
	}

	init_timers();
	init_sysrq();
	kern_start();

	return 0;
}

