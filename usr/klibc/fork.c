/*
 * fork.c
 */

#include <khui/syscalls.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>


pid_t fork(void)
{
	pid_t pid;
	int fd;

	//printf("=== before fork() ===\n");
	pid = __sys_fork();

	/* we don't inherit fds yet */
	if (!pid) {
		fd = open("/dev/serial", O_RDWR);
		fd = open("/dev/serial", O_RDWR);
		fd = open("/dev/serial", O_RDWR);
	}
	//printf("=== after fork(), pid=%d ===\n", pid);

	return pid;
}

