
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

	printf("KUCA KYKY\n");
	
	mm_init();

        printf("Setting isr table up...");
        isr_init();
        printf("Done\n");

        printf("Setting interrupts up...");
        interrupts_init();
	trap_init();
        printf("Done\n");

	thread_init();

	scheduler_init();
	
	main_thread = thread_create(&kernel_main_thread);
	if(!main_thread){
		printf("Failed to create main kernel thread\n");
		bug();
	}
	printf("Main kernel thread created\n");
	
	printf("Switching to the main kernel thread\n");
	thread_switch_to(main_thread);	

	bug();

}

void kernel_main_thread()
{

	struct thread_t * me;

#if TEST_THREADS
	struct thread_t * thread;
#endif

#if TEST_KLIST
        struct klist * list = NULL;
        struct klist * tmp;
#endif /* TEST_KLIST */

#if TEST_PAGE_ALLOC
        u32 * page1;
        u32 * page2;
#endif /* TEST_PAGE_ALLOC */


	me = thread_get_current();
	
        if(scheduler_add_to_queue(me)){
                printf("Failed to add main kernel thread to run queue\n");
		bug();
        }
        printf("Main kernel thread added to run queue.\n");

#if TEST_IRQS
        printf("Registering dummy keyboard interrupt service routine...");
        register_irq_handler(1, keyboard_irq_handler);
        printf("Done\n");
#endif /* TEST_IRQS */

#if TEST_PAGE_ALLOC
        page1 = pages_get(1);
        printf("Got a page, %x\n", page1);
        page2 = pages_get(1);
        printf("Got another page, %x\n", page2);

        pages_release(page1);

        printf("The first page released\n");

        pages_release(page2);

        printf("The second page released\n");

#endif /* TEST_THREADS */

#if TEST_KLIST
        printf("Testing the klist implementation\n");

        printf("klist_is_empty(&list) = %d\n", klist_is_empty(&list));

        printf("tmp = klist_new(), tmp->data = 1\n");
        tmp = klist_new();
        tmp->data = (void *)1;

        printf("klist_add(tmp, list)\n");
        klist_add(tmp, &list);

        printf("klist_is_empty(&list) = %d\n", klist_is_empty(&list));

        printf("Traversing the list\n");

        if(!klist_is_empty(&list)){
                tmp = list->next;
                do{
                        printf("got entry, list->data = %d\n", (u32) list->data);
                        klist_iter(&list);

                }while(tmp != list);
        }

        printf("Done with traversing\n");

        printf("adding 2 and 3\n");
        tmp = klist_new();
        tmp->data = (void *)2;
        klist_add(tmp, &list);
        tmp = klist_new();
        tmp->data = (void *)3;
        klist_add(tmp, &list);

        printf("Traversing the list\n");

        if(!klist_is_empty(&list)){
                tmp = list;
                do{
                        printf("got entry, list->data = %d\n", (u32) list->data);
                        klist_iter(&list);

                }while(tmp != list);
        }

        printf("Done with traversing\n");

#endif /* TEST_KLIST */

	scheduler_start();

#if TEST_THREADS
        thread = thread_create(&func1);
        if(!thread){
                printf("failed to create thread\n");
		bug();
        }
        printf("Thread A created.\n");

        if(scheduler_add_to_queue(thread)){
                printf("failed to add thread to run queue\n");
		bug();
        }
        printf("Thread A added to run queue.\n");


        thread = thread_create(&func2);
        if(!thread){
                printf("failed to create thread\n");
		bug();
        }
        printf("Thread B created.\n");

        if(scheduler_add_to_queue(thread)){
                printf("failed to add thread to run queue\n");
		bug();
        }
        printf("Thread B added to run queue.\n");

#endif /* TEST_THREADS */

//	thread_reschedule();

#if TEST_TIMER
        printf("Starting the Prigrammable Interval Timer...");
        timer_init();
        printf("Done\n");
#endif /* TEST_TIMER */


	for(;;) {
		__asm__ __volatile__("nop; nop; nop; nop");
		//printf("M");
	}

}


#if TEST_THREADS
void func1(){

//	enable_interrupts();
	int cnt = 0;
	for(;;){
		u32 i;

		if (cnt < 50) {
			printf("A");
			cnt++;
		}
		for(i = 10000; i > 0; i--);
		
//		thread_reschedule();
	}
	
//	for(;;)
//		asm volatile ("hlt");
}

void func2(){
	int cnt = 0;
//	enable_interrupts();
	for(;;){
		u32 i;
		
		if (cnt < 50) {
			printf("B");
			cnt++;
		}
		for(i = 10000; i > 0; i--);
//		thread_reschedule();
	}
	
}
#endif /* TEST_THREADS */

#if TEST_IRQS
extern u32 kernel_stack;
void keyboard_irq_handler(u32 number){
        unsigned char c;

        c = io_port_in(0x60);
        printf("Got a keyboard interrupt, scancode = %d\n", c);
	if (c == 32) {
		psod();
		bug();
	}
}
#endif /* TEST_IRQS */

