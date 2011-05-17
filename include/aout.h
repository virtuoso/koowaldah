/*
 * include/aout.h
 *
 * Copyright (C) 2007 Alexander Shishkin
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

#ifdef OPT_AOUT_SUPPORT
int aout_load(struct inode *inode, struct mapping *map);
#endif

#endif /* __AOUT_H__ */

