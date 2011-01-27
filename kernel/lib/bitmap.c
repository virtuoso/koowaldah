
/*
 * bitmap.c
 *
 * Copyright (C) 2006 Alexey Zaytsev
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
#include <textio.h>
#include <bitmap.h>

/*
 * Find the first set bit.
 */
int bitmap_seek_set(unsigned long * dst, int limit)
{
	/* Later we may use some arch-specific instructions for
	 * finding set bits. */
	int bit = 0;
	int tmp = 0;

	for (bit = 0; bit < limit; bit++, tmp++) {
		if (tmp == BITS_PER_LONG) {
			tmp = 0;
			dst++;
		}
		if ((*dst) & (1UL << tmp))
			return bit;

	}
	return -1;
}

/*
 * Find the first unset bit.
 */
int bitmap_seek_unset(unsigned long * dst, int limit)
{
	int bit = 0;
	int tmp = 0;

	for (bit = 0; bit < limit; bit++, tmp++) {
		if (tmp == BITS_PER_LONG) {
			tmp = 0;
			dst++;
		}
		if (!((*dst) & (1UL << tmp)))
			return bit;

	}
	return -1;
}

/*
 * Check if all bits are set.
 */
int bitmap_all_set(unsigned long * dst, int limit)
{
	while (limit >= BITS_PER_LONG) {
		if (*dst++ != ~0UL) {
			return 0;
		}
		limit -= BITS_PER_LONG;
	}

	return limit ? !(~(*dst) & (~0UL >> (BITS_PER_LONG - limit))) : 1;
}


/*
 * Check if all bits are unset.
 */
int bitmap_all_unset(unsigned long * dst, int limit)
{
	while (limit >= BITS_PER_LONG) {
		if (*dst++)
			return 0;
		limit -= BITS_PER_LONG;
	}

	return limit ? !((*dst) & (~0UL >> (BITS_PER_LONG - limit))) : 1;
}

static __inline void bitmap_print_ulong(unsigned long data, int nbits)
{
	int i;
	for (i = 0; i < nbits; i++)
		kprintf("%d", (data & (1UL << i)) ? 1 : 0);
	kprintf(" ");
}

/*
 * Print the bitmap out.
 */
void bitmap_print(unsigned long *dst, int bits)
{
	while (bits >= BITS_PER_LONG){
		bitmap_print_ulong(*dst++, BITS_PER_LONG);
		bits -= BITS_PER_LONG;
	}
	bitmap_print_ulong(*dst, bits);
	kprintf("\n");
}

