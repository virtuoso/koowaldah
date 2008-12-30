/*
 * mpipe_open.c
 */

/*
#include <unistd.h>
#include <fcntl.h>
#include <bitsize.h>
*/

#include <sys/types.h>

#include <khui/syscalls.h>

int mpipe_open(const char *pathname, int flags)
{
	return __sys_mpipe_open(pathname);
}

