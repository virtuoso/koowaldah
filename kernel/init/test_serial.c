/*
 * kernel/init/test_serial.c
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
#include <bug.h>
#include <thread.h>
#include <scheduler.h>

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

void test_serial()
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
