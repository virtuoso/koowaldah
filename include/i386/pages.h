/*
 * include/i386/pages.h
 *
 * Copyright (C) 2006, 2007 Alexander Shishkin
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Koowaldah developers nor the names of their
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE DEVELOPERS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE DEVELOPERS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
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
			"andl $0xffc00000, %%eax\n"
			"shrl $20, %%eax\n"
			"movl %%cr3, %%ebx\n" /* page dir */
			"addl %%ebx, %%eax\n"
			"movl (%%eax), %%eax\n"
			"andl $0xfffff000, %%eax\n"
			"movl %%eax, %%ecx\n" /* page table */
			"movl %1, %%eax\n"
			"andl $0x003ff000, %%eax\n"
			"shrl $10, %%eax\n"
			"lea (%%eax, %%ecx, 1), %%eax\n"
			"movl (%%eax), %%eax\n"
			"andl $0xfffff000, %%eax\n"
			"movl %%eax, %0"
			: "=r"(ret) : "m"(pgaddr) : "eax", "ebx", "ecx"
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
			"andl $0xffc00000, %%eax\n"
			"shrl $20, %%eax\n"
			"movl %%cr3, %%ebx\n" /* page dir */
			"addl %%ebx, %%eax\n"
			"movl (%%eax), %%eax\n"
			"andl $0xfffff000, %%eax\n"
			"movl %%eax, %%ecx\n" /* page table */
			"movl %1, %%eax\n"
			"andl $0x003ff000, %%eax\n"
			"shrl $10, %%eax\n"
			"lea (%%eax, %%ecx, 1), %%eax\n"
			"movl (%%eax), %%edx\n"
			"andl $0xfffff000, %%edx\n"
			"movl %1, %%eax\n"
			"andl $0x00000fff, %%eax\n"
			"orl %%edx, %%eax\n"
			"movl %%eax, %0"
			: "=r"(ret) : "m"(addr) : "eax", "ebx", "ecx", "edx"
		);
	return ret;
}

#endif

