
/*
 * tsleep.c
 */

#include <khui/syscalls.h>


int tsleep(unsigned int t)
{
	return __sys_tsleep(t);
}
