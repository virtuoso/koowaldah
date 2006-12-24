
/*
 * kernel/textio.c
 *
 * Copyright (C) 2005 Alexander Shishckin
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
#include <textio.h>
#include <lib.h>
#include <console.h>

static void __noprof print(const char *message)
{
	while (*message)
		console_put_char(*(message++));
}

void __noprof hex_dump(char * ptr, u32 len)
{
	int i;
	for (i = 0; i < len; i++) {
		if (!(i % 16)) { /*0, 16, 32...*/
			/* I wish we had a working %N.Mx in kprintf... */
			kprintf("0x%x%x%x%x%x%x%x%x:",
				((i + (u32)ptr) & 0xf0000000) >> 28,
				((i + (u32)ptr) & 0x0f000000) >> 24,
				((i + (u32)ptr) & 0x00f00000) >> 20,
				((i + (u32)ptr) & 0x000f0000) >> 16,
				((i + (u32)ptr) & 0x0000f000) >> 12,
				((i + (u32)ptr) & 0x00000f00) >> 8,
				((i + (u32)ptr) & 0x000000f0) >> 4,
				((i + (u32)ptr) & 0x0000000f));
		}
		
		if (!(i % 4))  /* 0, 4, 8, 12...*/
			kprintf("  %x", ptr[i] & 0xff);
		else if (!((i+ 1) % 16)) /* 15, 31, 47...*/
			kprintf(" %x\n", ptr[i] & 0xff);
		else
			kprintf(" %x", ptr[i] & 0xff);
	}
	kprintf("\n");
}

int __noprof kprintf(const char *fmt, ...)
{
	va_list args;
	int i;
	char buf[OPT_KPRINTF_BUFSIZE];

	va_start(args, fmt);
	i = vsnprintf(buf, OPT_KPRINTF_BUFSIZE, fmt, args);
	va_end(args);
	print(buf);

	return i;
}

