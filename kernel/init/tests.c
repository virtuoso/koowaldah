/*
 * kernel/init/tests.c
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
 * 3. Neither the name of the Koowaldah developers nor the names of theyr 
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

#include <kuca.h>
#include <console.h>
#include <page_alloc.h>
#include <mm_zone.h>
#include <arch/asm.h>
#include <arch/isr.h>
#include <irq.h>
#include <timer.h>
#include <textio.h>
#include <klist.h>
#include <thread.h>
#include <scheduler.h>
#include <bug.h>

#define TEST_IRQS 0
#define TEST_TIMER 0
#define TEST_THREADS 0
#define TEST_KLIST 0
#define PAGE_ALLOCATOR_TEST 0
#define TEST_PCKBD 0

void test_mm()
{
#if PAGE_ALLOCATOR_TEST
	struct page *pg[7];

	kprintf("Testing the page allocator...\n");

	print_alloc_info();

	pg[1] = alloc_pages(0, 0);
	kprintf("got page 0x%x, order %d\n", pg[1]->index, pg[1]->private.order);
	print_alloc_info();

	kprintf("\nTest 1 passed\n\n");

	pg[2] = alloc_pages(0, 1);
	kprintf("got page 0x%x, order %d\n", pg[2]->index, pg[2]->private.order);
	print_alloc_info();

	kprintf("\nTest 2 passed\n\n");


	kprintf("free 1\n");
	free_pages(pg[1]);
	print_alloc_info();
	
	pg[3] = alloc_pages(0, 3);
	kprintf("got page 0x%x, order %d\n", pg[3]->index, pg[3]->private.order);
	print_alloc_info();
	kprintf("\nTest 3 passed\n\n");

	pg[4] = alloc_pages(0, 1);
	kprintf("got page 0x%x, order %d\n", pg[4]->index, pg[4]->private.order);
	print_alloc_info();
	kprintf("\nTest 4 passed\n\n");

	kprintf("free 2\n");
	free_pages(pg[2]);
	print_alloc_info();

	pg[5] = alloc_pages(0, 4);
	kprintf("got page 0x%x, order %d\n", pg[5]->index, pg[5]->private.order);
	print_alloc_info();
	kprintf("\nTest 5 passed\n\n");

	pg[6] = alloc_pages(0, 2);
	kprintf("got page 0x%x, order %d\n", pg[6]->index, pg[6]->private.order);
	print_alloc_info();
	kprintf("\nTest 6 passed\n\n");

	kprintf("free 4\n");
	free_pages(pg[4]);
	print_alloc_info();

	kprintf("free 3\n");
	free_pages(pg[3]);
	print_alloc_info();
	kprintf("free 6\n");
	free_pages(pg[6]);
	print_alloc_info();

	kprintf("free 5\n");
	free_pages(pg[5]);
	print_alloc_info();

	kprintf("Done.\n");
#endif
}

void test_klist()
{
#if TEST_KLIST
        struct klist * list = NULL;
        struct klist * tmp;

        kprintf("Testing the klist implementation\n");

        kprintf("klist_is_empty(&list) = %d\n", klist_is_empty(&list));

        kprintf("tmp = klist_new(), tmp->data = 1\n");
        tmp = klist_new();
        tmp->data = (void *)1;

        kprintf("klist_add(tmp, list)\n");
        klist_add(tmp, &list);

        kprintf("klist_is_empty(&list) = %d\n", klist_is_empty(&list));

        kprintf("Traversing the list\n");

        if (!klist_is_empty(&list)) {
                tmp = list->next;
                do{
                        kprintf("got entry, list->data = %d\n", (u32) list->data);
                        klist_iter(&list);

                }while (tmp != list);
        }

        kprintf("Done with traversing\n");

        kprintf("adding 2 and 3\n");
        tmp = klist_new();
        tmp->data = (void *)2;
        klist_add(tmp, &list);
        tmp = klist_new();
        tmp->data = (void *)3;
        klist_add(tmp, &list);

        kprintf("Traversing the list\n");

        if (!klist_is_empty(&list)) {
                tmp = list;
                do {
                        kprintf("got entry, list->data = %d\n", (u32) list->data);
                        klist_iter(&list);

                } while (tmp != list);
        }

        kprintf("Done with traversing\n");

#endif /* TEST_KLIST */
}

#if TEST_THREADS
extern int tsleep(u32 delay);

void func1()
{
	for (;;) {
		kprintf("A");
		tsleep(50);
	}
}

void func2()
{
	for (;;) {
		kprintf("B");
		tsleep(70);
	}
}
#endif /* TEST_THREADS */

void test_threads()
{
#if TEST_THREADS
	struct thread_t *thread;

        thread = thread_create(&func1, "[thread A]");
        if (!thread) {
                kprintf("failed to create thread\n");
		bug();
        }
        kprintf("Thread A created.\n");

        if (scheduler_enqueue(thread)) {
                kprintf("failed to add thread to run queue\n");
		bug();
        }
        kprintf("Thread A added to run queue.\n");

        thread = thread_create(&func2, "[thread B]");
        if (!thread) {
                kprintf("failed to create thread\n");
		bug();
        }
        kprintf("Thread B created.\n");

        if (scheduler_enqueue(thread)) {
                kprintf("failed to add thread to run queue\n");
		bug();
        }
        kprintf("Thread B added to run queue.\n");
#endif /* TEST_THREADS */
}

#if TEST_IRQS
void keyboard_irq_handler(u32 number)
{
        unsigned char c;

        c = io_port_in(0x60);
	switch (c) {
		case 31:
			list_threads();
			break;
		case 32:
			psod();
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
#endif /* TEST_IRQS */

void test_irqs()
{
#if TEST_IRQS
        kprintf("Registering dummy keyboard interrupt service routine...");
        register_irq_handler(1, keyboard_irq_handler);
        kprintf("Done\n");
#endif /* TEST_IRQS */
}

#if TEST_PCKBD
extern int pckbd_open();
extern u16 pckbd_read();
extern int pckbd_load();

void kbd_reader()
{
	u16 c;

	pckbd_open();
	kprintf("Keyboard input: ");
	for (;;) {
		c = pckbd_read();
		console_put_char(c & 0xFF);
	}
}
#endif /* TEST_PCKBD */

void test_pckbd()
{
#if TEST_PCKBD && !TEST_IRQS
	struct thread_t *thread;

	pckbd_load();
        thread = thread_create(&kbd_reader, "[keyboard]");
        if (!thread) {
                kprintf("failed to create thread\n");
		bug();
        }
        kprintf("Thread keyboard created.\n");

        if (scheduler_enqueue(thread)) {
                kprintf("failed to add thread to run queue\n");
		bug();
        }
        kprintf("Thread keyboard added to run queue.\n");
#endif /* TEST_PCKBD */
}

void run_tests()
{
	test_mm();
	test_klist();
	test_threads();
	test_pckbd();
	test_irqs();
}

