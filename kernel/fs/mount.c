/*
 * kernel/fs/mount.c
 *
 * Copyright (C) 2007 Alexander Shishkin
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Koowaldah developers nor the names of their
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE DEVELOPERS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE DEVELOPERS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
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

