/*
 * K-Unit is a small framework for unit-testing freestanding components
 * of the kernel
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
