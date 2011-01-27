/*
 * kernel/fs/fs.h
 *
 * Copyright (C) 2006 Alexander Shishkin
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
 * 
 */

#ifndef __KERNEL_FS_FS_H__
#define __KERNEL_FS_FS_H__

#include <thread.h> /* for struct thread */

/*
 * Some internal FS related things that are shared between objects.
 * Anyone outside this directory has absolutely no business even looking
 * here.
 */

/* these come from ld script */
extern char rootfs_start;
extern char rootfs_end;

int alloc_fd(struct thread *thread);
void free_fd(struct thread *thread, int fd);

int cpio_read();
void fs_insert_entry(char *pathname, mode_t mode, dev_t dev, char *buf,
		size_t len);
void __init fs_init_super();
int __init fs_init_inodes();
void __init fs_init_namespace();
void __init fs_init_mount();
void __init fs_init_virtfs();
void __init fs_mpipe_init();

#endif
