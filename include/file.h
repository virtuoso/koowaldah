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

#ifndef __FILE_H__
#define __FILE_H__

/*
 * File related data structures and operations
 */

#include <klist0.h>
#include <atomic.h>
#include <super.h>
#include <inode.h>

struct file;

struct file_operations {
	int (*open)(struct file *file);
	int (*close)(struct file *file);
	int (*read)(struct file *file, char *buf, size_t size);
	int (*write)(struct file *file, char *buf, size_t size);
	int (*readdir)(struct file *file, struct udirentry *udents, int count);
};

struct file {
	struct superblock *f_sb;
	struct inode *f_inode;
	struct file_operations *f_ops;
	off_t f_offset;
	int f_fd;
	struct klist0_node f_tlist;
};

struct file *new_file();
void kill_file(struct file *file);

int generic_open(struct file *file);
int generic_close(struct file *file);

int open(char *name, int flags, mode_t mode);
int close(int fd);
int read(int fd, char *buf, size_t len);
int write(int fd, char *buf, size_t len);
int readdir(int fd, struct udirentry *udents, int count);
struct file *fd2file(int fd);

#endif
