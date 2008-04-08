/*
 * kernel/init/test_threads.c
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

