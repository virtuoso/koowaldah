/*
 * sys/resource.h
 */

#ifndef _SYS_RESOURCE_H
#define _SYS_RESOURCE_H

#include <klibc/extern.h>
#include <sys/types.h>		/* MUST be included before linux/resource.h */
#if 0
#include <linux/resource.h>
#endif

__extern int getpriority(int, int);
__extern int setpriority(int, int, int);

#endif				/* _SYS_RESOURCE_H */
