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

#include <koowaldah.h>
#include <console.h>
#include <page_alloc.h>
#include <slice.h>
#include <mm_zone.h>
#include <arch/asm.h>
#include <arch/isr.h>
#include <irq.h>
#include <timer.h>
#include <textio.h>
#include <klist.h>
#include <thread.h>
#include <scheduler.h>
#include <namespace.h>
#include <lib.h>
#include <bug.h>

struct page *pages[2048];

void test_mm()
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
void test_slice_alloc()
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

void test_klist()
{
#ifdef OPT_TEST_KLIST
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

#endif /* OPT_TEST_KLIST */
}

#ifdef OPT_TEST_THREADS
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
#endif /* OPT_TEST_THREADS */

void test_threads()
{
#ifdef OPT_TEST_THREADS
	struct thread *thread;

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
#endif /* OPT_TEST_THREADS */
}

#ifdef OPT_TEST_IRQS
void keyboard_irq_handler(u32 number)
{
        unsigned char c;

        c = io_port_in(0x60);
	switch (c) {
		case 31:
			list_threads();
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

void test_irqs()
{
#ifdef OPT_TEST_IRQS
        kprintf("Registering dummy keyboard interrupt service routine...");
        register_irq_handler(1, keyboard_irq_handler);
        kprintf("Done\n");
#endif /* OPT_TEST_IRQS */
}

#ifdef OPT_TEST_PCKBD

void kbd_reader()
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
#endif /* OPT_TEST_PCKBD */
}

void test_fslookup()
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

#ifdef OPT_TEST_ROOTFS
typedef void (*_start_t)(void);
_start_t _start;

void init_thread()
{
	_start();
	bug();
}
#endif

void test_rootfs()
{
#ifdef OPT_TEST_ROOTFS
	struct thread *thread;
	struct direntry *dent;
	struct inode *inode;
	char *dst = (char *)0x4000; /* _start() load address */
	int i;

	_start = (_start_t)dst;
	dent = lookup_path("/sbin/init");
	if (!dent) {
		kprintf("Init not found.\n");
		panic();
	}

	inode = dent->d_inode;
	i = memory_copy(dst, page_to_addr(inode->i_map.i_pages[1]), 4096);

        thread = thread_create_user(&init_thread, "init");
        if (!thread) {
                kprintf("failed to create thread\n");
		bug();
        }

        if (scheduler_enqueue(thread)) {
                kprintf("failed to add thread to run queue\n");
		bug();
        }
#endif
}

void test_bug()
{
#ifdef OPT_TEST_BUG
	bug();
#endif
}

void run_tests()
{
	test_mm();
	test_slice_alloc();
	test_klist();
	test_threads();
	test_pckbd();
	test_irqs();
	test_rootfs();
	test_fslookup();
	test_bug();
}

