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

#ifndef __SUPER_H__
#define __SUPER_H__

#include <klist0.h>
#include <device.h>
#include <khui/fs.h>
#include <inode.h>

/* magic of a "rootfs" superblock */
#define SB_MAGIC_ROOTFS (0x0)

/* sb flags */
#define SB_NEW (0x1)

struct super_operations {
	struct inode *(*alloc_inode)(void);
	void (*kill_inode)(struct inode *inode);
	void (*read_inode)(struct inode *inode);
	void (*write_inode)(struct inode *inode);
};

struct superblock {
	struct klist0_node s_blocks; /* big list linkage */
	struct klist0_node s_ilist;  /* our inodes */
	struct inode *s_root;        /* root inode */
	u32 s_magic;
	u32 s_inodes;
	dev_t s_dev;
	u32 s_flags;
	struct super_operations *s_ops;
	struct inode_operations *s_iops; /* this propagates to inodes */
};

struct superblock *alloc_super(dev_t dev);
struct superblock *get_super(dev_t dev);

#endif
