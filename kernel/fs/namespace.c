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

#include <koowaldah.h>
#include <bug.h>
#include <error.h>
#include <klist0.h>
#include <thread.h>
#include <mm.h>
#include <lib.h>
#include <device.h>
#include <super.h>
#include <inode.h>
#include <file.h>
#include <namespace.h>
#include <sys/stat.h>

/*
 * FS namespaces, names etc related code
 */

struct namespace root_ns;

struct direntry *new_direntry(char *name, struct inode *inode)
{
	struct direntry *dent;

	if (!inode || !name)
		bug();

	dent = memory_alloc(sizeof(struct direntry));
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
	if (atomic_read(&dent->d_refcnt))
		bug();

	klist0_unlink(&dent->d_idlist);
	memory_release(dent);
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

	/* we don't have inode ops yet */
#if 0
	if (klist0_empty(&inode->i_children))
		inode->i_ops->readdir(inode);
#endif

	klist0_for_each(d, &inode->i_children) {
		child_dent = klist0_entry(d, struct direntry, d_siblings);
		if (!kstrcmp(child_dent->d_name, name))
			return child_dent;
	}

	return NULL;
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

	if (*pathname == '/')
		p++;

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
	}

	dent = lookup_direntry(p, inode);

	return dent;
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
}

