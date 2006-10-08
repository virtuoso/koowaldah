
/*
 * include/irq.h
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

void interrupts_init();

int register_irq_handler(u32 number, void (*handler)());
void unregister_irq_handler(u32 number);

inline void disable_interrupts();
inline void enable_interrupts();


#endif /* __INTERRUPTS_H__ */
