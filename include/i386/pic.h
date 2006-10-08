
/*
 * include/i386/pic.h
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#ifndef __ARCH_PIC_H__
#define __ARCH_PIC_H__

void pic_clear();
void pic_mask_interrupt(u32 number);
void pic_unmask_interrupt(u32 number);
inline void pic_do_eoi(u32 number);

#endif /* __ARCH_PIC_H__ */
