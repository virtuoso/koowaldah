/*
 * kernel/init/test_kqueue.c
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
#include <kqueue.h>

void test_kqueue()
{
#ifdef OPT_TEST_KQUEUE
	struct kqueue_t *q;
	/*
	char p1 = 0xA1;
	char p2 = 0xA2;
	char p3 = 0xA3;
	char p4 = 0xA4;
	*/
	int i, t;

	q = kqueue_create();

	kprintf("Pushing 10000 numbers into the queue.\n");
	for (i = 0; i < 10000; i++) {
		kprintf("%d ", i);

		kqueue_push(q, (char*)&i, sizeof(int));
	//	kqueue_info(q);
	}
	kqueue_info(q);


	kprintf("\nDone.");

	kprintf("Now getting them back.\n");

	for (i = 0; i < 10000; i++) {
		kqueue_pull_tail(q, (char*)&t, sizeof(int));
		kprintf("%d ", t);
	}

	kprintf("\nDone.\n");
	kqueue_info(q);

	kqueue_destroy(q);

#endif /* OPT_TEST_KQUEUE */
}

