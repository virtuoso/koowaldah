/*
 * arch/dummy/serial.c
 *
 * Copyright (C) 2008 Alexander Shishkin
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
 * THIS SOFTWARE IS PROVIDED BY THE DEVELOPERS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE DEVELOPERS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
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

