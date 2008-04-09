/*
 * arch/dummy/loader/main.c
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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/mman.h>
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
EXPORT void dummy_bug(const char *file, const int line)
{
	void *bt[TRACE_FRAMES];
	int total, i;
	char **buf;

	printf("Kernel bug at %s:%d\n", file, line);

	total = backtrace(bt, TRACE_FRAMES);
	buf = backtrace_symbols(bt, total);
	if (buf) {
		puts("Backtrace:\n");
		for (i = 0; i < total; i++)
			puts(buf[i]);

		free(buf);
	}

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

PRIVATE int main(int argc, const char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Missing argument: kernel shared object\n");
		exit(EXIT_FAILURE);
	}

	printf("Starting koowaldah 'dummy' execution realm\n");

	if (init_memory()) {
		fprintf(stderr, "Can't allocate %d bytes of memory.\n",
				dummy_get_mem_size());
		exit(EXIT_FAILURE);
	}

	H = dlopen(argv[1], RTLD_LAZY | RTLD_GLOBAL);
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

