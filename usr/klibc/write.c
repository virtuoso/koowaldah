/*
 * read.c
 */

#include <sys/types.h>

#include <khui/syscalls.h>

ssize_t write(int fd, void *buf, size_t count)
{
	return __sys_write(fd, buf, count);
}

