/*
 * kernel/fs/cpio.c
 *
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

#define ALIGN2(x) roundup_l2((x), 2)

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
			body = name + ALIGN2(cpio2u16(r, h_namesize));
			fs_insert_entry(name, mode,
					NODEV,
					body, fsize);

			t = body + ALIGN2(fsize);
		} else {
			fs_insert_entry(name, mode,
					DEV_DEVICE(r->h_rdev[1], r->h_rdev[0]),
					NULL, 0);
			t = name + ALIGN2(cpio2u16(r, h_namesize));
		}
	}

	return 0;
}

