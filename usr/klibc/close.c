/*
 * close.c
 */

#include <sys/types.h>

#include <khui/syscalls.h>

ssize_t close(int fd)
{
	return __sys_close(fd);
}

