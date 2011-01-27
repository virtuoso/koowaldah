
/*
 * kernel/klib.c
 *
 * Copyright (C) 2005 Alexey Zaytsev
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
#include <lib.h>


size_t memory_set(void *s, char c, size_t n)
{

	size_t t = n;
	while (t-- > 0)
		((char *)s)[t] = (char)c;
	return n;
}

size_t memory_copy(void *dest, void *src, size_t n)
{
	size_t i;

	if (dest == src)
		return n;

	if (dest < src)
		for (i = 0; i < n; i++)
			((char *)dest)[i] = ((char *)src)[i];
	else
		for (i = n; i > 0; i--)
			((char *)dest)[i-1] = ((char *)src)[i-1];

	return n;
}

/* XxX: Whoever made it u32 deserves a slow death. Isn't the
 * thing called str-to-unsigned-long? */
/* nicked from NetBSD C library 'strtoull' */
u32 strtoul(char *nptr)
{
	char *s;
	u32 acc, cutoff;
	int c, base = 0;
	int neg, any, cutlim;

	s = nptr;
	do {
		c = (unsigned char) *s++;
	} while (isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else {
		neg = 0;
		if (c == '+')
			c = *s++;
	}
	if ((base == 0 || base == 16) &&
			c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;

	cutoff = ULONG_MAX / (u32)base;
	/* LONGLONG */
	cutlim = (int)(ULONG_MAX % (u32)base);
	for (acc = 0, any = 0;; c = (unsigned char) *s++) {
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0)
			continue;
		if (acc > cutoff || (acc == cutoff && c > cutlim)) {
			any = -1;
			acc = ULONG_MAX;
		} else {
			any = 1;
			acc *= (u32)base;
			acc += c;
		}
	}
	if (neg && any > 0)
		acc = -acc;

	return (acc);
}

