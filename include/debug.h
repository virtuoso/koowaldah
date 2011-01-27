
/*
 * include/debug.h
 *
 * Copyright (C) 2006 Alexey Zaytsev
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
 */


#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef DEBUG
#define DPRINT(fmt, args ...) do {	\
	kprintf(fmt, ## args); 		\
} while (0)
#else
#define DPRINT(fmt, args ...) 
#endif /* DEBUG */

#ifdef OPT_INSTRUMENT_PROFILER
void flush_events();
#else
#include <bug.h>
#define flush_events() bug()
#endif

#ifdef OPT_SYSRQ_DEBUG
int sysrq_hit(char n);
#else
#define sysrq_hit(n) 0
#endif

#endif /* __DEBUG_H__ */
