/*
 * include/dummy/pages.h
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

#ifndef __ARCH_PAGES_H__
#define __ARCH_PAGES_H__

/* page size, bits, mask, unmask */
#ifndef PAGE_SHIFT
#define PAGE_SHIFT 12
#endif
#define PAGE_SIZE (1UL << PAGE_SHIFT)
#define PAGE_MASK (PAGE_SIZE - 1)
#define NOPAGE_MASK ~(PAGE_MASK)
#define NOPAGE_ADDR (~0UL)

#define PTF_RW		(0x3)  /* PROT_READ | PROT_WRITE */
#define PTF_EXEC	(0x4)  /* PROT_EXEC */

static __inline u32 __virt2physpg(u32 pgaddr)
{
	return (pgaddr & NOPAGE_MASK);
}

static __inline u32 __virt2phys(u32 addr)
{
	return addr;
}

#endif

