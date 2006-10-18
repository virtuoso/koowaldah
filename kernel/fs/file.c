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
#include <device.h>
#include <super.h>
#include <inode.h>
#include <file.h>

/*
 * FS file operations.
 */
struct file *new_file()
{
	struct file *file;

	file = memory_alloc(sizeof(struct file));
	if (!file)
		return NULL;

	file->f_offset = 0;
	file->f_ops = NULL;
	file->f_sb = NULL;
	file->f_inode = NULL;
	return file;
}

void kill_file(struct file *file)
{
	memory_release(file);
}

int generic_open(struct file *file)
{
	return 0;
}

int generic_close(struct file *file)
{
	return 0;
}

int generic_read(struct file *file, char *buf, off_t offset)
{
	return 0;
}

int generic_write(struct file *file, char *buf, off_t offset)
{
	return 0;
}

static struct file_operations generic_fops = {
	.open = generic_open,
	.close = generic_close,
	.read = generic_read,
	.write = generic_write
};

int open(char *name)
{
	struct direntry *dent;
	struct file *file;

	dent = lookup_path(name);
	if (!dent)
		return -ENOENT;

	file = new_file();
	if (!file)
		return -ENOMEM;

	atomic_inc_u32(&dent->d_refcnt);

	file->f_inode = dent->d_inode;
	file->f_sb = dent->d_inode->i_sb;
	file->f_offset = 0;
	if (dent->d_inode->i_fops) {
		file->f_ops = dent->d_inode->i_fops;
		file->f_ops->open(file);
	} else
		file->f_ops = &generic_fops;

	return 0;
}


