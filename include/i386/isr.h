
/*
 * include/i386/isr.h 
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#ifndef __ARCH_ISR_H__
#define __ARCH_ISR_H__

void isr_init(); 
int isr_insert_entry(unsigned int number, void (* handler)());


#endif /* __ARCH_ISR_H__ */
