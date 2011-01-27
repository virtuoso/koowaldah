
/*
 * tests/atomic-unit.c
 *
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

