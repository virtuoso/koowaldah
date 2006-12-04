
/*
 * tests/k-unit.c
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
