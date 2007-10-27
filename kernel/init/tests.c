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
 * 3. Neither the name of the Koowaldah developers nor the names of their 
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

#include <koowaldah.h>
#include <console.h>
#include <page_alloc.h>
#include <slice.h>
#include <galloc.h>
#include <mm_zone.h>
#include <arch/asm.h>
#include <irq.h>
#include <timers.h>
#include <textio.h>
#include <thread.h>
#include <scheduler.h>
#include <namespace.h>
#include <lib.h>
#include <bug.h>
#include <kqueue.h>
#include <spinlock.h>

struct page *pages[2048];

static void test_mm()
{
#ifdef OPT_PAGE_ALLOCATOR_TEST
	struct page *pg[7];

	int i;

	kprintf("Testing the page allocator...\n");
	print_alloc_info();

	pg[1] = alloc_pages(0, 0);
	kprintf("got page 0x%x, order %d\n", pg[1]->index * PAGE_SIZE, pg[1]->private.order);
	print_alloc_info();

	kprintf("\nTest 1 passed\n\n");

	pg[2] = alloc_pages(0, 1);
	kprintf("got page 0x%x, order %d\n", pg[2]->index * PAGE_SIZE, pg[2]->private.order);
	print_alloc_info();

	kprintf("\nTest 2 passed\n\n");


	kprintf("free 1\n");
	free_pages(pg[1]);
	print_alloc_info();
	
	pg[3] = alloc_pages(0, 3);
	kprintf("got page 0x%x, order %d\n", pg[3]->index * PAGE_SIZE, pg[3]->private.order);
	print_alloc_info();
	kprintf("\nTest 3 passed\n\n");

	pg[4] = alloc_pages(0, 1);
	kprintf("got page 0x%x, order %d\n", pg[4]->index * PAGE_SIZE, pg[4]->private.order);
	print_alloc_info();
	kprintf("\nTest 4 passed\n\n");

	kprintf("free 2\n");
	free_pages(pg[2]);
	print_alloc_info();

	pg[5] = alloc_pages(0, 4);
	kprintf("got page 0x%x, order %d\n", pg[5]->index * PAGE_SIZE, pg[5]->private.order);
	print_alloc_info();
	kprintf("\nTest 5 passed\n\n");

	pg[6] = alloc_pages(0, 2);
	kprintf("got page 0x%x, order %d\n", pg[6]->index * PAGE_SIZE, pg[6]->private.order);
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

	pg[0] = alloc_pages(0, 1);
	pg[1] = alloc_pages(0, 2);
	pg[2] = alloc_pages(0, 3);
	pg[3] = alloc_pages(0, 4);
	pg[4] = alloc_pages(0, 5);
	pg[5] = alloc_pages(0, 6);
	
	kprintf("Now allocating 2048 0-order pages.\n");

	for (i = 0; i < 2048; i++) {
		pages[i] = alloc_page(0);
	}

	kprintf("Done allocating 2048 0-roder pages.\n");

	print_alloc_info();

	kprintf("Freeing even pages\n");
	
	for (i = 0; i < 1048; i = i + 2) {
		free_page(pages[i]);
	}

	kprintf("Done freeing even pages.\n");

	print_alloc_info();



	kprintf("pg[0] = 0x%x\npg[1] = 0x%x\npg[2] = 0x%x\npg[3] = 0x%x\n"
		"pg[4] = 0x%x\npg[5] = 0x%x\npg[6] = 0x%x\n",
		pg[0], pg[1], pg[2], pg[3], pg[4], pg[5], pg[6]);

	kprintf("page_to_addr(pg[0]) = 0x%x\n", page_to_addr(pg[0]));
	kprintf("page_to_addr(pg[1]) = 0x%x\n", page_to_addr(pg[1]));
	kprintf("page_to_addr(pg[2]) = 0x%x\n", page_to_addr(pg[2]));
	kprintf("page_to_addr(pg[3]) = 0x%x\n", page_to_addr(pg[3]));
	kprintf("page_to_addr(pg[4]) = 0x%x\n", page_to_addr(pg[4]));
	kprintf("page_to_addr(pg[5]) = 0x%x\n", page_to_addr(pg[5]));
	kprintf("page_to_addr(pg[6]) = 0x%x\n", page_to_addr(pg[6]));

	
	kprintf("addr_to_page(page_to_addr(pg[0])) = 0x%x\n",
		addr_to_page(page_to_addr(pg[0])));
	kprintf("addr_to_page(page_to_addr(pg[1])) = 0x%x\n",
		addr_to_page(page_to_addr(pg[1])));
	kprintf("addr_to_page(page_to_addr(pg[2])) = 0x%x\n",
		addr_to_page(page_to_addr(pg[2])));
	kprintf("addr_to_page(page_to_addr(pg[3])) = 0x%x\n",
		addr_to_page(page_to_addr(pg[3])));
	kprintf("addr_to_page(page_to_addr(pg[4])) = 0x%x\n",
		addr_to_page(page_to_addr(pg[4])));
	kprintf("addr_to_page(page_to_addr(pg[5])) = 0x%x\n",
		addr_to_page(page_to_addr(pg[5])));
	kprintf("addr_to_page(page_to_addr(pg[6])) = 0x%x\n",
		addr_to_page(page_to_addr(pg[6])));

	free_page(pg[0]);
	free_page(pg[1]);
	free_page(pg[2]);
	free_page(pg[3]);
	free_page(pg[4]);
	free_page(pg[5]);
	free_page(pg[6]);

	kprintf("Done.\n");
#endif
}

