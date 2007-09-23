
/*
 * include/lib.h
 *
 * Copyright (C) 2005 Alexey Zaytsev
 * Copyright (C) 2006 Alexander Shishkin
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
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
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

