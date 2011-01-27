/*
 * include/dummy/atomic.h
 *
 * Copyright (C) 2007 Alexander Shishkin
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

#ifndef __DUMMY_ATOMIC_H__
#define __DUMMY_ATOMIC_H__

/* TODO: synchronization */
#define ATOMIC_PROLOGUE() do {} while (0)
#define ATOMIC_EPILOGUE() do {} while (0)

#define ATOMIC_SET(TYPE)                              \
	static __inline void                          \
	atomic_set_##TYPE(atomic_##TYPE *p, TYPE v)   \
	{                                             \
		ATOMIC_PROLOGUE();                    \
		p->v |= v;                            \
		ATOMIC_EPILOGUE();                    \
	}

#define ATOMIC_CLEAR(TYPE)                            \
	static __inline void                          \
	atomic_clear_##TYPE(atomic_##TYPE *p, TYPE v) \
	{                                             \
		ATOMIC_PROLOGUE();                    \
		p->v &= ~v;                           \
		ATOMIC_EPILOGUE();                    \
	}

#define ATOMIC_ADD(TYPE)                              \
	static __inline void                          \
	atomic_add_##TYPE(atomic_##TYPE *p, TYPE v)   \
	{                                             \
		ATOMIC_PROLOGUE();                    \
		p->v += v;                            \
		ATOMIC_EPILOGUE();                    \
	}

#define ATOMIC_SUB(TYPE)                              \
	static __inline void                          \
	atomic_sub_##TYPE(atomic_##TYPE *p, TYPE v)   \
	{                                             \
		ATOMIC_PROLOGUE();                    \
		p->v -= v;                            \
		ATOMIC_EPILOGUE();                    \
	}

#define ATOMIC_SUB_AND_TEST(TYPE)                              \
	static __inline u8                                     \
	atomic_sub_and_test_##TYPE(atomic_##TYPE *p, TYPE v)   \
	{                                                      \
		u8 test;                                       \
		ATOMIC_PROLOGUE();                             \
		p->v -= v;                                     \
		test = !p->v;                                  \
		ATOMIC_EPILOGUE();                             \
		return test;                                   \
	}

#define ATOMIC_TEST_AND_SET(TYPE)                              \
	static __inline TYPE                                   \
	atomic_test_and_set_##TYPE(atomic_##TYPE *p, TYPE v)   \
	{                                                      \
		TYPE spare;                                    \
		ATOMIC_PROLOGUE();                             \
		spare = p->v; p->v = v; v = spare;             \
		ATOMIC_EPILOGUE();                             \
		return spare;                                  \
	}

ATOMIC_ADD(u8)
ATOMIC_SUB(u8)
ATOMIC_SET(u8)
ATOMIC_CLEAR(u8)
ATOMIC_SUB_AND_TEST(u8)
ATOMIC_TEST_AND_SET(u8)
#define atomic_inc_u8(p) atomic_add_u8(p, 1)
#define atomic_dec_u8(p) atomic_add_u8(p, 1)
#define atomic_dec_and_test_u8(p) atomic_sub_and_test_u8(p, 1)

ATOMIC_ADD(u16)
ATOMIC_SUB(u16)
ATOMIC_SET(u16)
ATOMIC_CLEAR(u16)
ATOMIC_SUB_AND_TEST(u16)
ATOMIC_TEST_AND_SET(u16)
#define atomic_inc_u16(p) atomic_add_u16(p, 1)
#define atomic_dec_u16(p) atomic_add_u16(p, 1)
#define atomic_dec_and_test_u16(p) atomic_sub_and_test_u16(p, 1)

ATOMIC_ADD(u32)
ATOMIC_SUB(u32)
ATOMIC_SET(u32)
ATOMIC_CLEAR(u32)
ATOMIC_SUB_AND_TEST(u32)
ATOMIC_TEST_AND_SET(u32)
#define atomic_inc_u32(p) atomic_add_u32(p, 1)
#define atomic_dec_u32(p) atomic_add_u32(p, 1)
#define atomic_dec_and_test_u32(p) atomic_sub_and_test_u32(p, 1)

#define __ARCH_ATOMIC

#endif

