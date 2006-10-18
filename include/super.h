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

#ifndef __SUPER_H__
#define __SUPER_H__

#include <klist0.h>
#include <device.h>
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
};

struct superblock *get_super(dev_t dev);

#endif
