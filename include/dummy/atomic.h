/*
 * include/dummy/atomic.h
 *
 * Copyright (C) 2007 Alexander Shishkin
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
 * THIS SOFTWARE IS PROVIDED BY THE DEVELOPERS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE DEVELOPERS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
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


ATOMIC_ADD(u8)
ATOMIC_SUB(u8)
ATOMIC_SET(u8)
ATOMIC_CLEAR(u8)
ATOMIC_SUB_AND_TEST(u8)
#define atomic_inc_u8(p) atomic_add_u8(p, 1)
#define atomic_dec_u8(p) atomic_add_u8(p, 1)
#define atomic_dec_and_test_u8(p) atomic_sub_and_test_u8(p, 1)

ATOMIC_ADD(u16)
ATOMIC_SUB(u16)
ATOMIC_SET(u16)
ATOMIC_CLEAR(u16)
ATOMIC_SUB_AND_TEST(u16)
#define atomic_inc_u16(p) atomic_add_u16(p, 1)
#define atomic_dec_u16(p) atomic_add_u16(p, 1)
#define atomic_dec_and_test_u16(p) atomic_sub_and_test_u16(p, 1)

ATOMIC_ADD(u32)
ATOMIC_SUB(u32)
ATOMIC_SET(u32)
ATOMIC_CLEAR(u32)
ATOMIC_SUB_AND_TEST(u32)
#define atomic_inc_u32(p) atomic_add_u32(p, 1)
#define atomic_dec_u32(p) atomic_add_u32(p, 1)
#define atomic_dec_and_test_u32(p) atomic_sub_and_test_u32(p, 1)

#define __ARCH_ATOMIC

#endif

