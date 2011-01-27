/*
 * Copyright (c) 2006, Alexander Shishkin
 * All rights reserved.
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*
 * Generic PC keyboard driver.
 * This is an old-style thing, don't be scared.
 */

#include <koowaldah.h>
#include <bug.h>
#include <irq.h>
#include <klist0.h>
#include <error.h>
#include <thread.h>
#include <scheduler.h>
#include <arch/asm.h>
#include <device.h>
#include <spinlock.h>
#include "table.h"

#define KEYBOARD_IRQ 1
#define KEYBOARD_IO  (0x60)
#define KEYBOARD_BUFSIZE 10

#define PCKBD_DEV (DEV_DEVICE(13,128))

/* let's have a ring buffer */
static u8  key_buf[KEYBOARD_BUFSIZE];
static u32 read_idx, write_idx, users;


/* let's assume that kbd can be opened by no more than 1 thread,
 * so no waitqueue here */
struct thread_queue tq;


static void pckbd_intr(u32 number)
{
	u8 scancode = inb(KEYBOARD_IO);

	if (!users)
		return;

	/* skip key releases for now */
	if (scancode & 0x80)
		return;

	/* rewind if necessary */
	if (write_idx == KEYBOARD_BUFSIZE)
		write_idx = 0;

	/* buffer full */
	if (write_idx+1 != read_idx)
		key_buf[write_idx++] = scancode;

	scheduler_enqueue(&tq);
}

u16 pckbd_read()
{
	/* trylock would be nice here */
//	local_irq_save();

	if (read_idx == write_idx) {
		scheduler_dequeue(&tq);
		scheduler_yield();
		return 0;
	}

	if (read_idx == KEYBOARD_BUFSIZE)
		read_idx = 0;

//	local_irq_restore();

	return pckbd_sctrans[key_buf[read_idx++]];
}

int pckbd_open()
{
	if (users)
		return -EBUSY;

	users++;

	read_idx = 0;
	write_idx = 0;

	tq_init(&tq);
	return 0;
}

int pckbd_close()
{
	if (!users)
		return -EINVAL;
	users--;

	/* There should be no threads waiting. */
	bug_on(scheduler_enqueue(&tq));
	return 0;
}

static int pckbd_dev_open(struct file *file)
{
	return pckbd_open();
}

static int pckbd_dev_read(struct file *file, char *buf, size_t len)
{
	u16 data = pckbd_read();

	buf[0] = (u8)(data >> 8);
	buf[1] = (u8)(data & 0xff);

	return 2;
}

static int pckbd_dev_close(struct file *file)
{
	pckbd_close();
	return 0;
}

static struct file_operations pckbd_fops = {
	.open = pckbd_dev_open,
	.close = pckbd_dev_close,
	.read = pckbd_dev_read,
	.write = NULL
};

static struct device pckbd_dev = {
	.d_name = {"pckbd"},
	.d_dev = PCKBD_DEV,
	.d_flags = DF_RDONLY | DF_CHAR,
	.d_fops = &pckbd_fops
};

int __init pckbd_load()
{
	kprintf("pckbd_load()\n");
	users = read_idx = write_idx = 0; /* although, that's .bss, isn it? */
	register_device(&pckbd_dev);
	return register_irq_handler(KEYBOARD_IRQ, pckbd_intr);
}

late_init(pckbd_load);

