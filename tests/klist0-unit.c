
/*
 * tests/klist0-unit.c
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
 * unit test for linked list implementation
 * klist0.h
 *
 * interfaces description
 * + klist0_prepend
 * + klist0_append
 * + klist0_unlink
 * + klist0_entry
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include "k-unit.h"
#include "klist0.h"

#define dump_node(name, node) \
	do { \
		printf("%s: %p, %s->prev: %p, %s->next: %p\n", \
			name, &node, name, node.prev, name, node.next); \
	} while (0);

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

