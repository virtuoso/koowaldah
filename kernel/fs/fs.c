/*
 * Copyright (c) 2006, Alexander Shishkin
 * All rights reserved.
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

#include <koowaldah.h>
#include <bug.h>
#include <error.h>
#include <klist0.h>
#include <lib.h>
#include <device.h>
#include <super.h>
#include <inode.h>
#include <file.h>
#include <namespace.h>
#include <message.h>
#include <page_alloc.h>
#include <khui/stat.h>
#include "fs.h"

/*
 * FS core; rootfs filesystem
 */

static void fs_read_inode(struct inode *inode)
{
	/* 
	 * this should never be called, because all the rootfs' inodes
	 * should be there already
	 */
	bug();
}

static void fs_write_inode(struct inode *inode)
{
	/*
	 * no backend -- no writeback
	 */
}

struct inode *fs_add_entry(struct inode *parent, char *name, mode_t mode,
		dev_t dev)
{
	struct superblock *sb = parent->i_sb;
	struct inode *inode;
	struct direntry *dent;
	static ino_t ino = ROOT_INO;

	inode = new_inode(sb);
	if (!inode)
		bug();
	inode->i_ino = ++ino; /* XXX */
	inode->i_mode = mode;
	inode->i_dev = dev;
	/* we want these inodes forever */
	atomic_inc_u32(&inode->i_refcnt);

	dent = new_direntry(name, inode);
	if (!dent)
		bug();
	atomic_inc_u32(&dent->d_refcnt);

	klist0_append(&dent->d_siblings, &parent->i_children);
	return inode;
}

static void fs_stuff_inode(struct inode *inode, char *src, off_t len)
{
	struct page **pages = inode->i_map.i_pages;
	int npages = 0; //inode->i_map.i_filled;
	off_t pglen = len >> PAGE_SHIFT;
	char *from = src;
	off_t left = len;
	int i;

	for (i = 0; i <= pglen; i++) {
		pages[npages] = alloc_pages(0, 0);
		memory_copy(page_to_addr(pages[npages]),
				from, MIN((off_t)PAGE_SIZE, left));
		from += PAGE_SIZE;
		left -= PAGE_SIZE;
		npages++;
	}

	inode->i_size = len;
	inode->i_map.i_filled = npages;
}

void fs_insert_entry(char *pathname, mode_t mode, dev_t dev, char *buf,
		size_t len)
{
	struct inode *inode;
	struct direntry *dent;
	char *p = kstrrchr(pathname, '/');
	char dir[FILENAME_MAX]; /* XXX: should be PATH_MAX and limits.h */

	if (!kstrcmp(pathname, "."))
		return;

	/* silently skip the attempt to insert an existing entry */
	dent = lookup_path(pathname);
	if (dent)
		return;

	memory_set(dir, '\0', FILENAME_MAX);

	if (p)
		memory_copy(dir, pathname, p - pathname);
	else {
		dir[0] = '/';
		p = pathname;
	}

	if (p == pathname)
		inode = root_ns.n_inode;
	else {
		dent = lookup_path(dir);
		if (!dent) {
			kprintf("directory %s not found\n", dir);
			bug();
		}
		inode = dent->d_inode;
		p++; /* skip the slash */
	}

	inode = fs_add_entry(inode, p, mode, dev);
	if (S_ISREG(mode) && buf && len)
		fs_stuff_inode(inode, buf, len);
}

/*
 * Initialize FS and create rootfs
 *
 * ROOTFS:
 *  /dev
 *  /dev/console
 *  /initfs
 *  /mnt
 *  /koowaldah
 */
void __init fs_init()
{
	struct superblock *sb;
	struct inode *root;
	struct inode *p;

	fs_init_super();
	fs_init_inodes();
	fs_init_namespace();
	fs_init_mount();

	sb = get_super(ROOTFSDEV);
	sb->s_ops->read_inode = fs_read_inode;
	sb->s_ops->write_inode = fs_write_inode;
	root = sb->s_root;

	p = fs_add_entry(root, "dev", S_IFDIR, NODEV);
	fs_add_entry(p, "tty0", S_IFCHR, DEV_DEVICE(4, 0));
	fs_add_entry(p, "pckbd", S_IFCHR, DEV_DEVICE(13, 128));
	fs_add_entry(p, "serial", S_IFCHR, DEV_DEVICE(12, 34));
	p = fs_add_entry(root, "initfs", S_IFDIR, NODEV);
	p = fs_add_entry(root, "mnt", S_IFDIR, NODEV);
	p = fs_add_entry(root, "koowaldah", S_IFDIR, NODEV);

	cpio_read();
	devices_init();
	messaging_init();
	fs_init_virtfs();
}

