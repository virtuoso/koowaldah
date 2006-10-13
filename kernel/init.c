
/*
 * kernel/init.c
 *
 * Copyright (C) 2005 Alexey Zaytsev
 * 
 * Is distributed under the GPL 2 license.
 * 
 */

#define TEST_IRQS 1
#define TEST_TIMER 1
#define TEST_THREADS 1
#define TEST_PAGE_ALLOC 0
#define TEST_KLIST 0

#include <kuca.h>
#include <console.h>
#include <mm.h>
#include <arch/asm.h>
#include <arch/isr.h>
#include <irq.h>
#include <timer.h>
#include <textio.h>
#include <klist.h>
#include <thread.h>
#include <scheduler.h>
#include <bug.h>

void keyboard_irq_handler(u32 number);

void func1();
void func2();

void kernel_main_thread();

extern void trap_init(void);

void __init kern_start(){

	struct thread_t * main_thread;

	early_console_init();

	kprintf("KUCA KYKY\n");
	
	mm_init();

        kprintf("Setting isr table up...");
        isr_init();
        kprintf("Done\n");

        kprintf("Setting interrupts up...");
        interrupts_init();
	trap_init();
        kprintf("Done\n");

	thread_init();

	timers_init();

	sched0_load();
	scheduler_init();
	
	main_thread = thread_create(&kernel_main_thread, "GOD");
	if(!main_thread){
		kprintf("Failed to create main kernel thread\n");
		bug();
	}
	kprintf("Main kernel thread created\n");
	
	kprintf("Switching to the main kernel thread\n");
	thread_switch_to(main_thread);	

	bug();

}

void test_irqs();

void kernel_main_thread()
{

	struct thread_t * me;
	struct thread_t * thread;


	me = CURRENT();
	/* init this thread 
	init_thread_struct(me); */
	
        if(scheduler_enqueue(me)){
                kprintf("Failed to add main kernel thread to run queue\n");
		bug();
        }
        kprintf("Main kernel thread added to run queue.\n");


	scheduler_start();

        thread = thread_create(&func1, "[thread A]");
        if(!thread){
                kprintf("failed to create thread\n");
		bug();
        }
        kprintf("Thread A created.\n");

        if(scheduler_enqueue(thread)){
                kprintf("failed to add thread to run queue\n");
		bug();
        }
        kprintf("Thread A added to run queue.\n");


        thread = thread_create(&func2, "[thread B]");
        if(!thread){
                kprintf("failed to create thread\n");
		bug();
        }
        kprintf("Thread B created.\n");

        if(scheduler_enqueue(thread)){
                kprintf("failed to add thread to run queue\n");
		bug();
        }
        kprintf("Thread B added to run queue.\n");

	test_irqs();

        kprintf("Starting the Prigrammable Interval Timer...");
        timer_init();
        kprintf("Done\n");

	for(;;)
		__asm__ __volatile__("sti; hlt");
}

void test_irqs()
{
#if TEST_IRQS
        kprintf("Registering dummy keyboard interrupt service routine...");
        register_irq_handler(1, keyboard_irq_handler);
        kprintf("Done\n");
#endif /* TEST_IRQS */
}

void test_page_alloc()
{
#if TEST_PAGE_ALLOC
        u32 * page1;
        u32 * page2;

        page1 = pages_get(1);
        kprintf("Got a page, %x\n", page1);
        page2 = pages_get(1);
        kprintf("Got another page, %x\n", page2);

        pages_release(page1);

        kprintf("The first page released\n");

        pages_release(page2);

        kprintf("The second page released\n");

#endif /* TEST_THREADS */
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

        if(!klist_is_empty(&list)){
                tmp = list->next;
                do{
                        kprintf("got entry, list->data = %d\n", (u32) list->data);
                        klist_iter(&list);

                }while(tmp != list);
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

        if(!klist_is_empty(&list)){
                tmp = list;
                do{
                        kprintf("got entry, list->data = %d\n", (u32) list->data);
                        klist_iter(&list);

                }while(tmp != list);
        }

        kprintf("Done with traversing\n");

#endif /* TEST_KLIST */
}

#if TEST_THREADS
extern int tsleep(u32 delay);
void func1()
{
	for(;;) {
		kprintf("A");
		tsleep(50);
	}
}

void func2(){
	for(;;) {
		kprintf("B");
		tsleep(70);
	}
	
}
#endif /* TEST_THREADS */

#if TEST_IRQS
extern u64 jiffies;
void keyboard_irq_handler(u32 number){
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

