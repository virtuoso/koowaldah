
/*
 * drivers/serial/serial_8250.c
 *
 * Copyright (C) 2006 Alexey Zaytsev
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
 *
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

#if 0
#define DEBUG
#endif

#include <debug.h>

/* Move to some global place later, we may have serial interfaces
 * other than i8250. */
#define SERIAL_DEV (DEV_DEVICE(12,34))

#define SERIO_1_BASE	0x3F8
#define SERIO_1_IRQ	4

#define SERIO_REG_IER	(SERIO_1_BASE + 1) /* Interrupt Enable Register */
#define SERIO_REG_MCR	(SERIO_1_BASE + 4) /* Modem Control Register */
#define SERIO_REG_LSR	(SERIO_1_BASE + 5) /* Line Status Register */


#define IER_RX	(1 << 0) /* Received Data Available Interrupt */
#define IER_TX	(1 << 1) /* Transmitter Holding Register Empty Interrupt */
#define IER_LINE (1 << 2) /* Receiver Line Status Interrupt */
#define IER_MODEM (1 << 3) /* Modem Status Interrupt */

#define LSR_READY	(1 << 0) /* Data Ready */ 
#define LSR_EOVERRUN	(1 << 1) /* Overrun Error */
#define LSR_EPARITY	(1 << 2) /* Parity Error */
#define LSR_EFRAMING	(1 << 3) /* Framing Error */
#define LSR_BREAK	(1 << 4) /* Break Interrupt */
#define LSR_EMPT_THR	(1 << 5) /* Empty Transmitter Holding Register */
#define LSR_EMPT_DHR	(1 << 6) /* Empty Data Holding Registers */
#define LSR_EFIFO	(1 << 7) /* Error in Received FIFO */

static struct thread_queue tq;

static struct kqueue_t *read_queue = NULL;
static struct kqueue_t *write_queue = NULL;


int serial_8250_open(struct file *file)
{
	DPRINT("serial_8250_open.\n");
	if (read_queue)
		return -EBUSY;
	
	read_queue = kqueue_create();
	if (!read_queue)
		return -ENOMEM;

	write_queue = kqueue_create();
	if (!write_queue) {
		kqueue_destroy(read_queue);
		return -ENOMEM;
	}

	tq_init(&tq);

	return 1;
}

int serial_8250_close(struct file *file)
{
	kqueue_destroy(read_queue);
	/* Should we delay the write queue destruction? Later. */
	kqueue_destroy(write_queue);

	/* There should be no threads waiting. */
	bug_on(scheduler_enqueue(&tq));

	return 0;
}

int serial_8250_read(struct file *file, char *buf, off_t size)
{
	int t;
	off_t num_read = 0;

	DPRINT("serial_8250_read\n");

	while (num_read < size) {
#ifdef DEBUG
		kqueue_info(read_queue);
#endif
		t = kqueue_pull_tail(read_queue, buf + num_read, size);
		DPRINT("got %d bytes from the queue.\n", t);
		num_read += t;

		if (!t) {
			/* No bytes yet arrived, take a break */
			DPRINT("No bytes to read, goind to sleep.\n");

			scheduler_dequeue(&tq);
			scheduler_yield();
		}
	}

	return num_read;
}

static void enable_tx_irq()
{
	u8 t = inb(SERIO_REG_IER);
	
	t |= IER_TX;
	outb(SERIO_REG_IER, t);
}

static void disable_tx_irq()
{
	u8 t = inb(SERIO_REG_IER);
	
	t &= ~IER_TX;
	outb(SERIO_REG_IER, t);
}




int serial_8250_write(struct file *file, char *buf, off_t size)
{
	int res;

	DPRINT("serial_8250_write\n");
	res = kqueue_push(write_queue, buf, size);
	DPRINT("Pished.\n");
	enable_tx_irq();

	while (kqueue_size(write_queue)) {
		DPRINT("Sleep until all those bytes are written.\n");

		scheduler_dequeue(&tq);
		scheduler_yield();
	}

	return size;
}

static void serial_8250_intr(u32 number)
{
	u8 line_status;
	u8 c;
	
	DPRINT("Interrupt!\n");

	line_status = inb(SERIO_1_BASE + 5);

	DPRINT("line status = 0x%x\n", line_status);
		
	if (line_status & LSR_READY) {
		c = inb(SERIO_1_BASE);
		DPRINT("Got byte: 0x%x\n", c);

		if (read_queue) {
			kqueue_push(read_queue, (char *)&c, 1);
			scheduler_enqueue(&tq);
		}

	} else if (line_status & (LSR_EMPT_THR | LSR_EMPT_DHR)) {
		int t;
		if (write_queue) {
#ifdef DEBUG
			kqueue_info(write_queue);
#endif
			t = kqueue_pull_tail(write_queue, (char *)&c, 1);
			if (t) {
				DPRINT("Writing byte 0x%x, t = %d\n", c, t);
				outb(SERIO_1_BASE, c);
			} else {
				disable_tx_irq();
				scheduler_enqueue(&tq);
			}
		} else {
			/* Should no be reachable at all. */
			info();
			disable_tx_irq();
		}
		
	} else if (line_status) {
		kprintf("Line Status Register not clean: 0x%x\n",
			line_status);
		/* Try to handle errors here. */
	}
	DPRINT("Done.\n");
}

static struct file_operations serial_8250_fops = {
	.open = serial_8250_open,
	.close = serial_8250_close,
	.read = serial_8250_read,
	.write = serial_8250_write,
};

static struct device serial_8250_dev = {
	.d_name = {"serial_8250"},
	.d_dev = SERIAL_DEV,
	.d_flags = DF_RDONLY | DF_WRONLY | DF_CHAR,
	.d_fops = &serial_8250_fops
};

int __init serial_8250_load()
{
	kprintf("Loading the serial 8250 driver.\n");


	outb(SERIO_1_BASE + 3, 0x80); /* DLAB = 1 */

	/* When DLAB is set, reg0 and reg1 are used to set the baud rate. */
	outb(SERIO_1_BASE + 0, 1); /* Baud rate divisor, low byte. */
	outb(SERIO_1_BASE + 1, 0); /* Baud rate divisor, high byte. */

	outb(SERIO_1_BASE + 3, 0x03);	/* Bits = 8
					 * Stopbits = 1
					 * Flow control = none
					 * DLAB = 0 */

	/* Now DLAB is unset and we use reg1 as the Interrupt Enable Register.*/
	outb(SERIO_REG_IER, IER_RX);
	outb(SERIO_REG_MCR, 0x00); /* No modem control */

	register_irq_handler(SERIO_1_IRQ, serial_8250_intr);
	register_device(&serial_8250_dev);

	kprintf("Serial driver for the 8250 loaded.\n");

	return 0;
}

late_init(serial_8250_load);

