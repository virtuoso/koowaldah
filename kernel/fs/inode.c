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
#include <galloc.h>
#include <device.h>
#include <super.h>
#include <inode.h>
#include <namespace.h>
#include <khui/stat.h>

/*
 * FS inode operations
 */

/*
 * Anonymous inodes are those who are not associated with any
 * super block, like pipes or sockets.
 */
static struct klist0_node anon_inodes;

/*
 * Generic lookup method,
 * a noop, for rootfs all the names are in the cache
 */
struct direntry *generic_lookup(struct inode *inode, char *name)
{
	if (!S_ISDIR(inode->i_mode))
		return NULL; /* ERRPTR=-EINVAL */

	return NULL; /* ERRPTR=-ENOENT */
}

/*
 * Generic inode operations
 */
static struct inode_operations generic_iops = {
	.lookup = generic_lookup,
};

/*
 * Generic inode allocation method
 */
struct inode *alloc_inode()
{
	struct inode *inode;
	
	inode = gobj_alloc(struct inode);
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
	gobj_free(inode);
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

	inode->i_mount = NULL;

	/* following fields must be filled in by fs driver */
	inode->i_ino = 0;
	inode->i_mode = 0;
	inode->i_dev = NODEV;
	inode->i_ctl = NULL;
	inode->i_size = 0;
	inode->i_nlinks = 0;
	inode->i_uid = inode->i_gid = 0;
	inode->i_atime = inode->i_ctime = inode->i_mtime = 0;
	inode->i_ops = sb->s_iops ? sb->s_iops : &generic_iops;
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

	/* remove mountpoint(s), if any */
	if (inode->i_mount)
		kill_mount(inode->i_mount);

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

void dump_inodes()
{
	struct klist0_node *ti, *td;
	struct inode *inode;
	struct direntry *dent;
	struct superblock *sb = get_super(ROOTFSDEV);

scan:
	klist0_for_each(ti, &sb->s_ilist) {
		inode = klist0_entry(ti, struct inode, i_sblist);

		kprintf("ino %d: refcnt=%d sz=%d nl=%d names:",
				inode->i_ino, atomic_read(&inode->i_refcnt),
				inode->i_size, inode->i_nlinks);

		klist0_for_each(td, &inode->i_dent) {
			dent = klist0_entry(td, struct direntry, d_idlist);

			kprintf(" %s", dent->d_name);
		}

		kprintf("\n");
	}

	if (sb->s_dev == ROOTFSDEV) {
		sb = get_super(VIRTFSDEV);
		goto scan;
	}
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

