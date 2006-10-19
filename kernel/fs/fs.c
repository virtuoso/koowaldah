/*
 * Copyright (c) 2006, Alexander Shishkin
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <kuca.h>
#include <bug.h>
#include <error.h>
#include <klist0.h>
#include <mm.h>
#include <lib.h>
#include <device.h>
#include <super.h>
#include <inode.h>
#include <file.h>
#include <namespace.h>
#include <page_alloc.h>
#include <sys/stat.h>

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

struct inode *fs_add_entry(struct inode *parent, char *name, u32 mode)
{
	struct superblock *sb = parent->i_sb;
	struct inode *inode;
	struct direntry *dent;

	inode = new_inode(sb);
	if (!inode)
		bug();
	inode->i_ino = parent->i_ino + 1; /* XXX */
	inode->i_mode = mode;

	dent = new_direntry(name, inode);
	if (!dent)
		bug();

	klist0_append(&dent->d_siblings, &parent->i_children);
	return inode;
}

static void __future fs_stuff_inode(struct inode *inode, char *src, off_t len)
{
	struct page **pages = inode->i_map.i_pages;
	int npages = inode->i_map.i_filled;
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

extern void __init fs_init_super();
extern int __init fs_init_inodes();
extern void __init fs_init_namespace();

/*
 * Initialize FS and create rootfs
 *
 * ROOTFS:
 *  /dev
 *  /dev/console
 *  /initfs
 *  /mnt
 */
void __init fs_init()
{
	struct superblock *sb;
	struct inode *root;
	struct inode *p;

	fs_init_super();
	fs_init_inodes();
	fs_init_namespace();

	sb = get_super(ROOTFSDEV);
	sb->s_ops->read_inode = fs_read_inode;
	sb->s_ops->write_inode = fs_write_inode;
	root = sb->s_root;

	p = fs_add_entry(root, "dev", S_IFDIR);
	p = fs_add_entry(p, "console", S_IFCHR);
	p = fs_add_entry(root, "initfs", S_IFDIR);
	p = fs_add_entry(root, "mnt", S_IFDIR);
}

