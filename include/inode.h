/*
 * Copyright (c) 2006, 2007 Alexander Shishkin
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

#ifndef __INODE_H__
#define __INODE_H__

#include <klist0.h>
#include <atomic.h>
#include <super.h>
#include <file.h>
#include <mount.h>
/* yes, I'm aware of cross-inclusion */

typedef u32 ino_t;

#define INODE_NEW (0x1)

#define ROOT_INO 2

/* this is temporary, till we have sane pages */
#define INODE_PAGES 64 /* 256kb per file is, khm.. */
struct inode_mapping {
	/* and this assumes consequent reading only */
	struct page 	*i_pages[INODE_PAGES];
	u32		i_filled;     /* how many pages are there */

	/* I don't see the point in separating these */
	int (*read_page)(struct page *page, off_t offset);
	int (*write_page)(struct page *page, off_t offset);
};

struct inode;

struct inode_operations {
	struct direntry *(*lookup)(struct inode *inode, char *name);
};

struct inode {
	struct klist0_node i_sblist;  /* sb list linkage */
	struct klist0_node i_dent;    /* direntries */
	struct superblock *i_sb;      /* our sb */
	struct mount *i_mount;
	void *i_private;
	u32 i_state;
	ino_t i_ino;
	atomic_u32 i_refcnt;
	u32 i_mode;
	u32 i_dev;
	u32 i_size;
	u32 i_nlinks;
	u32 i_uid;
	u32 i_gid;
	u32 i_atime;
	u32 i_ctime;
	u32 i_mtime;
	void *i_ctl;
	struct inode_mapping i_map;    /* what's inside */
	struct klist0_node i_children; /* direntries list */
	struct inode *i_parent;
	struct inode_operations *i_ops;
	struct file_operations *i_fops;/* this propagates to file */
};

struct inode *alloc_inode();
void kill_inode(struct inode *inode);
struct inode *new_inode(struct superblock *sb);
void free_inode(struct inode *inode);
struct inode *get_inode(struct superblock *sb, ino_t ino);
void release_inode(struct inode *inode);
void dump_inodes();

#endif

