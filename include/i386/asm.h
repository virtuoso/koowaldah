
/*
 * include/i386/asm.h 
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#ifndef __ARCH_ASM_H__
#define __ARCH_ASM_H__


inline u32 read_cr0();
inline void write_cr0(u32 value);

inline u32 read_cr3();
inline void write_cr3(u32 value);

inline u8 io_port_in(u16 port);

inline void io_port_out(u16 port, u8 data);

inline u32 read_eflags(); 

#endif /* __ARCH_ASM_H__ */
