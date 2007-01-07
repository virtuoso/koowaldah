
/*
 * include/i386/atomic.h
 *
 * Copyright (c) 2006, Alexander Shishkin
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

/* u8 operations */
ATOMIC_ASM(set,	     u8,   "orb %1,%0",   v)
ATOMIC_ASM(clear,    u8,   "andb %1,%0", ~v)
ATOMIC_ASM(add,	     u8,   "addb %1,%0",  v)
ATOMIC_ASM(sub,      u8,   "subb %1,%0",  v)
ATOMIC_SUB_AND_TEST_ASM(u8)
#define atomic_inc_u8(p) atomic_add_u8(p, 1)
#define atomic_dec_u8(p) atomic_sub_u8(p, 1)
#define atomic_dec_and_test_u8(p) atomic_sub_and_test_u8(p, 1)

/* u16 operations */
ATOMIC_ASM(set,	     u16,   "orw %1,%0",   v)
ATOMIC_ASM(clear,    u16,   "andw %1,%0", ~v)
ATOMIC_ASM(add,	     u16,   "addw %1,%0",  v)
ATOMIC_ASM(sub,      u16,   "subw %1,%0",  v)
ATOMIC_SUB_AND_TEST_ASM(u16)
#define atomic_inc_u16(p) atomic_add_u16(p, 1)
#define atomic_dec_u16(p) atomic_sub_u16(p, 1)
#define atomic_dec_and_test_u16(p) atomic_sub_and_test_u16(p, 1)

/* u32 operations */
ATOMIC_ASM(set,	     u32,   "orl %1,%0",   v)
ATOMIC_ASM(clear,    u32,   "andl %1,%0", ~v)
ATOMIC_ASM(add,	     u32,   "addl %1,%0",  v)
ATOMIC_ASM(sub,      u32,   "subl %1,%0",  v)
ATOMIC_SUB_AND_TEST_ASM(u32)
#define atomic_inc_u32(p) atomic_add_u32(p, 1)
#define atomic_dec_u32(p) atomic_sub_u32(p, 1)
#define atomic_dec_and_test_u32(p) atomic_sub_and_test_u32(p, 1)

#define __ARCH_ATOMIC

#endif
