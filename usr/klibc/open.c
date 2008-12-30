/*
 * open.c
 *
 * On 32-bit platforms we need to pass O_LARGEFILE to the open()
 * system call, to indicate that we're 64-bit safe.
 */

/*
#include <unistd.h>
#include <fcntl.h>
#include <bitsize.h>
*/

#define _KLIBC_IN_OPEN_C
#include <sys/types.h>
//extern int __open(const char *, int, mode_t);

#include <khui/syscalls.h>
#include <stdarg.h>
#include <fcntl.h>

int open(const char *pathname, int flags, ...)
{
	//va_list ap;
	mode_t mode = 0;

	/*if (flags & O_CREAT) {
		va_start(ap, flags);
		mode = va_arg(ap, mode_t);
		va_end(ap);
	}*/

	return __sys_open(pathname, flags, mode);
}

