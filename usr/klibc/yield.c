/*
 * yield.c
 */

#include <khui/syscalls.h>

/*
 * There is no such POSIX system call called yield. There is
 * something called sched_yield, but as I understand it is not
 * a "basic" system call, and is defined in <sched.h>, while
 * our yield() is defined in <yield.h>.
 */


int yield(void)
{
	return __sys_yield();
}
