/*
 * kernel/fs/virtfs.c
 *
 * Copyright (C) 2007 Alexander Shishkin
 *
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
#include <super.h>
#include <inode.h>
#include <namespace.h>
#include <khui/stat.h>
#include <klist0.h>
#include <lib.h>
#include <slice.h>
#include <virtfs.h>
#include <mpipe.h>
#include "fs.h"

struct virtfs_operations veops_nop = { NULL, NULL, NULL };

/*
 * Find an entry starting from given inode as a search root
 * @root -- search root
 * @name -- name of the entry we're after
 * returns a valid pointer on success, NULL otherwise
 */
static struct virtfs_entry *virtfs_get_entry(struct inode *root, char *name)
{
	struct klist0_node *tmp;
	struct virtfs_entry *ve;
	struct virtfs_entry *root_ve = root->i_private;

	if (!root_ve || !root_ve->ve_child)
		return NULL;

	klist0_for_each(tmp, &root_ve->ve_child->ve_sibling) {
		ve = klist0_entry(tmp, struct virtfs_entry, ve_sibling);

		if (!kstrcmp(name, ve->ve_name))
			return ve;
	}

	return NULL;
}

static struct slice_pool *ve_pool;

struct virtfs_entry *new_ve()
{
	struct virtfs_entry *ve;

	ve = slice_alloc(ve_pool);
	if (!ve)
		return NULL;
	
	memory_set(ve, 0, sizeof(struct virtfs_entry));
	KLIST0_INIT(&ve->ve_sibling);

	return ve;
}

void free_ve(struct virtfs_entry *ve)
{
	klist0_unlink(&ve->ve_sibling);
	slice_free(ve, ve_pool);
}

/* forwards */
static struct file_operations virtfs_fops;
static struct inode_operations virtfs_iops;

/*
 * Inode operations
 */

/*
 * Lookup a name in the virtual fs, starting from a given inode
 * as a search root. An inode and a directory entry are created
 * in FS caches on success.
 * @inode -- search root
 * @name  -- name of an entry we're after
 * returns a direntry ptr on success, NULL on failure (TODO)
 */
struct direntry *virtfs_lookup(struct inode *inode, char *name)
{
	struct direntry *dent;
	struct inode *ret_inode;
	struct virtfs_entry *ve;
	struct virtfs_entry *root_ve = inode->i_private;

	DPRINT("# %s(): %s\n", __FUNCTION__, name);
	if (!S_ISDIR(inode->i_mode))
		return NULL; /* ERRPTR=-EINVAL */

	if (!root_ve)
		return NULL; /* ERRPTR=-EINVAL */

	if (root_ve->ve_ops->lookup)
		ve = root_ve->ve_ops->lookup(name); 
	else
		ve = virtfs_get_entry(inode, name);

	if (!ve)
		return NULL; /* ERRPTR=-ENOENT */

	/* ve_inode shouldn't be there, otherwise we couldn't end up
	 * in lookup() path at all */
	bug_on(ve->ve_inode);
	ret_inode = new_inode(inode->i_sb);
	bug_on(!ret_inode);

	ret_inode->i_ino = (u32)ve; /* XXX: this will suck on 64bit arches */
	ret_inode->i_mode = ve->ve_mode;
	ret_inode->i_size = S_ISDIR(ve->ve_mode) ? 0 : 1; /* XXX */
	ret_inode->i_dev = ve->ve_dev;
	ret_inode->i_fops = &virtfs_fops;
	ret_inode->i_ops = &virtfs_iops;
	atomic_inc_u32(&ret_inode->i_refcnt);


	dent = new_direntry(name, ret_inode);
	bug_on(!dent);
	atomic_inc_u32(&dent->d_refcnt);

	klist0_append(&dent->d_siblings, &inode->i_children);

	ve->ve_inode = ret_inode;
	ret_inode->i_private = ve;

	return dent;
}

static struct inode_operations virtfs_iops = {
	.lookup = virtfs_lookup,
};

/*
 * File operations
 */
static int virtfs_open(struct file *file)
{
	/*kprintf("-> opened\n");*/
	return 0;
}

static int virtfs_close(struct file *file)
{
	/*kprintf("-> closed\n");*/
	return 0;
}

#define VIRTFS_TEST_STRING "virtfs test\n"
static int virtfs_read(struct file *file, char *buf, size_t len)
{
	int r = kstrlen(VIRTFS_TEST_STRING);

	memory_copy(buf, VIRTFS_TEST_STRING, r + 1);
	/* kprintf("-> read\n"); */

	return r;
}

static int virtfs_write(struct file *file, char *buf, size_t offset)
{
	return 0;
}

