
/*
 * arch/i386/asm.c
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
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
	asm volatile ("pop %%eax"::);
	asm volatile ("movl %%eax, %0":"=r"(result):);

	return result;
}

