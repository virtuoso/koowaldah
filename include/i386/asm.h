
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
 * 3. Neither the name of the Koowaldah developers nor the names of theyr 
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

/* page size, bits, mask, unmask */
#define PAGE_SHIFT 12
#define PAGE_SIZE (1UL << PAGE_SHIFT)
#define PAGE_MASK (PAGE_SIZE - 1)
#define NOPAGE_MASK ~(PAGE_MASK)

/* page translation constants */
#define PTE_BITS  32
#define PTE_BYTES  (PTE_BITS/8)
#define PGD_SHIFT 22
#define PGT_SHIFT 12
#define PGD_ENTRIES (1 << (PTE_BITS - PGD_SHIFT))
#define PGT_ENTRIES (1 << (PGD_SHIFT - PGT_SHIFT))
#define PGD_MASK 0xffc00000
#define PGT_MASK 0x003ff000

/* page table flags */
#define PTF_PRESENT  (1 << 0)
#define PTF_RW       (1 << 1)
#define PTF_USER     (1 << 2)
#define PTF_PWT      (1 << 3)
#define PTF_PCD      (1 << 4)
#define PTF_ACCESSED (1 << 5)
#define PTF_DIRTY    (1 << 6)
/* more to come as needed */

/* memory mapping/page directory */
struct mapping {
	u32 *m_pgdir;
	u32 *m_pgtable;
};

extern struct mapping root_map;

/* read/write cpu control registers */
#define READ_REG(reg, REG) \
static inline unsigned long read_ ##reg() \
{ \
	unsigned long r; \
	__asm__ __volatile__("movl " REG ", %0" : "=r"(r)); \
	return r; \
}

#define WRITE_REG(reg, REG) \
static inline void write_ ##reg(unsigned long v) \
{ \
	__asm__ __volatile__("movl %0, " REG : : "r"(v)); \
}

READ_REG(cr0, "%%cr0")
READ_REG(cr2, "%%cr2")
READ_REG(cr3, "%%cr3")
WRITE_REG(cr0, "%%cr0")
WRITE_REG(cr2, "%%cr2")
WRITE_REG(cr3, "%%cr3")

/*
 * Lookup the virtual __page__ address in the directory.
 * No checks are performed as to verify presence of a page;
 * in case of absence, page fault will raise.
 */
static inline u32 __virt2physpg(u32 pgaddr)
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
			: "=r"(ret) : "m"(pgaddr)
		);
	return ret;
}

/*
 * Lookup the virtual address in the directory.
 * No checks are performed as to verify presence of a page;
 * in case of absence, page fault will raise.
 */
static inline u32 __virt2phys(u32 addr)
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
			: "=r"(ret) : "m"(addr)
		);
	return ret;
}

void copy_map(struct mapping *dst, struct mapping *map);
void init_user_map(struct mapping *map);
void switch_map(struct mapping *from, struct mapping *to);

inline u8 io_port_in(u16 port);

inline void io_port_out(u16 port, u8 data);

/* read/write cpu flags */
static inline u32 read_eflags()
{
	u32 f;
	__asm__ __volatile__("pushfl; popl %0" : "=r"(f));
	return f;
}

static inline u32 write_eflags()
{
	u32 f;
	__asm__ __volatile__("pushl %0; popfl" : : "r"(f));
	return f;
}

#endif /* __ARCH_ASM_H__ */
