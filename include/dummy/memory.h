/*
 * include/dummy/memory.h
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
 *
 */

#ifndef __ARCH_MEMORY_H__
#define __ARCH_MEMORY_H__

/* allow kernel to consume 1/KERN_ALLOWANCE of available physical memory */
#define KERN_ALLOWANCE 8

/* virtual addresses and limits for basic mem_areas */
#define USERMEM_START    0x00200000
#define USERMEM_STACK    0x00300000
#define USERMEM_STACKLIM 0x0000f000
#define USERMEM_HEAP     0x00300000
#define USERMEM_HEAPLIM  0x00080000
#define USERMEM_MBOX     0x00400000
#define in_kernel(p) ((u32)p < USERMEM_START ? 1 : 0)

#endif

