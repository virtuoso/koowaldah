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
#include <error.h>
#include <klist0.h>
#include <galloc.h>
#include <device.h>
#include <super.h>
#include <inode.h>

/*
 * FS super block related operations.
 */

/* all the king's man */
static KLIST0_DECLARE(superblock_list);

static struct super_operations generic_sbops = {
	.alloc_inode = alloc_inode,
	.kill_inode  = kill_inode,
	.read_inode  = NULL,
	.write_inode = NULL
};

/*
 * Allocate a super block for a given back-end device
 * @dev -- the backing device
 */
struct superblock *alloc_super(dev_t dev)
{
	struct superblock *sb;

	sb = gobj_alloc(struct superblock);
	if (!sb)
		return NULL; /* XXX: ERRPTR? */

	KLIST0_INIT(&sb->s_blocks);
	KLIST0_INIT(&sb->s_ilist);
	sb->s_inodes = 0;
	sb->s_magic = SB_MAGIC_ROOTFS; /* fs driver should change this */
	sb->s_dev = dev;
	sb->s_flags = SB_NEW;
	sb->s_ops = &generic_sbops;
	sb->s_iops = NULL;
	sb->s_root = NULL;

	klist0_append(&sb->s_blocks, &superblock_list);
	return sb;
}

/*
 * Deallocate a super block (try to free its inodes first)
 * @sb -- the super block
 */
int kill_super(struct superblock *sb)
{
	/* free sb->s_ilist */
	klist0_unlink(&sb->s_blocks);
	gobj_free(sb);

	return 0;
}

/*
 * Find a superblock object by a backing device
 * @dev -- backing device
 *
 * XXX: should be similar to get/release_inode().
 */
struct superblock *get_super(dev_t dev)
{
	struct klist0_node *t;
	struct superblock *sb;

	if (klist0_empty(&superblock_list))
		return NULL;

	klist0_for_each(t, &superblock_list) {
		sb = klist0_entry(t, struct superblock, s_blocks);
		if (sb->s_dev == dev)
			return sb;
	}
	return NULL;
}

void __init fs_init_super()
{
	KLIST0_INIT(&superblock_list);
	/* system rootfs */
	alloc_super(ROOTFSDEV);
}

