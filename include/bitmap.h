
/*
 * include/bitmap.h
 *
 * Copyright (C) 2006, 2008 Alexey Zaytsev
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
