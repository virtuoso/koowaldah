/*
 * kernel/init/test_serial.c
 *
 * Copyright (C) 2006 Alexey Zaytsev
 * Copyright (C) 2006 Alexander Shishkin
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
 * 
 */
#include <koowaldah.h>
#include <textio.h>
#include <bug.h>
#include <thread.h>
#include <scheduler.h>

#ifdef OPT_TEST_SERIAL
static void do_test_serial()
{
	int fd;
	int tmp;
	char buffer[11];

	kprintf("Serial driver test.\n");
	fd = open("/dev/serial", 0);
	if (fd < 0) {
		kprintf("Error opening /dev/serial_8250: %d\n", fd);
		bug();
	}
	
	kprintf("Reading 10 bytes from the serial port.\n");
	tmp = read(fd, buffer, 10);
	kprintf("result = %d\n", tmp);
	buffer[10] = '\0';

	kprintf("Buffer = \"%s\"\n", buffer);

	kprintf("Writing them back.\n");

	tmp = write(fd, buffer, 10);

	kprintf("result = %d\n", tmp);

	kprintf("Serial Input: ");
	for (;;) {
		tmp = read(fd, buffer, 1);
		if (tmp != 1)
			bug();
		buffer[1] = '\0';

		kprintf("%s", buffer);
	}

	
	kprintf("Done.\n");
}
#endif /* OPT_TEST_SERIAL */

void test_serial()
{
#ifdef OPT_TEST_SERIAL
	struct thread *thread;
	struct thread_queue q;

	tq_init(&q);

	thread = thread_create(&do_test_serial, "[serial]", NULL);
	if (!thread)
		bug();

	tq_insert_head(thread, &q);
	if (!scheduler_enqueue(&q)) {
		bug();
	}
#endif

}