u32 * slices[1024];
static void test_slice_alloc()
{
#ifdef OPT_TEST_SLICE
	struct slice_pool *slice;
	int i;

	kprintf("Testing allocator with 4-byte slices: \n");

	slice = slice_pool_create(0, 4);
	kprintf("Created a fresh slice allocator\n");

	slice_pool_info(slice);

	kprintf("Allocating 1024 4-byte slices.\n");
	
	for (i = 0; i < 1024; i++) {
		/* kprintf("Allocating object %d\n", i); */
		slices[i] = slice_alloc(slice);
		/* kprintf("Allocated object %d\n", i); */
		/* lice_info(slice); */
	}

	kprintf("Done with allocating 1024 4-byte objects.\n");

	slice_pool_info(slice);

	
	
	/*
	for (i = 0; i < 1024; i++) {
		kprintf("object %d = 0x%x\n", i, objects[i]);
	}
	*/
	
	kprintf("Freeing even slices\n");
	for (i = 0; i < 1024; i = i + 2) {
		/* kprintf("Freeind object %d\n", i); */
		slice_free(slices[i], slice);
		/* kprintf("Done freeing object %d\n", i);*/
		/* slice_pool_info(slice); */
	}
	
	kprintf("Done freeing even pages.\n");
	slice_pool_info(slice);

	kprintf("Allocating even slices back.\n");

	for (i = 0; i < 1024; i = i + 2) {
		slices[i] = slice_alloc(slice);
	}

	kprintf("Done allocating even slices back.\n");

	slice_pool_info(slice);

	kprintf("Freeing odd slices.\n");

	for (i = 1; i < 1024; i = i + 2) {
		slice_free(slices[i], slice);
	}

	kprintf("Done freeing odd slices.\n");

	slice_pool_info(slice);

	kprintf("Freeing even slices.\n");

	for (i = 0; i < 1024; i = i + 2) {
		slice_free(slices[i], slice);
	}

	kprintf("Done freeing even slices.\n");

	slice_pool_info(slice);

	kprintf("Recycling slice pool.\n");

	slice_pool_recycle(slice);

	kprintf("Done recycling slice pool.\n");

	slice_pool_info(slice);
		

	kprintf("Done with testing slice allocator on 4-byte slices.\n");
	
	kprintf("Testing allocator with 40-byte slices: \n");

	slice = slice_pool_create(0, 40);
	kprintf("Created a fresh slice allocator\n");

	slice_pool_info(slice);

	kprintf("Allocating 1024 40-byte slices.\n");
	
	for (i = 0; i < 1024; i++) {
		/* kprintf("Allocating object %d\n", i); */
		slices[i] = slice_alloc(slice);
		/* kprintf("Allocated object %d\n", i); */
		/* lice_info(slice); */
	}

	kprintf("Done with allocating 1024 40-byte objects.\n");

	slice_pool_info(slice);

	
	
	/*
	for (i = 0; i < 1024; i++) {
		kprintf("object %d = 0x%x\n", i, objects[i]);
	}
	*/
	
	kprintf("Freeing even slices\n");
	for (i = 0; i < 1024; i = i + 2) {
		/* kprintf("Freeind object %d\n", i); */
		slice_free(slices[i], slice);
		/* kprintf("Done freeing object %d\n", i);*/
		/* slice_pool_info(slice); */
	}
	
	kprintf("Done freeing even pages.\n");
	slice_pool_info(slice);

	kprintf("Allocating even slices back.\n");

	for (i = 0; i < 1024; i = i + 2) {
		slices[i] = slice_alloc(slice);
	}

	kprintf("Done allocating even slices back.\n");

	slice_pool_info(slice);

	kprintf("Freeing odd slices.\n");

	for (i = 1; i < 1024; i = i + 2) {
		slice_free(slices[i], slice);
	}

	kprintf("Done freeing odd slices.\n");

	slice_pool_info(slice);

	kprintf("Freeing even slices.\n");

	for (i = 0; i < 1024; i = i + 2) {
		slice_free(slices[i], slice);
	}

	kprintf("Done freeing even slices.\n");

	slice_pool_info(slice);

	kprintf("Recycling slice pool.\n");

	slice_pool_recycle(slice);

	kprintf("Done recycling slice pool.\n");

	slice_pool_info(slice);

	kprintf("Done with testing slice allocator on 40-byte slices.\n");
	
	kprintf("Testing allocator with 200-byte slices: \n");

	slice = slice_pool_create(0, 200);
	kprintf("Created a fresh slice allocator\n");

	slice_pool_info(slice);

	kprintf("Allocating 1024 200-byte slices.\n");
	
	for (i = 0; i < 1024; i++) {
		/* kprintf("Allocating object %d\n", i); */
		slices[i] = slice_alloc(slice);
		/* kprintf("Allocated object %d\n", i); */
		/* lice_info(slice); */
	}

	kprintf("Done with allocating 1024 200-byte objects.\n");

	slice_pool_info(slice);

	
	
	/*
	for (i = 0; i < 1024; i++) {
		kprintf("object %d = 0x%x\n", i, objects[i]);
	}
	*/
	
	kprintf("Freeing even slices\n");
	for (i = 0; i < 1024; i = i + 2) {
		/* kprintf("Freeind object %d\n", i); */
		slice_free(slices[i], slice);
		/* kprintf("Done freeing object %d\n", i);*/
		/* slice_pool_info(slice); */
	}
	
	kprintf("Done freeing even pages.\n");
	slice_pool_info(slice);

	kprintf("Allocating even slices back.\n");

	for (i = 0; i < 1024; i = i + 2) {
		slices[i] = slice_alloc(slice);
	}

	kprintf("Done allocating even slices back.\n");

	slice_pool_info(slice);

	kprintf("Freeing odd slices.\n");

	for (i = 1; i < 1024; i = i + 2) {
		slice_free(slices[i], slice);
	}

	kprintf("Done freeing odd slices.\n");

	slice_pool_info(slice);

	kprintf("Freeing even slices.\n");

	for (i = 0; i < 1024; i = i + 2) {
		slice_free(slices[i], slice);
	}

	kprintf("Done freeing even slices.\n");

	slice_pool_info(slice);

	kprintf("Recycling slice pool.\n");

	slice_pool_recycle(slice);

	kprintf("Done recycling slice pool.\n");

	slice_pool_info(slice);

	kprintf("Done with testing slice allocator on 200-byte slices.\n");

#endif /* OPT_TEST_SLICE */
}

