/*
 * Copyright (c) 2007, Alexander Shishkin
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

#ifndef __MOUNT_H__
#define __MOUNT_H__

#include <klist0.h>
#include <device.h>
#include <khui/fs.h>
#include <super.h>
#include <inode.h>

struct direntry;

struct mount {
	struct klist0_node m_list;
	struct superblock *m_sb;  /* the fs that we're about to mount */
	struct inode *m_inode;    /* mountpoint inode */
};

struct mount *alloc_mount();
struct mount *get_mount(char *path);
void kill_mount(struct mount *mnt);
int mount0(struct direntry *mntpt, struct superblock *mntsb);

#endif
