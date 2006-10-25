/*
 * kernel/init.c
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

void kernel_main_thread();

/*
 * All the first stage init functions are extern here
 * since we don't want to expose those to the rest of
 * the kernel
 */
extern void trap_init(void);
extern void early_console_init(void);
extern void thread_init(void);
extern void timers_init(void);
extern void sched0_load(void);
extern void scheduler_init(void);
extern void fs_init(void);

extern void init_mem_info(void);
extern void slice_init(void);
extern void mm_init(void); /* legacy mm */ 

/* this is also needed only once */
extern void run_tests(void);
u8 kernel_started = 0;

void __init kern_start()
{
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

	init_mem_info();
	slice_init();

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

extern initfn late_init_start;
extern initfn late_init_end;

void __noprof call_late_init()
{
	initfn *fn;

	if (&late_init_start == &late_init_end)
		return;

	for (fn = &late_init_start; fn < &late_init_end; fn++)
		if ((*fn)() != 0)
			kprintf("Late-init function %x failed\n", *fn);
}

void __noprof kernel_main_thread()
{
	struct thread_t *me;

	me = CURRENT();
	
        if (scheduler_enqueue(me)) {
                kprintf("Failed to add main kernel thread to run queue\n");
		bug();
        }
        kprintf("Main kernel thread added to run queue.\n");

	kprintf("Initializing vfs core... ");
	fs_init();
	kprintf("Done.\n");

	scheduler_start();

        kprintf("Starting the Prigrammable Interval Timer...");
        timer_init();
        kprintf("Done\n");
	//kernel_started = 1;

	call_late_init();

	run_tests();

	for (;;)
		__asm__ __volatile__("sti; hlt");
}

