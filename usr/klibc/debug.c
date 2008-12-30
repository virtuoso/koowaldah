

#include <khui/syscalls.h>


int debug(char *str)
{
	return __sys_debug(str);
}

