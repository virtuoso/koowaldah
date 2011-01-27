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

#define PTF_PRESENT  (1 << 0)  /* page/pte is present    */
#define PTF_RW       (1 << 1)  /* write access available */
#define PTF_USER     (1 << 2)  /* accessible by user     */
#define PTF_PWT      (1 << 3)  /* prevent write-through  */
#define PTF_PCD      (1 << 4)  /* cache disable          */
#define PTF_ACCESSED (1 << 5)  /* page has been accessed */
#define PTF_DIRTY    (1 << 6)  /* page is dirty          */
#define PTF_PGSIZE   (1 << 7)  /* page size = 4M         */
#define PTF_GLOBAL   (1 << 8)  /* global (for TLB)       */

static __inline u32 __virt2physpg(u32 pgaddr)
{
	return (pgaddr & NOPAGE_MASK);
}

static __inline u32 __virt2phys(u32 addr)
{
	return addr;
}

#endif

