/*
 * include/profiler.h
 *
 * Copyright (C) 2006 Alexander Shishkin
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

#ifndef __PROFILER_H__
#define __PROFILER_H__

#include <types.h>

/*
 * Profiling event types
 */
enum {
	PROF_ENTRY,
	PROF_EXIT
};

/*
 * Trigger flags
 */
#define PROF_DUMP  0x1   /* immediately flush event log */
#define PROF_START 0x2   /* start profiling with this event */
#define PROF_STOP  0x4   /* stop profiling */

/*
 * Profiling trigger
 */
struct prof_trigger {
	void *function;
	u32 flags;
	u8 type;
};

#endif

