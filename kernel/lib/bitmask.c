
/*
 * kernel/bitmask.c
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
 * 3. Neither the name of the Koowaldah developers nor the names of theyr
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

#include <koowaldah.h>
#include <textio.h>
#include <bitmask.h>

/*
 * Find the first set bit.
 */
int bitmask_seek_set(unsigned long * dst, int limit)
{
	/* Later we may use some arch-specific instructions for
	 * finding set bits. */
	int bit = 0;
	int tmp = 0;

	for (bit = 0; bit < limit; bit++, tmp++) {
		if (tmp == BITS_PER_ULONG) {
			tmp = 0;
			dst++;
		}
		if ((*dst) & (1 << tmp))
			return bit;

	}
	return -1;
}

/*
 * Find the first unset bit.
 */
int bitmask_seek_unset(unsigned long * dst, int limit)
{
	int bit = 0;
	int tmp = 0;

	for (bit = 0; bit < limit; bit++, tmp++) {
		if (tmp == BITS_PER_ULONG) {
			tmp = 0;
			dst++;
		}
		if (!((*dst) & (1 << tmp)))
			return bit;

	}
	return -1;
}

/*
 * Check if all bits are set.
 */
int bitmask_all_set(unsigned long * dst, int limit)
{
	while (limit >= BITS_PER_ULONG) {
		if (*dst++ != ~0UL) {
			return 0;
		}
		limit -= BITS_PER_ULONG;
	}

	return limit ? (!(~(*dst) & (~0UL >> (BITS_PER_ULONG - limit)))) : 1;
}


/*
 * Check if all bits are unset.
 */
int bitmask_all_unset(unsigned long * dst, int limit)
{
	while (limit >= BITS_PER_ULONG) {
		if (*dst++)
			return 0;
		limit -= BITS_PER_ULONG;
	}

	return limit ? (!((*dst) & (~0UL >> (BITS_PER_ULONG - limit)))) : 1;
}

static inline void bitmask_print_ulong(unsigned long data, int nbits)
{
	int i;
	for (i = 0; i < nbits; i++)
		kprintf("%d", (data & (1 << i )) ? 1 : 0);
	kprintf(" ");
}

/*
 * Print the bitmask out.
 */
void bitmask_print(unsigned long *dst, int bits)
{
	while (bits >= BITS_PER_ULONG){
		bitmask_print_ulong(*dst++, BITS_PER_ULONG);
		bits -= BITS_PER_ULONG;
	}
	bitmask_print_ulong(*dst, bits);

	kprintf("\n");
}
