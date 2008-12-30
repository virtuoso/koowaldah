/*
 * getpid.c
 */

#include <khui/syscalls.h>
#include <sys/types.h>


pid_t getpid(void)
{
	return (pid_t) __sys_getpid();
}

/* XXX */
pid_t getppid(void)
{
	return 1;
}

