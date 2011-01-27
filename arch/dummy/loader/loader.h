/*
 * arch/dummy/loader/loader.h
 *
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
 */

#ifndef __ARCH_DUMMY_LOADER_H__
#define __ARCH_DUMMY_LOADER_H__

/* EXPORT'ed symbols are allowed to be referenced by the kernel,
 * PRIVATE ones are loader-only visible */
#define EXPORT __attribute__((used,visibility("protected")))
#define PRIVATE __attribute__((visibility("internal")))

extern void *H;

int init_memory();
int dummy_get_mem_size();

void init_timers(void);
void init_sysrq(void);

typedef void (*idle_call_t)(void *);

void push_idle_call(idle_call_t fn, int timeout, void *data);

#endif

