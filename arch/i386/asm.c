
/*
 * arch/i386/asm.c
 *
 * Copyright (C) 2005 Alexey Zaytsev
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


#include <kuca.h>
#include <arch/asm.h>


/* Control Register operations */

/* CR0 */
inline u32 read_cr0()
{
        u32 result;
        asm("mov %%cr0, %0": "=r"(result));
        return result;
}

inline void write_cr0(u32 value)
{
        asm("mov %0, %%cr0"::"r"(value));
}

/* CR3 */
inline u32 read_cr3()
{
        u32 result;
        asm("mov %%cr3, %0": "=r"(result));
        return result;
}

inline void write_cr3(u32 value)
{
        asm("mov %0, %%cr3"::"r"(value));
}



/* IO operations */

inline u8 io_port_in(u16 port)
{
	u8 result;
	asm("in %%dx, %%al" : "=a" (result) : "d" (port));
	return result;
}

inline void io_port_out(u16 port, u8 data)
{
	asm("out %%al, %%dx" : :"a" (data), "d" (port));
}

/* Misc */

inline u32 read_eflags()
{
	u32 result;
	
	asm volatile ("pushf");
	asm volatile ("pop %%eax" :);
	asm volatile ("movl %%eax, %0":"=r"(result));

	return result;
}

