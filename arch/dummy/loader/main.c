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
#include <dlfcn.h>
#include <sys/mman.h>
#include "loader.h"

/*
 * This is 'dummy' architecture realm for koowaldah, that is,
 * user-mode kernel execution helper/loader/emulator.
 */

void *H;                   /* kernel so handle */
void (*kern_start)();      /* kernel entry point */

EXPORT void __attribute__((noreturn))  dummy_abort()
{
	/* clean up */
	exit(EXIT_SUCCESS);
}

EXPORT void dummy_noreturn()
{
	printf("NORETURN!\n");
}

EXPORT void dummy_bug(const char *file, const int line)
{
	printf("Kernel bug at %s:%d\n", file, line);
	dummy_abort();
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

	kern_start();

	return 0;
}

