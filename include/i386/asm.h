
/*
 * include/i386/asm.h 
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

/* disable interrupts on current cpu */
static __inline void local_irq_disable()
{
        __asm__ __volatile__ ("cli");
}

/* enable interrupts on current cpu */
static __inline void local_irq_enable()
{
        __asm__ __volatile__ ("sti");
}

#endif /* __ARCH_ASM_H__ */

