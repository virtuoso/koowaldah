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

#ifndef __INODE_H__
#define __INODE_H__

#include <klist0.h>
#include <atomic.h>
#include <super.h>
#include <file.h>
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

struct inode {
	struct klist0_node i_sblist;  /* sb list linkage */
	struct klist0_node i_dent;    /* direntries */
	struct superblock *i_sb;      /* our sb */
	u32 i_state;
	ino_t i_ino;
	atomic_u32 i_refcnt;
	u32 i_mode;
	u32 i_size;
	u32 i_nlinks;
	u32 i_uid;
	u32 i_gid;
	u32 i_atime;
	u32 i_ctime;
	u32 i_mtime;
	struct inode_mapping i_map;    /* what's inside */
	struct klist0_node i_children; /* direntries list */
	struct inode *i_parent;
	struct file_operations *i_fops;/* this propagates to file */
};

struct inode *alloc_inode();
void kill_inode(struct inode *inode);
struct inode *new_inode(struct superblock *sb);
void free_inode(struct inode *inode);
struct inode *get_inode(struct superblock *sb, ino_t ino);
void release_inode(struct inode *inode);

#endif

