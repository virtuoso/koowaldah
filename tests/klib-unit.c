
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
 * unit test for library macros
 * lib.h
 *
 * interfaces description
 * + strncmp
 * + __strcmp
 * + strcmp
 *
 * TODO: complete
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "k-unit.h"

#define __inline __attribute__((always_inline)) inline
#include "lib.h"

char *string = "this is the test string";
char *strings[] = {
	"this is the test STRING",
	"this is the test string fuckers",
	"this is the test string"
};

int main() {
	/* kstrcmp */
	TEST_DECLARE(1, "kstrcmp(): equal strings");
	if (kstrcmp(string, strings[2])) {
		printf("equal strings are not considered equal\n");
		TEST_FAILURE(1);
	} else
		TEST_PASSED(1);
	separator();

	TEST_DECLARE(2, "kstrcmp(): different length");
	if (kstrcmp(string, strings[1]) >= 0) {
		printf("inequal strings are considered equal\n");
		TEST_FAILURE(2);
	} else
		TEST_PASSED(2);
	separator();

	TEST_DECLARE(3, "kstrcmp(): just different");
	if (kstrcmp(string, strings[0]) <= 0) {
		printf("inequal strings are considered equal\n");
		TEST_FAILURE(3);
	} else
		TEST_PASSED(3);
	separator();

	/* k__strcmp */
	TEST_DECLARE(4, "k__strcmp(): equal strings");
	if (k__strcmp(string, strings[2])) {
		printf("equal strings are not considered equal\n");
		TEST_FAILURE(4);
	} else
		TEST_PASSED(4);
	separator();

	TEST_DECLARE(5, "k__strcmp(): different length");
	if (k__strcmp(string, strings[1]) >= 0) {
		printf("inequal strings are considered equal\n");
		TEST_FAILURE(5);
	} else
		TEST_PASSED(5);
	separator();

	TEST_DECLARE(6, "k__strcmp(): just different");
	if (k__strcmp(string, strings[0]) <= 0) {
		printf("inequal strings are considered equal\n");
		TEST_FAILURE(6);
	} else
		TEST_PASSED(6);
	separator();

	TEST_DECLARE(7, "kstrcmp(): corner case");
	if (!kstrcmp("init", "mnt")) {
		printf("kstrcmp() thinks that 'mnt' and 'init' are equal\n");
		TEST_FAILURE(7);
	} else
		TEST_PASSED(7);
	separator();

	TEST_DECLARE(8, "kstrlen()");
	if (strlen(string) != kstrlen(string)) {
		printf("strlen() disagrees with kstrlen()\n");
		TEST_FAILURE(8);
	} else
		TEST_PASSED(8);
	separator();

	printf("tests failed: %d, tests passed: %d\n",
			TESTS_FAILED, TESTS_PASSED);

	return TESTS_FAILED;
}

