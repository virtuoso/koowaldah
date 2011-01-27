
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

