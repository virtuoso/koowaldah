/*
 * arch/dummy/loader/dl_init.c
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

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include "loader.h"

/*
 * Load dynamically linked program using host's libdl
 */
EXPORT int dummy_load_so(const char *path)
{
	void *__H;
	void (*so_start)();
	void **sys_call_gate;
	void *sys_call;

	/* open each shared object in a new namespace */
	__H = dlmopen(-1, path, RTLD_NOW|RTLD_DEEPBIND);
	if (!__H) {
		fprintf(stderr, "Init load failed: %s\n", dlerror());
		return -1;
	}

	/* obtain program's entry point */
	so_start = dlsym(__H, "__libc_init");
	if (!so_start) {
		fprintf(stderr, "Init exec failed: %s\n", dlerror());
		return -1;
	}

	/* in-program address of system call trampoline */
	sys_call_gate = dlsym(__H, "_sys_call");
	if (!sys_call_gate) {
		fprintf(stderr, "Init exec failed: %s\n", dlerror());
		return -1;
	}

	/* obtain kernel's system call gate */
	sys_call = dlsym(H, "sys_call");
	if (!sys_call) {
		fprintf(stderr, "Can't find kernel's sys_call_gate.\n");
		return -1;
	}

	/* patch program's system call entry function to kernel's
	 * sys_call gate: this requires _sys_call symbol exported
	 * from the program (assumingly by libc) */
	*sys_call_gate = sys_call;

	/* start the program */
	so_start();

	return 0;
}

