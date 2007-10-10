/*
 * kernel/fs/cpio.c
 *
 * Copyright (C) 2006 Alexander Shishkin
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Koowaldah developers nor the names of their
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
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
#include <error.h>
#include <khui/stat.h>
#include <lib.h>
#include <klist0.h>
#include <device.h>
#include <inode.h>
#include "fs.h"

/*
 * cpio archive routines
 * derived from PAX implementation in NetBSD
 */
#define MAGIC           070707          /* transportable archive id */
#define TRAILER "TRAILER!!!"            /* last archive entry */

typedef struct {
	u8 h_magic[2];
	u8 h_dev[2];
	u8 h_ino[2];
	u8 h_mode[2];
	u8 h_uid[2];
	u8 h_gid[2];
	u8 h_nlink[2];
	u8 h_rdev[2];
	u8 h_mtime_1[2];
	u8 h_mtime_2[2];
	u8 h_namesize[2];
	u8 h_filesize_1[2];
	u8 h_filesize_2[2];
} HD_BCPIO;

#define cpio2su16(s, f) ((u16)s->f[1] << 8 | s->f[0])
#define cpio2ru16(s, f) ((u16)s->f[0] << 8 | s->f[1])
#define cpio2u16(s, f) (reverse ? cpio2ru16(s,f) : cpio2su16(s,f))

#define ALIGN4(x) ({ \
		unsigned long __r = ((unsigned long)(x) + 3) & ~3; \
		(__typeof__ (x))__r; \
	})

#define _cast(x) ((HD_BCPIO *)x)

int cpio_read()
{
	HD_BCPIO *r;
	char *t = &rootfs_start;
	char *name;
	char *body;
	size_t fsize;
	mode_t mode;
	int reverse = 0;

	/* check byte order first */
	if (cpio2ru16(_cast(t), h_magic) == MAGIC)
		reverse = 1;

	while (t < &rootfs_end) {
		/* obtain header and check magic */
		while (t < &rootfs_end && 
				cpio2u16(_cast(t), h_magic) != MAGIC)
			t++;

		r = _cast(t);
		if (cpio2u16(r, h_magic) != MAGIC) {
			kprintf("no magic :(\n");
			return -EIO;
		}

#ifdef OPT_DEBUG_CPIO
		kprintf("\nheader found at %d offset\n", t - &rootfs_start);
		kprintf("entry info:\n"
				" ino=%d uid=%d gid=%d nlink=%d\n"
				" namesize=%d filesize=%d dev=%d,%d\n",
				cpio2u16(r, h_ino),
				cpio2u16(r, h_uid),
				cpio2u16(r, h_gid),
				cpio2u16(r, h_nlink),
				cpio2u16(r, h_namesize),
				cpio2u16(r, h_filesize_2),
				r->h_rdev[1], r->h_rdev[0]
		      );
#endif
		fsize = cpio2u16(r, h_filesize_1) << 16 |
			cpio2u16(r, h_filesize_2);
		mode = cpio2u16(r, h_mode);
		name = t + sizeof(HD_BCPIO);
		if (!kstrcmp(name, TRAILER))
			break;

#ifdef OPT_DEBUG_CPIO
		kprintf("name: %s, size=%d\n", name, fsize);
#endif
		if (!S_ISCHR(mode) && !S_ISDIR(mode)) {
			body = ALIGN4(name + cpio2u16(r, h_namesize));
			fs_insert_entry(name, mode,
					NODEV,
					body, fsize);

			t = body + fsize;
		} else {
			fs_insert_entry(name, mode,
					DEV_DEVICE(r->h_rdev[1], r->h_rdev[0]),
					NULL, 0);
			t = ALIGN4(t + sizeof(HD_BCPIO));
		}
	}

	return 0;
}

