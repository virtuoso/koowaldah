/*
 * read.c
 */

#include <sys/types.h>

#include <khui/syscalls.h>

ssize_t read(int fd, void *buf, size_t count)
{
	return __sys_read(fd, buf, count);
}

