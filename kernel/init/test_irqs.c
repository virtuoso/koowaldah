/*
 * kernel/init/tests_irqs.c
 *
 * Copyright (C) 2006 Alexey Zaytsev
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
#include <koowaldah.h>
#include <bug.h>
#include <textio.h>
#include <irq.h>
#include <scheduler.h>
#include <arch/asm.h>

#ifdef OPT_TEST_IRQS
static void keyboard_irq_handler(u32 number)
{
        unsigned char c;

	c = inb(0x60);
	switch (c) {
		case 31:
			list_threads(LIST_BACKTRACE);
			break;
		case 32:
			bug();
			break;
		case 33:
			kprintf("Timer says %d\n", (u32)jiffies);
			break;
		default:
			kprintf("keyboard, scancode = %d\n", c);
			break;
	}
}
#endif /* OPT_TEST_IRQS */

void test_irqs()
{
#ifdef OPT_TEST_IRQS
        kprintf("Registering dummy keyboard interrupt service routine...");
        register_irq_handler(1, keyboard_irq_handler);
        kprintf("Done\n");
#endif /* OPT_TEST_IRQS */
}

