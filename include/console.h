
/*
 * include/console.h
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 */

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

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
