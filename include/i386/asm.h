
/*
 * include/i386/asm.h 
 *
 * Copyright (C) 2006 Alexander Shishkin
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
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 */

#ifndef __ARCH_ASM_H__
#define __ARCH_ASM_H__

/* allow kernel to consume 1/KERN_ALLOWANCE of available physical memory */
#define KERN_ALLOWANCE 8

#define USERMEM_START 0x40000000
#define USERMEM_STACK 0x80000000
#define USERMEM_STACKLIM 0x0000f000
#define USERMEM_HEAP  0x80000000
#define USERMEM_HEAPLIM 0x00080000
#define USERMEM_MBOX  0xa0000000
#define in_kernel(p) ((u32)p < USERMEM_START ? 1 : 0)

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

#ifndef __ASSEMBLY__
#include <i386/segments.h>
#define reset_stack() \
	do { \
		CURRENT()->context.esp = (u32)CURRENT() - 4; \
		root_tss.esp0 = (u32)CURRENT() - 4; \
	} while (0);

struct register_frame {
	u32 eax;
	u32 ebx;
	u32 ecx;
	u32 edx;
	u32 esi;
	u32 edi;
	u32 ebp;
	u32 esp;
	u32 prev_eip;
	u32 prev_cs;
	u32 prev_eflags;
	u32 prev_esp;
	u32 prev_ss;
};

/* read/write cpu control registers */
#define READ_REG(reg, REG) \
static __inline unsigned long read_ ##reg() \
{ \
	unsigned long r; \
	__asm__ __volatile__("movl " REG ", %0" : "=r"(r)); \
	return r; \
}

#define WRITE_REG(reg, REG) \
static __inline void write_ ##reg(unsigned long v) \
{ \
	__asm__ __volatile__("movl %0, " REG : : "r"(v)); \
}

/* read segment registers */
#define READ_SEGREG(reg, OP) \
static __inline unsigned short read_ ##reg() \
{ \
	unsigned short r; \
	__asm__ __volatile__(OP "\nmovw %%ax, %0" : "=r"(r) : : "ax"); \
	return r; \
}

/* write segment registers */
#define WRITE_SEGREG(reg, OP) \
static __inline void write_ ##reg(unsigned short v) \
{ \
	__asm__ __volatile__("movw %0, %%ax\n" OP : : "m"(v) : "ax"); \
}

READ_REG(cr0, "%%cr0")
READ_REG(cr2, "%%cr2")
READ_REG(cr3, "%%cr3")
WRITE_REG(cr0, "%%cr0")
WRITE_REG(cr2, "%%cr2")
WRITE_REG(cr3, "%%cr3")

WRITE_SEGREG(tr, "ltr %%ax")
READ_SEGREG(cs, "movw %%cs, %%ax")
READ_SEGREG(ss, "movw %%ss, %%ax")
READ_SEGREG(ds, "movw %%ds, %%ax")
READ_SEGREG(es, "movw %%es, %%ax")
READ_SEGREG(fs, "movw %%fs, %%ax")
READ_SEGREG(gs, "movw %%gs, %%ax")
WRITE_SEGREG(ss, "movw %%ax, %%ss")
WRITE_SEGREG(ds, "movw %%ax, %%ds")
WRITE_SEGREG(es, "movw %%ax, %%es")
WRITE_SEGREG(fs, "movw %%ax, %%fs")
WRITE_SEGREG(gs, "movw %%ax, %%gs")

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

static __inline u8 inb(u16 port)
{
	u8 r;
	__asm__ __volatile__("inb %w1, %0" : "=a"(r) : "d"(port));
	return r;
}

static __inline void outb(u16 port, u8 data)
{
	__asm__ __volatile__("outb %0, %w1" : : "a"(data), "d"(port));
}

static __inline u16 inw(u16 port)
{
	u16 r;
	__asm__ __volatile__("inw %w1, %0" : "=a"(r) : "d"(port));
	return r;
}

static __inline void outw(u16 port, u16 data)
{
	__asm__ __volatile__("outb %0, %w1" : : "a"(data), "d"(port));
}

/* read/write cpu flags */
static __inline u32 read_eflags()
{
	u32 f;
	__asm__ __volatile__("pushfl; popl %0" : "=r"(f));
	return f;
}

static __inline void write_eflags(u32 f)
{
	__asm__ __volatile__("pushl %0; popfl" : : "r"(f));
}
#define EFLAGS_IF	0x0200

static __inline void local_irq_disable()
{
        __asm__ __volatile__ ("cli");
}

static __inline void local_irq_enable()
{
        __asm__ __volatile__ ("sti");
}

#endif /* __ASSEMBLY__ */

#endif /* __ARCH_ASM_H__ */

