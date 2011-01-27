
/*
 * include/i386/i386.h 
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
 * Compile-time sanity check
 * Turns out to be pretty useful.
 *
 */

#ifndef OPT_CPU_ARCH_I386
#error Never, ever include this file on non-x86 architectures.
#endif
#ifndef __i386__
#error Non-x86 or x86? Make up your mind
#endif
