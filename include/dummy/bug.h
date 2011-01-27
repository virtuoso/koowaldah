/*
 * include/dummy/bug.h
 *
 * Copyright (C) 2006 Alexey Zaytsev
 * Copyright (C) 2007 Alexander Shishkin
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
#ifndef __DUMMY_BUG_H__
#define __DUMMY_BUG_H__

#include <koowaldah.h>
#include <arch/asm.h>

void dummy_abort();
void dummy_dump_stack(void);
void dummy_bug(const char *file, const int line);
void dummy_delay();

#define arch_idle() dummy_delay()

#define arch_halt() \
	dummy_abort();

#define arch_dump_registers() do {} while (0)

#define arch_display_registers() do {} while (0)

#define arch_display_stack() do {} while (0)

#define arch_dump_stack(s) \
	dummy_dump_stack();

#define __HAVE_ARCH_BUG

#define bug()      			\
	dummy_bug(__FILE__, __LINE__);

#endif /* __DUMMY_BUG_H__ */

