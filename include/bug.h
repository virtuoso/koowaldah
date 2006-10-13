
/*
 * include/bug.h
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#ifndef __BUG_H__
#define __BUG_H__


#include <textio.h>
#include <irq.h>

#define bug() {\
	kprintf("Achtung! BUG in file %s, line %d\n System halted.\n", __FILE__, __LINE__); \
	disable_interrupts(); \
	for(;;) \
		asm volatile ("hlt"); \
}

/* this is arch-specific, actually */
void psod();

#endif /* __BUG_H__ */
