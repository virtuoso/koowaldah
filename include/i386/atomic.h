
/*
 * include/i386/atomic.h
 *
 * Copyright (c) 2006, Alexander Shishkin
 * All rights reserved.
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
 */

#ifndef __I386_ATOMIC_H__
#define __I386_ATOMIC_H__

/*
 * i386 atomic operations.
 * Partially derived from FreeBSD and NetBSD respective implementations
 * of those.
 */

#define ATOMIC_ASM(NAME, TYPE, OP, V) \
	static __inline void \
	atomic_##NAME##_##TYPE(atomic_##TYPE *p, TYPE v) \
	{ \
		__asm __volatile(OP \
				 : "=m" (p->v) \
				 : "ir" (V)); \
	}

#define ATOMIC_SUB_AND_TEST_ASM(TYPE) \
	static __inline u8 \
	atomic_sub_and_test_##TYPE(atomic_##TYPE *p, int v) \
	{ \
		u8 test; \
		__asm__ __volatile__("subl %2, %0; sete %1" \
				: "=m"(p->v), "=qm"(test) \
				: "ir"(v), "m"(p->v) : "memory"); \
		return test; \
	}

#define ATOMIC_TEST_AND_SET(TYPE) \
	static __inline TYPE \
	atomic_test_and_set_##TYPE(atomic_##TYPE *p, volatile TYPE v) \
	{ \
		/*volatile TYPE t = v;*/ \
		__asm__ __volatile__("xchgl %1, %0" \
				: "=m"(p->v), "=r"(v) \
				: "m"(p->v), "r"(v) : "memory"); \
		return v; \
	}

/* u8 operations */
ATOMIC_ASM(set,	     u8,   "orb %1,%0",   v)
ATOMIC_ASM(clear,    u8,   "andb %1,%0", ~v)
ATOMIC_ASM(add,	     u8,   "addb %1,%0",  v)
ATOMIC_ASM(sub,      u8,   "subb %1,%0",  v)
ATOMIC_SUB_AND_TEST_ASM(u8)
ATOMIC_TEST_AND_SET(u8)
#define atomic_inc_u8(p) atomic_add_u8(p, 1)
#define atomic_dec_u8(p) atomic_sub_u8(p, 1)
#define atomic_dec_and_test_u8(p) atomic_sub_and_test_u8(p, 1)

/* u16 operations */
ATOMIC_ASM(set,	     u16,   "orw %1,%0",   v)
ATOMIC_ASM(clear,    u16,   "andw %1,%0", ~v)
ATOMIC_ASM(add,	     u16,   "addw %1,%0",  v)
ATOMIC_ASM(sub,      u16,   "subw %1,%0",  v)
ATOMIC_SUB_AND_TEST_ASM(u16)
ATOMIC_TEST_AND_SET(u16)
#define atomic_inc_u16(p) atomic_add_u16(p, 1)
#define atomic_dec_u16(p) atomic_sub_u16(p, 1)
#define atomic_dec_and_test_u16(p) atomic_sub_and_test_u16(p, 1)

/* u32 operations */
ATOMIC_ASM(set,	     u32,   "orl %1,%0",   v)
ATOMIC_ASM(clear,    u32,   "andl %1,%0", ~v)
ATOMIC_ASM(add,	     u32,   "addl %1,%0",  v)
ATOMIC_ASM(sub,      u32,   "subl %1,%0",  v)
ATOMIC_SUB_AND_TEST_ASM(u32)
ATOMIC_TEST_AND_SET(u32)
#define atomic_inc_u32(p) atomic_add_u32(p, 1)
#define atomic_dec_u32(p) atomic_sub_u32(p, 1)
#define atomic_dec_and_test_u32(p) atomic_sub_and_test_u32(p, 1)

#define __ARCH_ATOMIC

#endif
