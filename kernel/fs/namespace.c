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
#include <lib.h>
#include <device.h>
#include <super.h>
#include <inode.h>
#include <file.h>
#include <namespace.h>

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
	memory_copy(dent->d_name, name,
			MIN(FILENAME_MAX, string_len(name)));
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
	inode = get_inode(sb, 0);
	if (!inode)
		bug();

	/* assign a name for it */
	dent = new_direntry("/", inode);

	root_ns.n_inode = inode;
}

