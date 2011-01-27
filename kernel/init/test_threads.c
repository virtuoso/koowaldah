/*
 * kernel/init/test_threads.c
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
#include <thread.h>
#include <scheduler.h>
#include <timers.h>

static void __future func1(void *data)
{
	for (;;) {
		kprintf("A");
		tsleep(50);
	}

}

static void __future func2(void *data)
{
	for (;;) {
		kprintf("B");
		tsleep(70);
	}
}

void test_threads()
{
#ifdef OPT_TEST_THREADS
	struct thread *thread;
	struct thread_queue q;

	tq_init(&q);

        thread = thread_create(&func1, "[thread A]", NULL);
        if (!thread) {
                kprintf("failed to create thread\n");
		bug();
        }
        kprintf("Thread A created.\n");

	tq_insert_head(thread, &q);
        scheduler_enqueue(&q);

        kprintf("Thread A added to run queue.\n");

        thread = thread_create(&func2, "[thread B]", NULL);
        if (!thread) {
                kprintf("failed to create thread\n");
		bug();
        }
        kprintf("Thread B created.\n");

	tq_insert_head(thread, &q);

        scheduler_enqueue(&q);
        kprintf("Thread B added to run queue.\n");
#endif /* OPT_TEST_THREADS */
}

