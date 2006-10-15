/*
 * Copyright (c) 2006, Alexander Shishkin
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Generic PC keyboard driver.
 * This is an old-style thing, don't be scared.
 */

#include <kuca.h>
#include <bug.h>
#include <irq.h>
#include <klist0.h>
#include <error.h>
#include <thread.h>
#include <scheduler.h>
#include <arch/asm.h>
#include "table.h"

#define KEYBOARD_IRQ 1
#define KEYBOARD_IO  (0x60)
#define KEYBOARD_BUFSIZE 10

/* let's have a ring buffer */
static u8  key_buf[KEYBOARD_BUFSIZE];
static u32 read_idx, write_idx, users;

/* let's assume that kbd can be opened by no more than 1 thread,
 * so no waitqueue here */
static struct thread_t *sleeper = NULL;

static void pckbd_intr(u32 number)
{
	u8 scancode = io_port_in(KEYBOARD_IO);

	/* skip key releases for now */
	if (scancode & 0x80)
		return;

	/* rewind if necessary */
	if (write_idx == KEYBOARD_BUFSIZE)
		write_idx = 0;

	/* buffer full */
	if (write_idx+1 == read_idx)
		return;

	if (sleeper) {
		scheduler_enqueue(sleeper);
		sleeper->state = THREAD_RUNNABLE;
		sleeper = NULL;
	}
	key_buf[write_idx++] = scancode;
}

u16 pckbd_read()
{
	/* trylock would be nice here */
	disable_interrupts();

	if (read_idx == write_idx) {
		struct thread_t *thread = CURRENT();
	
		thread->state = 0;
		scheduler_dequeue(thread);
		sleeper = thread;
		enable_interrupts();
		scheduler_yield();
	}

	if (read_idx == KEYBOARD_BUFSIZE)
		read_idx = 0;

	enable_interrupts();

	return pckbd_sctrans[key_buf[read_idx++]];
}

int pckbd_open()
{
	if (users)
		return -EBUSY;

	users++;
	sleeper = NULL;
	read_idx = 0;
	write_idx = 0;
	return 0;
}

int pckbd_close()
{
	if (!users)
		return -EINVAL;
	users--;
	return 0;
}

int __lowinit pckbd_load()
{
	sleeper = NULL;
	users = read_idx = write_idx = 0; /* although, that's .bss, isn it? */
	return register_irq_handler(KEYBOARD_IRQ, pckbd_intr);
}

