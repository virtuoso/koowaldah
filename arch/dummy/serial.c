/*
 * arch/dummy/serial.c
 *
 * Copyright (C) 2008 Alexander Shishkin
 *
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

#include <koowaldah.h>
#include <bug.h>
#include <irq.h>
#include <arch/asm.h>
#include <thread.h>
#include <file.h>
#include <device.h>
#include <khui/errno.h>
#include <kqueue.h>
#include <scheduler.h>
#include <dummy/loader.h>

int serial_dummy_open(struct file *file)
{
	return 0;
}

int serial_dummy_close(struct file *file)
{
	return 0;
}

int serial_dummy_read(struct file *file, char *buf, size_t size)
{
	return 0;
}

int serial_dummy_write(struct file *file, char *buf, size_t size)
{
	int i = 0;

	while (i < size)
		dummy_putc(buf[i++]);

	return i;
}

static struct file_operations serial_dummy_fops = {
	.open  = serial_dummy_open,
	.close = serial_dummy_close,
	.read  = serial_dummy_read,
	.write = serial_dummy_write,
};

static struct device serial_dummy_dev = {
	.d_name  = {"serial_dummy"},
	.d_dev   = DEV_DEVICE(12, 34),
	.d_flags = DF_RDONLY | DF_WRONLY | DF_CHAR,
	.d_fops  = &serial_dummy_fops
};

int __init serial_dummy_load()
{
	kprintf("Loading the serial dummy driver.\n");
	register_device(&serial_dummy_dev);
	kprintf("Serial driver for the dummy loaded.\n");

	return 0;
}

late_init(serial_dummy_load);

