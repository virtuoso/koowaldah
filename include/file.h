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

#endif
