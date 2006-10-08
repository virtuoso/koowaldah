
/*
 * include/i386/vga_console.h
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#ifndef __ARCH_VGA_CONSOLE_H__
#define __ARCH_VGA_CONSOLE_H__

void vga_console_init(); 

void vga_put_char(char c);

#endif /* __ARCH_VGA_CONSOLE_H__ */

