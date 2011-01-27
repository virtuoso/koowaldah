/*
 * include/dummy/asm.h
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

#ifndef __ARCH_ASM__
#define __ARCH_ASM__

static __inline u8 inb(u16 port)
{
	return 0;
}

static __inline u16 inw(u16 port)
{
	return 0;
}

static __inline void outb(u16 port, u8 data)
{
}

static __inline void outw(u16 port, u16 data)
{
}

static __inline void local_irq_disable()
{
	/* same as ATOMIC_PROLOGUE */
}

static __inline void local_irq_enable()
{
	/* same as ATOMIC_PROLOGUE */
}

#endif

