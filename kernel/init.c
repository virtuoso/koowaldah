/*
 * kernel/init.c
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
#include <console.h>
#include <page_alloc.h>
#include <mm_zone.h>
#include <arch/asm.h>
#include <irq.h>
#include <textio.h>
#include <lib.h>
#include <kqueue.h>
#include <thread.h>
#include <scheduler.h>
#include <bug.h>

void kernel_main_thread();

/*
 * All the first stage init functions are extern here
 * since we don't want to expose those to the rest of
 * the kernel
 */
void timers_init(void);
void sched0_load(void);
void scheduler_init(void);
void fs_init(void);

void init_mem_info(void);
void galloc_init(void);
void paging_init(void);
void kqueue_init(void);

/* this is also needed only once */
void run_tests(void);

void mach_start();
void mach_running();

void __init kern_start()
{
	struct thread *main_thread;

	mach_start();

	kprintf("Starting Koowaldah v%s kernel.\n", VERSION_STRING);

	/* memory allocation machinery: zones, buddy allocator, slices */
	init_mem_info();
	paging_init();
	galloc_init();

	kqueue_init();

	timers_init();

	sched0_load();
	scheduler_init();
	main_thread = thread_create(&kernel_main_thread, "GOD", NULL);
	if (!main_thread) {
		kprintf("Failed to create main kernel thread\n");
		bug();
	}
	thread_switch_to(main_thread);

	bug();
}

#ifndef OPT_CPU_ARCH_DUMMY
extern initfn late_init_start;
extern initfn late_init_end;
#else
extern initfn __start_InitLate;
extern initfn __stop_InitLate;
#define late_init_start __start_InitLate
#define late_init_end   __stop_InitLate
#endif

void __noprof call_late_init()
{
	initfn *fn;

	kprintf("Late init.\n");
	for (fn = &late_init_start; fn < &late_init_end; fn++)
		if ((*fn)() != 0)
			kprintf("Late-init function %x failed\n", *fn);
}

static void load_init()
{
#ifndef OPT_DONT_CALL_INIT
	int s = spawn("/sbin/init");
	if (s)  spawn("/bin/init");
	if (s)  spawn("/kosrc");
	if (s) {
		kprintf("Failed to execute init. Bye-bye!\n");
		panic();
	}
#endif
}

void __noprof kernel_main_thread(void *data)
{
	struct thread *me = CURRENT();
	struct thread_queue tmp_q;

	tq_init(&tmp_q);
	tq_insert_head(me, &tmp_q);

	bug_on(!scheduler_enqueue(&tmp_q));

	mach_running();

	kprintf("Initializing vfs core... ");
	fs_init();
	kprintf("Done.\n");

	call_late_init();

	run_tests();

	load_init();

	for (;;) {
		scheduler_yield();
		arch_idle();
	}

	bug();
}

