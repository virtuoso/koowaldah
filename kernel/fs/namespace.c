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
#include <thread.h>
#include <galloc.h>
#include <lib.h>
#include <device.h>
#include <super.h>
#include <inode.h>
#include <file.h>
#include <namespace.h>
#include <khui/stat.h>

/*
 * FS namespaces, names etc related code
 */

struct namespace root_ns;

struct direntry *new_direntry(char *name, struct inode *inode)
{
	struct direntry *dent;

	if (!inode || !name)
		bug();

	dent = gobj_alloc(struct direntry);
	if (!dent)
		return NULL;
	
	ATOMIC_INIT(&dent->d_refcnt);
	KLIST0_INIT(&dent->d_idlist);
	KLIST0_INIT(&dent->d_siblings);
	memory_copy(dent->d_name, name,
			MIN(FILENAME_MAX, kstrlen(name) + 1));
	dent->d_inode = inode;
	klist0_append(&dent->d_idlist, &inode->i_dent);
	return dent;
}

void free_direntry(struct direntry *dent)
{
	/* direntry should be unused */
	if (atomic_read(&dent->d_refcnt)) {
		kprintf("# tried to free %s dent\n", dent->d_name);
		bug();
	}

	klist0_unlink(&dent->d_idlist);
	gobj_free(dent);
}

/*
 * Look up a given name in inode's children.
 * @name -- part of a pathname to look for
 * @inode -- directory's inode
 */
struct direntry *lookup_direntry(char *name, struct inode *inode)
{
	struct direntry *child_dent;
	struct klist0_node *d;

	if (!S_ISDIR(inode->i_mode))
		return NULL;

	klist0_for_each(d, &inode->i_children) {
		child_dent = klist0_entry(d, struct direntry, d_siblings);
		if (!kstrcmp(child_dent->d_name, name))
			return child_dent;
	}

	/* cache lookup failed, let's ask the filesystem driver */
	child_dent = inode->i_ops->lookup(inode, name);

	return child_dent;
}

/*
 * Look up a pathname
 * @pathname -- a full *normalized* pathname
 */
struct direntry *lookup_path(char *pathname)
{
	struct thread *me = CURRENT();
	struct inode *inode = me->ns->n_inode;
	struct direntry *dent;
	char name[FILENAME_MAX];
	char *p = pathname;
	char *n;

	while (*p == '/')
		p++;

	if (*p == '\0')
		return root_ns.n_dent;

	while ((n = kstrchr(p, '/'))) {
		memory_copy(name, p, n - p);
		name[n - p] = '\0';

		dent = lookup_direntry(name, inode);
		if (!dent)
			return NULL;

		p = n + 1;
		/* it's absolutely necessary for inode to be there */
		inode = dent->d_inode;
		if (!inode)
			bug();

		if (inode->i_mount)
			inode = inode->i_mount->m_sb->s_root;
	}

	dent = lookup_direntry(p, inode);

	return dent;
}

void release_direntry(struct direntry *dent)
{
	struct inode *inode = dent->d_inode;

	if (atomic_dec_and_test_u32(&dent->d_refcnt))
		free_direntry(dent);

	release_inode(inode);
}

/*
 * Create root namespace
 */
void __init fs_init_namespace()
{
	struct superblock *sb;
	struct inode *inode;
	struct direntry *dent;

	/* get rootfs superblock */
	sb = get_super(ROOTFSDEV);
	if (!sb)
		bug();

	/* get root inode */
	inode = get_inode(sb, ROOT_INO);
	if (!inode)
		bug();

	/* assign a name for it */
	dent = new_direntry("/", inode);

	root_ns.n_inode = inode;
	root_ns.n_dent = dent;
}

