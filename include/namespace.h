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

#ifndef __NAMESPACE_H__
#define __NAMESPACE_H__

/*
 * Namespaces, directory entries and whatever else names.
 */

#include <klist0.h>
#include <atomic.h>
#include <super.h>
#include <inode.h>

extern struct namespace root_ns;

struct direntry {
	struct inode *d_inode;
	struct klist0_node d_idlist;   /* i_dent linkage */
	char d_name[FILENAME_MAX];
	atomic_u32 d_refcnt;
	struct klist0_node d_siblings; /* i_children linkage */
};

struct direntry *new_direntry(char *name, struct inode *inode);
void free_direntry(struct direntry *dent);

struct direntry *lookup_direntry(char *name, struct inode *inode);
struct direntry *lookup_path(char *pathname);
void release_direntry(struct direntry *dent);

struct namespace {
	struct inode *n_inode;  /* inode to start the namespace */
	struct direntry *n_dent;
};

#endif
