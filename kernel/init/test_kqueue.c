/*
 * kernel/init/test_kqueue.c
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

