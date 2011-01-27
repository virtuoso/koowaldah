/*
 * arch/dummy/loader/dl_init.c
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

