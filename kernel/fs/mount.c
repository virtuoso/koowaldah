/*
 * kernel/fs/mount.c
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
#include <mount.h>
#include <namespace.h>
#include <khui/stat.h>
#include <lib.h>
#include <klist0.h>
#include <slice.h>

static struct slice_pool *mount_pool;
static KLIST0_DECLARE(mount_list);

void __init fs_init_mount()
{
	mount_pool = slice_pool_create(0, sizeof(struct mount));
        bug_on(!mount_pool);
}

struct mount *alloc_mount()
{
	struct mount *mnt;

	mnt = slice_alloc(mount_pool);
	if (!mnt)
		return NULL; /* ERRPTR */

	KLIST0_INIT(&mnt->m_list);
	klist0_append(&mnt->m_list, &mount_list);

	return mnt;
}

void kill_mount(struct mount *mnt)
{
	klist0_unlink(&mnt->m_list);
	slice_free(mnt, mount_pool);
}

struct mount *get_mount(char *path)
{
	return NULL;
}

int mount0(struct direntry *mntpt, struct superblock *mntsb)
{
	struct mount *mnt;

	mnt = alloc_mount();
	if (!mnt)
		return -ENOMEM;

	mnt->m_inode = mntpt->d_inode;
	mnt->m_sb = mntsb;

	mntpt->d_inode->i_mount = mnt;
	mntpt->d_inode = mnt->m_sb->s_root;

	return 0;
}

