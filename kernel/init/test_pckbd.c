/*
 * kernel/init/test_pckdb.c
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
#include <file.h>
#include <console.h>
#include <thread.h>
#include <scheduler.h>

#ifdef OPT_TEST_PCKBD

static void kbd_reader()
{
	int fd;
	u16 buf;

	fd = open("/dev/pckbd", 0);
	if (fd < 0) {
		kprintf("Error opening /dev/pckbd: %d\n", fd);
		bug();
	}
	kprintf("Keyboard input: ");
	for (;;) {
		if(2 != read(fd, (char *)&buf, 0)) bug();
		console_put_char(buf >> 8);
		//console_put_char(buf & 0xFF);
	}
}
#endif /* OPT_TEST_PCKBD */

void test_pckbd()
{
#ifdef OPT_TEST_PCKBD
	struct thread *thread;
	struct thread_queue q;

	tq_init(&q);

        thread = thread_create(&kbd_reader, "[keyboard]", NULL);
        if (!thread) {
                kprintf("failed to create thread\n");
		bug();
        }
        kprintf("Thread keyboard created.\n");

	tq_insert_head(thread, &q);

        if (!scheduler_enqueue(&q)) {
                kprintf("failed to add thread to run queue\n");
		bug();
        }
        kprintf("Thread keyboard added to run queue.\n");
#endif /* OPT_TEST_PCKBD */
}

