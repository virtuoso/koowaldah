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
#include <error.h>
#include <klist0.h>
#include <mm.h>
#include <device.h>
#include <super.h>
#include <inode.h>

/*
 * FS super block related operations.
 */

/* all the king's man */
static struct klist0_node superblock_list;

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

	sb = memory_alloc(sizeof(struct superblock));
	if (!sb)
		return NULL; /* XXX: ERRPTR? */

	KLIST0_INIT(&sb->s_blocks);
	KLIST0_INIT(&sb->s_ilist);
	sb->s_inodes = 0;
	sb->s_magic = SB_MAGIC_ROOTFS; /* fs driver should change this */
	sb->s_dev = dev;
	sb->s_flags = SB_NEW;
	sb->s_ops = &generic_sbops;

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
	memory_release(sb);

	return 0;
}

/*
 * Find a superblock object by a backing device
 * @dev -- backing device
 */
struct superblock *get_super(dev_t dev)
{
	struct klist0_node *t;
	struct superblock *sb;

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

