
/*
 * tests/k-unit.c
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
 * K-Unit is a small framework for unit-testing freestanding components
 * of the kernel
 *
 */

#ifndef __K_UNIT_H__
#define __K_UNIT_H__

/* ------------------------------------------ */
/* generic test stuff                         */
/* ------------------------------------------ */
static unsigned int TESTS_FAILED = 0;
static unsigned int TESTS_PASSED = 0;
#define TEST_FAILURE(n) \
	do { \
		TESTS_FAILED++; \
		printf("TEST %d failed\n", n); \
	} while (0)

#define TEST_PASSED(n) \
	do { \
		TESTS_PASSED++; \
		printf("TEST %d passed\n", n); \
	} while (0)

#define TEST_DECLARE(n, title) \
	printf("TEST %d, \"%s\"\n", n, title)

#define separator() \
	printf("------------------------\n");

/* ------------------------------------------ */
/* end of generic test stuff                  */
/* ------------------------------------------ */

#endif