u32 * tmp[2044];


static void test_galloc()
{
#ifdef OPT_TEST_GALLOC

	int i;

	for (i = 0; i < 2044; i++) {
		kprintf("Allocating chunk, size = %d\n", i);

		tmp[i] = galloc(0, i);
		if (!tmp[i]) {
			kprintf("Failed!\n");
			bug();
		}
	}

	for (i = 0; i < 2044; i = i + 2) {

		kprintf("Freeing chunk %d (0x%x) \n", i, (u32) tmp[i]);
		

		gfree(tmp[i]);
	
	}
	for (i = 1; i < 2044; i = i + 2) {

		kprintf("Freeing chunk %d\n", i);

		gfree(tmp[i]);
	
	}

	for (i = 0; i < 100; i++) {
		tmp[i] = galloc(0, PAGE_SIZE * i);
		if (!tmp[i]) {
			kprintf("Failed\n");
			bug();
		}
	}

	for (i = 0; i < 100; i++) {
		kprintf("Freeing chunk %d\n", i);
		gfree(tmp[i]);
	}

	kprintf("Done.");



#endif
}

static void test_kqueue()
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

		kqueue_push(q, &i, sizeof(int));
	//	kqueue_info(q);
	}
	kqueue_info(q);


	kprintf("\nDone.");

	kprintf("Now getting them back.\n");

	for (i = 0; i < 10000; i++) {
		kqueue_pull_tail(q, &t, sizeof(int));
		kprintf("%d ", t);
	}

	kprintf("\nDone.\n");
	kqueue_info(q);

	kqueue_destroy(q);

