/*
 * unit test for linked list implementation
 * klist0.h
 *
 * interfaces description
 * + klist0_prepend
 * + klist0_append
 * + klist0_unlink
 * + klist0_entry
 *
 * P.S. We'll turn this into a testing framework one day.
 */
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include "klist0.h"

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

#define dump_node(name, node) \
	do { \
		printf("%s: %p, %s->prev: %p, %s->next: %p\n", \
			name, &node, name, node.prev, name, node.next); \
	} while (0);
#define separator() \
	printf("------------------------\n");
/* ------------------------------------------ */
/* end of generic test stuff                  */
/* ------------------------------------------ */

#define MAX_NODES 4
typedef struct list_test {
	struct klist0_node the_list;
	int data;
} list_test_struct;
#define TEST_KEY 1234

struct list_test _test;
struct list_test _node[MAX_NODES];

int main() {
	int i;
	struct klist0_node *t;

	/* test 1: klist0_append() */
	TEST_DECLARE(1, "klist0_append()");
	KLIST0_INIT(&_test.the_list);
	_test.data = TEST_KEY;
	for (i = 0; i < MAX_NODES; i++) {
		KLIST0_INIT(&_node[i].the_list);
		_node[i].data = i;
	}

	klist0_append(&_node[1].the_list, &_node[0].the_list);
	klist0_append(&_node[2].the_list, &_node[1].the_list);
	dump_node("_node[0]", _node[0].the_list);
	dump_node("_node[1]", _node[1].the_list);
	dump_node("_node[2]", _node[2].the_list);
	if (_node[0].the_list.next != &_node[1].the_list ||
	    _node[1].the_list.next != &_node[2].the_list ||
	    _node[2].the_list.next != &_node[0].the_list ||
	    _node[0].the_list.prev != &_node[2].the_list ||
	    _node[1].the_list.prev != &_node[0].the_list ||
	    _node[2].the_list.prev != &_node[1].the_list)
		TEST_FAILURE(1);
	else
		TEST_PASSED(1);
	separator();

	/* test 2: klist0_prepend() */
	TEST_DECLARE(2, "klist0_prepend()");
	klist0_prepend(&_test.the_list, &_node[1].the_list);
	dump_node("_test", _test.the_list);
	dump_node("_node[0]", _node[0].the_list);
	dump_node("_node[1]", _node[1].the_list);
	dump_node("_node[2]", _node[2].the_list);
	if (_node[0].the_list.next != &_test.the_list ||
	    _node[1].the_list.next != &_node[2].the_list ||
	    _test.the_list.next != &_node[1].the_list ||
	    _test.the_list.prev != &_node[0].the_list)
		TEST_FAILURE(2);
	else
		TEST_PASSED(2);
	separator();

	/* test 3: klist0_unlink() */
	TEST_DECLARE(3, "klist0_unlink()");
	klist0_unlink(&_test.the_list);
	dump_node("_test", _test.the_list);
	dump_node("_node[0]", _node[0].the_list);
	dump_node("_node[1]", _node[1].the_list);
	dump_node("_node[2]", _node[2].the_list);
	if (_node[0].the_list.next != &_node[1].the_list ||
	    _node[1].the_list.next != &_node[2].the_list ||
	    _node[0].the_list.prev != &_node[2].the_list ||
	    _node[1].the_list.prev != &_node[0].the_list ||
	    _test.the_list.next != &_test.the_list ||
	    _test.the_list.prev != &_test.the_list)
		TEST_FAILURE(3);
	else
		TEST_PASSED(3);
	separator();

	/* test 4: klist0_entry() */
	TEST_DECLARE(4, "klist0_entry()");
	printf("_test->data=%d\n",
			klist0_entry(&_test.the_list,
				list_test_struct,
				the_list)->data);
	if (klist0_entry(&_test.the_list, list_test_struct, the_list)->data !=
			TEST_KEY)
		TEST_FAILURE(4);
	else
		TEST_PASSED(4);
	separator();

	/* test 5: klist0_for_each() */
	TEST_DECLARE(5, "klist0_for_each()");
	i = 1;
	klist0_for_each(t, &_node[0].the_list) {
		list_test_struct *l = klist0_entry(t, list_test_struct, the_list);
		dump_node("t", l->the_list);
		printf("   data=%d\n", l->data);
		if (l->data != i++)
			TEST_FAILURE(5);
	}
	if (i = 3)
		TEST_PASSED(5);
	separator();

	printf("tests failed: %d, tests passed: %d\n",
			TESTS_FAILED, TESTS_PASSED);

	return TESTS_FAILED;
}

