
/*
 * include/bitmap.h
 *
 * Copyright (C) 2006, 2008 Alexey Zaytsev
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

/*
 * Bitmasks are represented as arrays of type unsigned long, with
 * redundant bits ignored. The bits inside a single ulong are
 * numbered starting from the least significant one.
 *
 * 0x00                       0x04                       0x08
 * | . | . | .... | . | . |   | . | . | .... | . | . |   | . | ...
 *  31  30         01  00      63  62         33  32      95
 *
 *
 * This implementation is hidden from the user, and even bitmap_print
 * lists the bits from left to right. This may be a subject to change, so
 * don't touch the redundant bits.
 */

#ifndef __BITMAP_H__
#define __BITMAP_H__

#include <lib.h>

#define BITS_PER_LONG (sizeof(unsigned long) * 8)
#define LONG_MASK (BITS_PER_LONG - 1) /* Given long is a power of 2 */

/*
 * Round the number of bits up to the next long and divide by 8
 * to get the number of bytes
 */
#define BITMAP_SIZE(nbits) (((nbits + LONG_MASK) & ~LONG_MASK) / 8)

/* Define a static bit map. */
#define BITMAP(name, nbits) unsigned long name[BITMAP_SIZE(nbits)]

/*
 * Clear a number of bits starting from zero.
 */
static __inline void bitmap_zero(unsigned long *dst, int bits)
{
	if (bits / BITS_PER_LONG)
		/* Round up to ulong and zero out.*/
		memory_set(dst, 0, (bits & ~(BITS_PER_LONG - 1)) / 8);

	*dst &= (~0UL << (bits % BITS_PER_LONG));
}

/*
 * Set a numer of bits starting from zero.
 */
static __inline void bitmap_fill(unsigned long *dst, int bits)
{
	while (bits >= BITS_PER_LONG) {
		*dst++ = ~0UL;
		bits -= BITS_PER_LONG;
	}
	*dst |= (~0UL >> (bits - BITS_PER_LONG));
}

/*
 * Set a single bit.
 */
static __inline void bitmap_bit_set(unsigned long *dst, int bit)
{
	dst[bit / BITS_PER_LONG] |= (1UL << (bit % BITS_PER_LONG));
}

/*
 * Clear a single bit.
 */
static __inline void bitmap_bit_clear(unsigned long *dst, int bit)
{
	dst[bit / BITS_PER_LONG] &= ~(1UL << (bit % BITS_PER_LONG));
}

/*
 * Read a single bit.
 */
static __inline int bitmap_bit_get(unsigned long *dst, int bit)
{
	return dst[bit / BITS_PER_LONG] & (1UL << (bit % BITS_PER_LONG));
}

int bitmap_seek_set(unsigned long * dst, int limit);
int bitmap_seek_unset(unsigned long * dst, int limit);
int bitmap_all_set(unsigned long * dst, int limit);
int bitmap_all_unset(unsigned long * dst, int limit);
void bitmap_print(unsigned long *dst, int bits);

#endif /* __BITMAP_H__ */
