/*
 * kernel/init/test_bug.c
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
#include <bug.h>
#include <textio.h>
#include <galloc.h>

u32 * tmp[2044];

void test_galloc()
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

