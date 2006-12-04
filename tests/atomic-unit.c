
/*
 * tests/atomic-unit.c
 *
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

/*
 * unit test for atomic operations
 * atomic.h
 *
 * interfaces description
 * + ATOMIC_INIT()
 * + atomic_read()
 * + atomic_set_u32
 *
 * TODO: complete
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include "k-unit.h"

#define __inline __attribute__((always_inline)) inline
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
#include "atomic.h"

atomic_u8  a8;
atomic_u16 a16;
atomic_u32 a32;

int main() {
	u32 i;

	TEST_DECLARE(1, "ATOMIC_INIT() + atomic_read()");
	ATOMIC_INIT(&a8);
	ATOMIC_INIT(&a16);
	ATOMIC_INIT(&a32);
	i = atomic_read(&a32);
	if (i) {
		printf("a32 == %u, should be 0\n", i);
		TEST_FAILURE(1);
	} else
		TEST_PASSED(1);
	separator();

	TEST_DECLARE(2, "atomic_set_u32()");
	atomic_set_u32(&a32, 5);
	i = atomic_read(&a32);
	if (i != 5) {
		printf("a32 == %u, should be 5\n", i);
		TEST_FAILURE(2);
	} else
		TEST_PASSED(2);
	separator();

	TEST_DECLARE(3, "atomic_dec_and_test_u32()");
	i = 5;
	while (!atomic_dec_and_test_u32(&a32)) --i;
	if (i != 1) {
		printf("dec and test failed, i=%d\n", i);
		TEST_FAILURE(3);
	} else
		TEST_PASSED(3);
	separator();


	printf("tests failed: %d, tests passed: %d\n",
			TESTS_FAILED, TESTS_PASSED);

	return TESTS_FAILED;
}

