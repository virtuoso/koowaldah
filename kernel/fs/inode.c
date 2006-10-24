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
#include <device.h>
#include <super.h>
#include <inode.h>
#include <namespace.h>
#include <sys/stat.h>

/*
 * FS inode operations
 */

/*
 * Anonymous inodes are those who are not associated with any
 * super block, like pipes or sockets.
 */
static struct klist0_node anon_inodes;

/*
 * Generic inode allocation method
 */
struct inode *alloc_inode()
{
	struct inode *inode;
	
	inode = memory_alloc(sizeof(struct inode));
	if (!inode)
		return NULL; /* XXX: ERRPTR again */

	KLIST0_INIT(&inode->i_sblist);
	return inode;
}

/*
 * Generic inode deallocation method
 * @inode -- inode to be deallocated
 */
void kill_inode(struct inode *inode)
{
	/* inode should be unused at this point */
	if (atomic_read(&inode->i_refcnt))
		bug();

	klist0_unlink(&inode->i_sblist);
	memory_release(inode);
}

/*
 * Obtain a new inode associated with a given superblock
 * @sb -- the superblock, NULL for anonymous inode
 */
struct inode *new_inode(struct superblock *sb)
{
	struct inode *inode;

	if (sb)
		inode = sb->s_ops->alloc_inode();
	else
		inode = alloc_inode();

	inode->i_sb = sb;
	if (sb)
		klist0_append(&inode->i_sblist, &sb->s_ilist);

	KLIST0_INIT(&inode->i_dent);
	ATOMIC_INIT(&inode->i_refcnt);

	/* relatives */
	KLIST0_INIT(&inode->i_children);
	inode->i_parent = NULL; /* we are disconnected */

	inode->i_state = INODE_NEW;

	/* following fields must be filled in by fs driver */
	inode->i_ino = 0;
	inode->i_mode = 0;
	inode->i_dev = NODEV;
	inode->i_size = 0;
	inode->i_nlinks = 0;
	inode->i_uid = inode->i_gid = 0;
	inode->i_atime = inode->i_ctime = inode->i_mtime = 0;
	inode->i_fops = NULL;
	inode->i_map.i_filled = 0;
	inode->i_map.read_page = NULL;
	inode->i_map.write_page = NULL;

	return inode;
}

/*
 * Get rid of an inode. Should be normally called when
 * inode is no longer used (see i_refcnt).
 * @inode -- inode to be gotten rid of
 */
void free_inode(struct inode *inode)
{
	struct direntry *dent;
	struct klist0_node *t;

	/* remove all associated direntries, if any */
	if (!klist0_empty(&inode->i_dent)) {
		t = &inode->i_dent;
		do {
			dent = klist0_entry(t, struct direntry, d_idlist);
			t = t->next;
			free_direntry(dent);
		} while (t != &inode->i_dent);
	}

	if (inode->i_sb)
		inode->i_sb->s_ops->kill_inode(inode);
	else
		kill_inode(inode);
}

/*
 * Find an inode associated with a given superblock, with given
 * number and increment its reference count.
 *
 * By calling this you imply you are the use of this inode and thus
 * fully responsible of it.
 * @sb -- superblock
 * @ino -- inode number
 */
struct inode *get_inode(struct superblock *sb, ino_t ino)
{
	struct klist0_node *t;
	struct klist0_node *list;
	struct inode *inode;

	if (sb)
		list = &sb->s_ilist;
	else
		list = &anon_inodes;

	klist0_for_each(t, list) {
		inode = klist0_entry(t, struct inode, i_sblist);
		if (inode->i_ino == ino)
			goto out;
	}

	/* search failed, allocate */
	inode = new_inode(sb);
	if (!inode)
		return NULL;

	inode->i_ino = ino;
	/* we read via fops' read method on anonymous inodes */
	if (sb)
		sb->s_ops->read_inode(inode);
out:
	atomic_inc_u32(&inode->i_refcnt);
	return inode;
}

/*
 * Decrement usage count of a given inode and free it
  Opposite of get_inode()* if no longer in use. Opposite of get_inode()
 * @inode -- inode
 */
void release_inode(struct inode *inode)
{
	if (atomic_dec_and_test_u32(&inode->i_refcnt))
		free_inode(inode);
}

/*
 * Do all the necessary things to get inode caches work.
 */
int __init fs_init_inodes()
{
	struct superblock *sb;
	struct inode *root;

	sb = get_super(ROOTFSDEV);
	if (!sb)
		bug();

	/* root inode */
	root = new_inode(sb);
	root->i_ino = ROOT_INO;
	root->i_mode = S_IFDIR;
	sb->s_root = root;
	KLIST0_INIT(&anon_inodes);
	return 0;
}

