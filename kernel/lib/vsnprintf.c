/*
 * kernel/lib/vsnprintf.c
 *
 * Copyright (C) 2005, 2006 Alexander Shishckin
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

#include <koowaldah.h>
#include <lib.h>
#include <stdarg.h>

/*
 * Various string-formatting functions
 */

static char *digits="0123456789ABCDEF";

static size_t __noprof sprint_int_signed(char *buf, long i, char base)
{
	int pos = 0;
	int n;
	long l = i;

	if (i < 0) {
		buf[pos++] = '-';
		i = 0 - i;
	}

	while (l / base) {
		l = l / base;
		pos++;
	}

	n = pos + 1;

	do {
		buf[pos--] = digits[i % base];
		i = i / base;
	} while (i);

	return n;
}

static size_t __noprof sprint_int_unsigned(char *buf, unsigned long i, char base)
{
	int pos = 0;
	int n;
	unsigned long l = i;

	while (l / base) {
		l = l / base;
		pos++;
	}

	n = pos + 1;

	do {
		buf[pos--] = digits[i % base];
		i = i / base;
	} while (i);

	return n;
}

int __noprof vsnprintf(char *buf, size_t size, const char *format, va_list args)
{
	/* format string length */
	int flen = kstrlen((char *)format) + 1;
	/* current position on the format string */
	char *cur = (char *)format;
	/* previous position on the format string */
	char *old = (char *)format;
	/* position in the output buffer */
	char *pos = buf;
	int fmt_broken = 0;
	/* %x08: 0 is the 'tag' and 8 is 'num' */
	char fill_tag = 0;
	int fill_num = 0;
	/* args */
	long a_x;
	/*char a_c;*/
	char sbuf[12]; /* <-- this is evil also */
	char *a_s;

	/* parse <format> */
	do {
		/* look for '%' sign, copy everything before that
		 * to output buffer */
		cur = kstrchr(cur, '%');
		if (NULL != cur)
			pos += memory_copy(pos, old, cur - old);

reswitch:
		/* check for EOLN or no-more-'%'-signs and skip the '%' sign
		 * in the format string */
		if (NULL == cur || '\0' == *cur++)
			break;

		/* now, the conversion specifiers */
		switch (*cur) {
			case '0'...'9':
				if (!fill_tag)
					fill_tag = *cur;
				else if (!fill_num)
					fill_num = *cur - '0';
				else
					fmt_broken++;
				goto reswitch;

			case 'd':
				memory_set(sbuf, 0, 12);
				a_x = va_arg(args, int);
				a_x = sprint_int_signed(sbuf, a_x, 10);
				pos += memory_copy(pos, sbuf, a_x);
				fill_tag = fill_num = 0;
				break;

			case 'l':
				memory_set(sbuf, 0, 12);
				a_x = va_arg(args, long);
				a_x = sprint_int_signed(sbuf, a_x, 10);
				pos += memory_copy(pos, sbuf, a_x);
				fill_tag = fill_num = 0;
				break;

			case 'x':
				memory_set(sbuf, 0, 12);
				a_x = va_arg(args, long);
				a_x = sprint_int_unsigned(sbuf, a_x, 16);

				/* pad output with fill_tag to fill_num
				 * characters */
				for (; fill_num - a_x > 0; fill_num--)
					*pos++ = fill_tag;

				pos += memory_copy(pos, sbuf, a_x);
				fill_tag = fill_num = 0;
				break;

#if 0
			case 'c':
				a_c = va_arg(args, char);
				*pos++ = a_c;
				break;
#endif

			case 's':
				a_s = va_arg(args, char *);
				if (!a_s || *a_s == '\0') {
					pos += memory_copy(pos, "<null>", 6);
					break;
				}

				pos += memory_copy(pos, a_s, kstrlen(a_s));
				fill_tag = fill_num = 0;
				break;

			default:
				fmt_broken++;
				fill_tag = fill_num = 0;
				/* fail silently */
				break;
		}
		old = cur + 1;
	} while (cur < format + flen);

	pos += memory_copy(pos, old, format + flen - old);

	return pos - buf;
}

int snprintf(char *buf, size_t size, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsnprintf(buf,size,fmt,args);
	va_end(args);
	return i;
}

