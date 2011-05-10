
/*
 * include/lib.h
 *
 * Copyright (C) 2005 Alexey Zaytsev
 * Copyright (C) 2006 Alexander Shishkin
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

#ifndef __LIB_H__
#define __LIB_H__

#include <stdarg.h>

size_t memory_set(void *s, char c, size_t n);
size_t memory_copy(void *dest, void *src, size_t n);

#define isdigit(__c) (__c >= '0' && __c <= '9')
#define isupper(__c) (__c >= 'A' && __c <= 'Z')
#define islower(__c) (__c >= 'a' && __c <= 'a')
#define isalpha(__c) (isupper(__c) || islower(__c))
#define isspace(__c) (__c == ' ' || __c == '\t' || __c == '\n' || __c == '\r')

u32 strtoul(char *nptr);

#define NMASK(x) ((1 << (x)) - 1)
static __inline unsigned long log2(unsigned long n)
{
	u8 l = 1;
	while ((l < 32) && (n >> l)) l++;
	return n & NMASK(l - 1) ? l : l - 1;
}

/*
 * Round up x by base; base should be a power of 2
 */
#define roundup_l2(x, base)			\
	(((x) + ((base) - 1)) & ~((base) - 1))

static __inline char *kstrchr(char *str, char c)
{
	char *p = str;

	while ('\0' != *p && c != *p) p++;
	if ('\0' == *p) return NULL;

	return p;
}

static __inline size_t kstrlen(const char *str)
{
	const char *p = str;

	while ('\0' != *(++p));
	return p - str;
}
#define kstrlen_(__s) (kstrlen(__s) + 1)

static __inline char *kstrrchr(char *str, char c)
{
	char *p = str + kstrlen(str);

	while (str != p && c != *p) p--;
	if (c != *p) return NULL;

	return p;
}

static __inline int kstrncmp(char *a, char *b, size_t n)
{
	while (n-- && *a == *b) a++, b++;
	return (*a - *b);
}

static __inline int k__strcmp(char *a, char *b)
{
	while (*a && *b && *a == *b) a++, b++;
	return (*a - *b);
}

static __inline int kstrcmp(char *a, char *b)
{
	int len_a = kstrlen(a);
	int len_b = kstrlen(b);

	if (len_a == len_b)
		return kstrncmp(a, b, len_a);

	return k__strcmp(a, b);
}

int vsnprintf(char *buf, size_t size, const char *format, va_list args);
int snprintf(char *buf, size_t size, const char *fmt, ...);

#endif /* __LIB_H__ */

