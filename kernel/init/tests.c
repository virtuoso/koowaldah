/*
 * kernel/init/tests.c
 *
 * Copyright (C) 2006 Alexey Zaytsev
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

#include <koowaldah.h>

void test_mm(void);
void test_slice_alloc(void);
void test_galloc(void);
void test_kqueue(void);
void test_threads(void);
void test_pckbd(void);
void test_serial(void);
void test_irqs(void);
void test_rootfs(void);
void test_fslookup(void);
void test_bug(void);
void test_panic(void);
void test_pf(void);

void run_tests()
{
	test_mm();
	test_slice_alloc();
	test_galloc();
	test_kqueue();
	test_threads();
	test_pckbd();
	test_serial();
	test_irqs();
	test_rootfs();
	test_fslookup();
	test_bug();
	test_panic();
	test_pf();
}

