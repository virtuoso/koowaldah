/*
 * include/aout.h
 *
 * Copyright (C) 2007 Alexander Shishkin
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
 */

#ifndef __AOUT_H__
#define __AOUT_H__

/* XXX: may not be valid for other arches though */
#define AOUT_PAGESIZE (PAGE_SIZE)

struct exec {
	u32  a_midmag;       /* flags<<26 | mid<<16 | magic */
	u32  a_text;         /* text segment size */
	u32  a_data;         /* initialized data size */
	u32  a_bss;          /* uninitialized data size */
	u32  a_syms;         /* symbol table size */
	u32  a_entry;        /* entry point */
	u32  a_trsize;       /* text relocation size */
	u32  a_drsize;       /* data relocation size */
};

#define AOUTH(x) ((struct exec *)x)

/* aout types */
#define AOUT_OMAGIC 0407     /* currently supported type */
#define AOUT_NMAGIC 0410
#define AOUT_ZMAGIC 0413
#define AOUT_QMAGIC 0414

#define AOUT_MAGIC(h) \
	(h->a_midmag & 0xffff)

/* is it an omagic aout? */
#define AOUT_isOMAGIC(h) \
	(AOUT_MAGIC(h) == AOUT_OMAGIC)

/* machine types */
#define AOUT_PC386   100     /* what we have atm */
#define AOUT_POWERPC 149     /* what we hope to support in future */
#define AOUT_ARM6    143     /* likewise */

#define AOUT_MID(h) \
	((h->a_midmag >> 16) & 0x3ff)

/* omagic sections are not aligned */
#define AOUT_ALIGN(h,x) \
	(x)

/* text section offset */
#define AOUT_TEXTOFF(h)               \
	(sizeof(struct exec))

/* data section offset */
#define AOUT_DATAOFF(h)               \
	(AOUT_TEXTOFF(h) + h->a_text)

/* text relocation section offset */
#define AOUT_TRELOFF(h)               \
	(AOUT_DATAOFF(h) + h->a_data)

/* bss section offset */
#define AOUT_DRELOFF(h)                \
	(AOUT_TRELOFF(h) + h->a_trsize)

/* bss section offset */
#define AOUT_SYMOFF(h)                 \
	(AOUT_DRELOFF(h) + h->a_drsize)

/* bss section offset */
#define AOUT_STROFF(h)                 \
	(AOUT_SYMOFF(h) + h->a_syms)

int aout_load(struct inode *inode, struct mapping *map);

#endif /* __AOUT_H__ */

