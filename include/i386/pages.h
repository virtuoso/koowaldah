/*
 * include/i386/pages.h
 *
 * Copyright (C) 2006, 2007 Alexander Shishkin
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

#ifndef __ARCH_PAGES_H__
#define __ARCH_PAGES_H__

/* page size, bits, mask, unmask */
#define PAGE_SHIFT 12
#define PAGE_SIZE (1UL << PAGE_SHIFT)
#define PAGE_MASK (PAGE_SIZE - 1)
#define NOPAGE_MASK ~(PAGE_MASK)
#define NOPAGE_ADDR (~0UL)

/* page translation constants */
#define PTE_BITS  32
#define PTE_BYTES  (PTE_BITS/8)
#define PGD_SHIFT 22
#define PGT_SHIFT 12
#define PGD_ENTRIES (1 << (PTE_BITS - PGD_SHIFT))
#define PGT_ENTRIES (1 << (PGD_SHIFT - PGT_SHIFT))
#define PGD_MASK 0xffc00000
#define PGT_MASK 0x003ff000

#define PGDIDX(addr) ((addr) >> PGD_SHIFT)
#define PGTIDX(addr) (((addr) & PGT_MASK) >> PGT_SHIFT)

/* page table flags                                      */
#define PTF_EXEC     (0)       /* for compatibility      */
#define PTF_PRESENT  (1 << 0)  /* page/pte is present    */
#define PTF_RW       (1 << 1)  /* write access available */
#define PTF_USER     (1 << 2)  /* accessible by user     */
#define PTF_PWT      (1 << 3)  /* prevent write-through  */
#define PTF_PCD      (1 << 4)  /* cache disable          */
#define PTF_ACCESSED (1 << 5)  /* page has been accessed */
#define PTF_DIRTY    (1 << 6)  /* page is dirty          */
#define PTF_PGSIZE   (1 << 7)  /* page size = 4M         */
#define PTF_GLOBAL   (1 << 8)  /* global (for TLB)       */

#define PTE_DESC(addr, attrs)      \
	(                          \
	 ((addr) << PAGE_SHIFT)  | \
	 (attrs)                   \
	)

/* Extract parts of page table/page directory entries */
#define PE_ADDR(pe) (pe & 0xFFFFF000)
#define PE_FLAGS(pe) (pe & 0x00000FFF)

/* page fault error code bits                                        */
/*      name      bit         cause / otherwise                      */
#define PFLT_PROT  (0x01)  /* protection violation, oth. non-present */
#define PFLT_WRITE (0x02)  /* write operation, oth. read operation   */
#define PFLT_USER  (0x04)  /* in user mode, oth. in supervisor mode  */
#define PFLT_RESB  (0x08)  /* reserved bit violation                 */

/*
 * Lookup the virtual __page__ address in the directory.
 * No checks are performed as to verify presence of a page;
 * in case of absence, page fault will raise.
 */
static __inline u32 __virt2physpg(u32 pgaddr)
{
	u32 ret;

	__asm__ __volatile__(
			"movl %1, %%eax\n"
			"andl %2, %%eax\n"    /* PGD_MASK */
			"shrl $20, %%eax\n"
			"movl %%cr3, %%ebx\n" /* page dir */
			"addl %%ebx, %%eax\n"
			"movl (%%eax), %%eax\n"
			"andl %3, %%eax\n"    /* NOPAGE_MASK */
			"movl %%eax, %%ebx\n" /* page table */
			"movl %1, %%eax\n"
			"andl %4, %%eax\n"    /* PGT_MASK */
			"shrl $10, %%eax\n"
			"lea (%%eax, %%ebx, 1), %%eax\n"
			"movl (%%eax), %%eax\n"
			"andl %3, %%eax\n"    /* NOPAGE_MASK */
			"movl %%eax, %0"
			: "=r"(ret) : "m"(pgaddr),
			"i"(PGD_MASK), "i"(NOPAGE_MASK), "i"(PGT_MASK)
			: "eax", "ebx"
		);
	return ret;
}

/*
 * Lookup the virtual address in the directory.
 * No checks are performed as to verify presence of a page;
 * in case of absence, page fault will raise.
 */
static __inline u32 __virt2phys(u32 addr)
{
	u32 ret;

	__asm__ __volatile__(
			"movl %1, %%eax\n"
			"andl %2, %%eax\n"    /* PGD_MASK */
			"shrl $20, %%eax\n"
			"movl %%cr3, %%ebx\n" /* page dir */
			"addl %%ebx, %%eax\n"
			"movl (%%eax), %%eax\n"
			"andl %3, %%eax\n"    /* NOPAGE_MASK */
			"movl %%eax, %%ebx\n" /* page table */
			"movl %1, %%eax\n"
			"andl %4, %%eax\n"    /* PGT_MASK */
			"shrl $10, %%eax\n"
			"lea (%%eax, %%ebx, 1), %%eax\n"
			"movl (%%eax), %%ebx\n"
			"andl %3, %%ebx\n"    /* NOPAGE_MASK */
			"movl %1, %%eax\n"
			"andl %5, %%eax\n"    /* PAGE_MASK */
			"orl %%ebx, %%eax\n"
			"movl %%eax, %0"
			: "=r"(ret) : "m"(addr),
			"i"(PGD_MASK), "i"(NOPAGE_MASK), "i"(PGT_MASK),
			"i"(PAGE_MASK) : "eax", "ebx"
		);
	return ret;
}

#endif

