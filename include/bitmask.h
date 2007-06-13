
/*
 * include/bitmask.h
 *
 * Copyright (C) 2006 Alexey Zaytsev
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
  * redudant bits ignored. The bits inside a single ulong are 
  * numbered starting from the least significant one.
  *
  * 0x00                       0x04                       0x08
  * | . | . | .... | . | . |   | . | . | .... | . | . |   | . | ...
  *  31  30         01  00      63  62         33  32      95
  *  
  *
  * This implementation is hidden from the user, and even bitmask_print
  * lists the bits from left to right. This may be a subject to change, so
  * don't touch the redudant bits.
  */

#ifndef __BITMASK_H__
#define __BITMASK_H__

#define BITS_PER_ULONG (sizeof(unsigned long) * 8)

#define BITS_TO_LONGS(nbits) 						\
	(((nbits + BITS_PER_ULONG - 1) & ~(BITS_PER_ULONG - 1))		\
		/ BITS_PER_ULONG)

#define BITS_TO_BYTES(bits) 						\
	(((bits + 7) & ~7) / 8)

#define BITMASK(name, nbits) 						\
	unsigned long name[BITS_TO_LONGS(nbits)]			\

/*
 * Clear a number of bits starting from zero.
 */
static __inline void bitmask_zero(unsigned long * dst, int bits) 
{
	while (bits >= BITS_PER_ULONG) {
		*dst++ = 0;
		bits -= BITS_PER_ULONG;
	}
	*dst = *dst & (~0UL << bits);
}

/*
 * Set a numer of bits starting from zero.
 */
static __inline void bitmask_fill(unsigned long * dst, int bits)
{
	while (bits >= BITS_PER_ULONG) {
		*dst++ = ~0UL;
		bits -= BITS_PER_ULONG;
	}
	*dst = *dst | (~0UL >> (bits - BITS_PER_ULONG));
}

/*
 * Set a single bit.
 */
static __inline void bitmask_bit_set(unsigned long * dst, int bit)
{
	dst[bit / BITS_PER_ULONG] = 
		dst[bit / BITS_PER_ULONG] | 1 << (bit % BITS_PER_ULONG);
}

/*
 * Clear a single bit.
 */
static __inline void bitmask_bit_clear(unsigned long * dst, int bit)
{
	dst[bit / BITS_PER_ULONG] =
		dst[bit / BITS_PER_ULONG] & ~(1 << (bit % BITS_PER_ULONG));
}

/*
 * Read a single bit.
 */
static __inline int bitmask_bit_get(unsigned long * dst, int bit)
{
	return dst[bit / BITS_PER_ULONG] & (1 << (bit % BITS_PER_ULONG));
}


int bitmask_seek_set(unsigned long * dst, int limit);
int bitmask_seek_unset(unsigned long * dst, int limit);
int bitmask_all_set(unsigned long * dst, int limit);
int bitmask_all_unset(unsigned long * dst, int limit);
void bitmask_print(unsigned long *dst, int bits);

#endif /* __BITMASK_H__ */
