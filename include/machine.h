/*
 * include/machine.h
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

#ifndef __MACHINE_H__
#define __MACHINE_H__

extern u32 mach_state;

enum {
	MACH_BOOTUP,
	MACH_RUNNING
};

/*
 * Architecture provided functions
 */

/* machine-dependent startup code */
void mach_start();
/* called when everything is set up */
void mach_running();

#endif

