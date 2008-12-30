/*
 * libc_init.c
 *
 * This function takes the raw data block set up by the ELF loader
 * in the kernel and parses it.  It is invoked by crt0.S which makes
 * any necessary adjustments and passes calls this function using
 * the standard C calling convention.
 *
 * The arguments are:
 *  uintptr_t *elfdata	 -- The ELF loader data block; usually from the stack.
 *                          Basically a pointer to argc.
 *  void (*onexit)(void) -- Function to install into onexit
 */

/*
 * Several Linux ABIs don't pass the onexit pointer, and the ones that
 * do never use it.  Therefore, unless USE_ONEXIT is defined, we just
 * ignore the onexit pointer.
 */
/* #define USE_ONEXIT */

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <klibc/compiler.h>
#include <khui/types.h>
#include <khui/syscalls.h>
#if 0 /* no elf */
#include <elf.h>
#endif
#include "atexit.h"

/* This file is included from __static_init.c or __shared_init.c */
#ifndef SHARED
# error "SHARED should be defined to 0 or 1"
#endif

char **environ;
unsigned int __page_size, __page_shift;
extern char __bss_start, __bss_end;
void debug_exit()
{
	__sys_debug("atexit()");
}

__noreturn __libc_init(uintptr_t * elfdata, void (*onexit) (void))
{
	extern int main(int, char **, char **);
	static struct atexit at_exit;
	int fd;
	int argc = 1;
	char *argv[] = {"init", NULL};
	char buf[80];

	//sprintf(buf, "__bss_start=%08x, __bss_end=%08x\n", &__bss_start, &__bss_end);
	//__sys_debug(buf);
	memset(&__bss_start, 0, &__bss_end - &__bss_start);

	__page_size = PAGE_SIZE;
	__page_shift = PAGE_SHIFT;

	at_exit.fctn = debug_exit;
	at_exit.next = NULL;
	__atexit_list = &at_exit;



	close(0);
	close(1);
	close(2);
	/* STDIN */
	fd = open("/dev/serial", O_RDWR);
	/* STDOUT */
	fd = open("/dev/serial", O_RDWR);
	/* STDERR */
	fd = open("/dev/serial", O_RDWR);

	__sys_debug("libc_init()");

	exit(main(argc, argv, NULL));
}

#if 0 /* Only makes sense for linux/Elf */
struct auxentry {
	uintptr_t type;
	uintptr_t v;
};

__noreturn __libc_init(uintptr_t * elfdata, void (*onexit) (void))
{
	int argc;
	char **argv, **envp, **envend;
	struct auxentry *auxentry;
#if SHARED
	typedef int (*main_t) (int, char **, char **);
	main_t MAIN = NULL;
#else
	extern int main(int, char **, char **);
#define MAIN main
#endif
	unsigned int page_size = 0, page_shift = 0;

#ifdef USE_ONEXIT
	if (onexit) {
		static struct atexit at_exit;

		at_exit.fctn = (void (*)(int, void *))onexit;
		/* at_exit.next = NULL already */
		__atexit_list = &at_exit;
	}
#else
	(void)onexit;		/* Ignore this... */
#endif

	argc = (int)*elfdata++;
	argv = (char **)elfdata;
	envp = argv + (argc + 1);

	/* The auxillary entry vector is after all the environment vars */
	for (envend = envp; *envend; envend++) ;
	auxentry = (struct auxentry *)(envend + 1);

	while (auxentry->type) {
		switch (auxentry->type) {
#if SHARED
		case AT_ENTRY:
			MAIN = (main_t) (auxentry->v);
			break;
#endif
		case AT_PAGESZ:
			page_size = (unsigned int)(auxentry->v);
			break;
		}
		auxentry++;
	}

	__page_size = page_size;

#if __GNUC__ >= 4
	/* unsigned int is 32 bits on all our architectures */
	page_shift = __builtin_clz(page_size) ^ 31;
#elif defined(__i386__) || defined(__x86_64__)
      asm("bsrl %1,%0": "=r"(page_shift):"r"(page_size));
#else
	while (page_size > 1) {
		page_shift++;
		page_size >>= 1;
	}
#endif
	__page_shift = page_shift;

	environ = envp;
	exit(MAIN(argc, argv, envp));
}
#endif