static int virtfs_readdir(struct file *file, struct udirentry *udents, int count)
{
	struct klist0_node *tmp;
	struct virtfs_entry *ve;
	struct virtfs_entry *root_ve = file->f_inode->i_private;
	int c = 0;
	
	if (!S_ISDIR(file->f_inode->i_mode))
		return -ENOTDIR;

	/* a defined ve_ops->readdir does all the work for us */
	if (root_ve->ve_ops->readdir)
		return root_ve->ve_ops->readdir(udents, count);

	if (!root_ve->ve_child)
		return 0; /* nothing here */

	/* iterate through siblings */
	klist0_for_each(tmp, &root_ve->ve_child->ve_sibling) {
		ve = klist0_entry(tmp, struct virtfs_entry, ve_sibling);

		if (c == count) break;

		memory_copy(udents->name, ve->ve_name,
				kstrlen(ve->ve_name) + 1);
		udents++;
		c++;
	}

	return c;
}

static struct file_operations virtfs_fops = {
	.open    = virtfs_open,
	.close   = virtfs_close,
	.read    = virtfs_read,
	.write   = virtfs_write,
	.readdir = virtfs_readdir,
};

/*
 * Superblock operations
 */

/*
 * Deassociate a virtfs_entry from a dying inode.
 * Inodes in a virtfs *can* be free'ed unlike the rootfs ones,
 * since they have a good backing store: virtfs_entries tree.
 */
static void virtfs_kill_inode(struct inode *inode)
{
	struct virtfs_entry *ve;

	if (inode->i_private) {
		ve = inode->i_private;

		ve->ve_inode = NULL;
	}
}

static struct super_operations virtfs_sops = {
	.alloc_inode = alloc_inode,
	.kill_inode  = virtfs_kill_inode,
	.read_inode  = NULL,
	.write_inode = NULL
};

static void __init virtfs_init_inodes(struct superblock *sb)
{
	struct inode *root;

	root = new_inode(sb);
	root->i_ino = ROOT_INO;
	root->i_mode = S_IFDIR;
	root->i_fops = &virtfs_fops;
	root->i_ops = &virtfs_iops;
	sb->s_root = root;
}

/*
 * Skeleton of a virtfs layout: the root directory is 'koowaldah',
 * and the subtrees are 'proc' and 'srv'. More to come.
 */
VE_DECLARE(virtfs_tree, "koowaldah", S_IFDIR, NODEV, &veops_nop);
VE_DECLARE(_dot, ".",   S_IFDIR,  NODEV, &veops_nop);
VE_DECLARE(virtfs_srv,  ".",    S_IFDIR, NODEV, &veops_nop);
VE_DECLARE_STATIC(srv,  "srv",  S_IFDIR, NODEV, &veops_nop);
VE_DECLARE_STATIC(omg,  "omg",  S_IFDIR, NODEV, &veops_nop);
VE_DECLARE_STATIC(wtf,  "wtf",  S_IFREG, NODEV, &veops_nop);
VE_DECLARE_STATIC(lol,  "lol",  S_IFREG, NODEV, &veops_nop);

/*
 * Link the entries into a tree
 */
static void __init virtfs_init_tree()
{
	VE_ADD_SUBTREE(&virtfs_tree, &_dot);
	VE_ADD_SIBLING(&_dot, &srv);
	VE_ADD_SIBLING(&_dot, &wtf);

	VE_ADD_SUBTREE(&srv, &virtfs_srv);
	VE_ADD_SIBLING(&virtfs_srv, &omg);
	VE_ADD_SIBLING(&virtfs_srv, &lol);

	virtfs_proc_init();
	virtfs_dev_init();
}

/*
 * Initialize and mount virtfs.
 * Should be called when rootfs is initialized
 */
void __init fs_init_virtfs()
{
	struct superblock *sb, *root_sb;
	struct direntry *dent;
	int r;

	ve_pool = slice_pool_create(0, sizeof(struct virtfs_entry));
	bug_on(!ve_pool);

	sb = alloc_super(VIRTFSDEV);
	if (!sb)
		panic("Not enough memory to initialize virtfs");

	sb->s_iops = &virtfs_iops;
	sb->s_ops = &virtfs_sops;
	virtfs_init_inodes(sb);
	virtfs_init_tree();
	sb->s_root->i_private = &virtfs_tree;

	root_sb = get_super(ROOTFSDEV);
	if (!sb)
		panic("No root superblock?!");

	dent = lookup_path("/koowaldah");

	r = mount0(dent, sb);
	if (r)
		panic("Virtfs mount failed\n");

	fs_mpipe_init();
}