#endif /* OPT_TEST_KQUEUE */
}

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

static void test_threads()
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

#ifdef OPT_TEST_IRQS
static void keyboard_irq_handler(u32 number)
{
        unsigned char c;

        c = io_port_in(0x60);
	switch (c) {
		case 31:
			list_threads(LIST_BACKTRACE);
			break;
		case 32:
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
#endif /* OPT_TEST_IRQS */

static void test_irqs()
{
#ifdef OPT_TEST_IRQS
        kprintf("Registering dummy keyboard interrupt service routine...");
        register_irq_handler(1, keyboard_irq_handler);
        kprintf("Done\n");
#endif /* OPT_TEST_IRQS */
}

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

static void test_serial()
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

static void test_pckbd()
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

static void test_fslookup()
{
#ifdef OPT_TEST_FSLOOKUP
	struct direntry *dent = lookup_path("/sbin/init");
	int fd, l;
	char buf[256];
	
	if (!dent)
		bug();

	kprintf("FS test found: %s\n", dent->d_name);

	fd = open("/init", 0);
	if (fd < 0) {
		kprintf("Open failed! %d\n", fd);
		return;
	}
	kprintf("Opened a file descriptor %d\n", fd);

	memory_set(buf, 0, 256);
	do {
		l = read(fd, buf, 100);
		if (l < 0) {
			kprintf("Error %d occured\n", l);
			return;
		}
		kprintf("Read %d bytes, [%s]\n", l, buf);
		break;
	} while (l == 100);
#endif
}


static void test_rootfs()
{
#ifdef OPT_TEST_ROOTFS
	struct direntry *dent;

	dent = lookup_path("/dev/console");
	if (!dent)
		kprintf("/dev/console not found.\n");
#endif
}

static void test_bug()
{
#ifdef OPT_TEST_BUG
	kprintf("BUG!!!!!!!\n");
	bug();
#endif
}

static void test_panic()
{
#ifdef OPT_TEST_PANIC
	kprintf("PANIC!!!!!!!\n");
	panic("Test!");
#endif
}

static void test_pf()
{
#ifdef OPT_TEST_PF
	u32 * address = (u32 *) 0xDEADBEEF;
	
	kprintf("We are going to have a page fault at 0x%x\n",
		(u32) address);

	*address = 0;

	/* We should be already dead now. */
	bug();
#endif
}

void run_tests()
{
	test_mm();
	test_slice_alloc();
	test_galloc();
	test_kqueue();
	test_threads();
	test_pckbd();
	test_serial();
	test_irqs();
	test_rootfs();
	test_fslookup();
	test_bug();
	test_panic();
	test_pf();
}

