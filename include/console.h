
/*
 * include/console.h
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <config.h>

/* console-like device driver structure */
struct console {
	int  (*init)();
	void (*destroy)();
	void (*put_char)(char c);
};

/*
 * Call this function before any console 
 * manipulations are performad
 */

int console_init();

void console_put_char(char c);

#endif /* __CONSOLE_H__ */
