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
#include <lib.h>
#include <klist0.h>
#include <galloc.h>
#include <device.h>
#include <super.h>
#include <inode.h>
#include <file.h>
#include <page_alloc.h>
#include <bitmap.h>
#include <khui/stat.h>
#include <khui/fcntl.h>
#include <thread.h>
#include "fs.h"

/*
 * FS file operations.
 */
struct file *new_file()
{
	struct file *file;

	file = gobj_alloc(struct file);
	if (!file)
		return NULL;

	KLIST0_INIT(&file->f_tlist);
	file->f_offset = 0;
	file->f_ops = NULL;
	file->f_sb = NULL;
	file->f_inode = NULL;
	return file;
}

void kill_file(struct file *file)
{
	gobj_free(file);
}

int generic_open(struct file *file)
{
	return 0;
}

int generic_close(struct file *file)
{
	return 0;
}

int generic_read(struct file *file, char *buf, size_t len)
{
	struct inode *inode = file->f_inode;
	struct page **pages;
	int npages;
	int r;
	int R; 		/* overall bytes read in */
	off_t pgstart_off;
	off_t pgstart;
	off_t pgsize;
	char *to;
	size_t left;

	/* make sure not to read beyond what's expected */
	if (file->f_offset + len > inode->i_size)
		len = inode->i_size - file->f_offset;
	left = len;

	pages = inode->i_map.i_pages;
	npages = inode->i_map.i_filled;
	pgstart = file->f_offset >> PAGE_SHIFT;
	pgstart_off = file->f_offset & (PAGE_SIZE - 1);
	pgsize = len >> PAGE_SHIFT;

	if (pgstart + pgsize > npages) {
		/* ok, we should do some reading */
		if (inode->i_map.read_page) {
			do {
				r = inode->i_map.read_page(pages[npages], npages);
				R += r;
				/* incomplete page read means
				 * it's the last one */
				if (!r || r < PAGE_SIZE)
					break;

			} while (npages++ < pgstart + pgsize);
		} else {
			/* 
			 * not having read_page method means all the pages
			 * should be there and since we're here, they are
			 * not
			 */
			bug();
		}

		/* ok, we read all the pages we wanted */
		inode->i_map.i_filled = npages;
	}

	/* do a corner case first */
	if (!pgsize) {
		memory_copy(buf,
			page_to_addr(pages[pgstart]) + pgstart_off, len);
		file->f_offset += len;
		return len;
	}

	r = pgstart;
	to = buf;

	do {
		memory_copy(to, page_to_addr(pages[r++]) + pgstart_off,
				MIN((size_t)PAGE_SIZE, left));
		left -= PAGE_SIZE;
		to += PAGE_SIZE;
		pgstart_off = 0;
	} while (r < npages);

	file->f_offset += len;

	return len;
}

int generic_write(struct file *file, char *buf, size_t offset)
{
	return 0;
}

int generic_readdir(struct file *file, struct udirentry *udents, int count)
{
	struct direntry *dent;
	struct klist0_node *d;
	int i = 0;

	if (!S_ISDIR(file->f_inode->i_mode))
		return -EINVAL;

	if (count <= 0)
		return -EINVAL;

	klist0_for_each(d, &file->f_inode->i_children) {
		dent = klist0_entry(d, struct direntry, d_siblings);

		memory_copy(udents->name, dent->d_name, FILENAME_MAX);

		if (i++ == count)
			break;

		udents++;
	}

	return i;
}

static struct file_operations generic_fops = {
	.open = generic_open,
	.close = generic_close,
	.read = generic_read,
	.write = generic_write,
	.readdir = generic_readdir,
};

int alloc_fd(struct thread *thread)
{
	int fd;
	
	/* this looks like an off-by-one in bitmap code vvv */
	fd = bitmap_seek_unset(thread->fdset, thread->last_fd + 1);
	if (fd == -1)
		return -EBADF; /* no more descriptors available */

	if (fd == thread->last_fd && thread->last_fd < FS_MAX_FDS)
		thread->last_fd++;

	/* mark fd as occupied */
	bitmap_bit_set(thread->fdset, fd);

	return fd;
}

void free_fd(struct thread *thread, int fd)
{
	bitmap_bit_clear(thread->fdset, fd);
}

/*
 * Open a file with a given operation mode
 * @name -- full pathname of a file
 * @mode -- how to open it
 */
int open(char *name, int flags, mode_t mode)
{
	struct direntry *dent;
	struct file *file;
	struct device *device;
	struct file_operations *fops = NULL;
	struct thread *thread = CURRENT();

	if (flags & O_CREAT) /* this will only work for rootfs */
		fs_insert_entry(name, mode, NODEV, NULL, 0);

	dent = lookup_path(name);
	if (!dent)
		return -ENOENT;

	/* if it's a device file, look it up and hook its fops to
	 * our file */
	if (dent->d_inode->i_dev != NODEV && !(flags & O_FORCE)) {
		device = get_device(dent->d_inode->i_dev);
		if (!device)
			return -ENODEV;

		fops = device->d_fops;
	}
	
	file = new_file();
	if (!file)
		return -ENOMEM;

	atomic_inc_u32(&dent->d_refcnt);
	atomic_inc_u32(&dent->d_inode->i_refcnt);

	klist0_append(&file->f_tlist, &thread->files);
	file->f_fd = alloc_fd(thread);
	file->f_inode = dent->d_inode;
	file->f_sb = dent->d_inode->i_sb;
	file->f_offset = 0;

	if (fops)
		file->f_ops = fops;
	else if (dent->d_inode->i_fops)
		file->f_ops = dent->d_inode->i_fops;
	else
		file->f_ops = &generic_fops;

	file->f_ops->open(file);

	return file->f_fd;
}

/*
 * Find file object by given descriptor
 */
struct file *fd2file(int fd)
{
	struct thread *thread = CURRENT();
	struct klist0_node *t;
	struct file *file;

	if (fd > thread->last_fd)
		return NULL;

	/* find a corresponding file */
	klist0_for_each(t, &thread->files) {
		file = klist0_entry(t, struct file, f_tlist);
		if (file->f_fd == fd)
			return file;
	}

	return NULL;
}

/*
 * Read len bytes from file fd buf. Bytes are read starting
 * from current offset.
 * @fd -- descriptor of an (opened) file to read from
 * @buf -- a buffer to read the data into
 * @len -- how many bytes you want to read
 * returns number of bytes actually read.
 */
int read(int fd, char *buf, size_t len)
{
	struct file *file;
	struct inode *inode;

	file = fd2file(fd);
	if (!file)
		return -EBADF;

	if (!file->f_inode)
		bug();

	inode = file->f_inode;
	if (file->f_offset == inode->i_size && !S_ISCHR(inode->i_mode))
		return 0;

	return file->f_ops->read(file, buf, len);
}

int write(int fd, char *buf, size_t len)
{
	struct file *file;
	struct inode *inode;

	file = fd2file(fd);
	if (!file)
		return -EBADF;

	if (!file->f_inode)
		bug();

	inode = file->f_inode;

	return file->f_ops->write(file, buf, len);
}

int close(int fd)
{
	struct file *file;

	file = fd2file(fd);
	if (!file)
		return -EBADF;
	release_inode(file->f_inode);
	free_fd(CURRENT(), fd);
	
	return file->f_ops->close(file);
}

/*
 * Read at most count entries from directory opened at fd into
 * user-allocated udents array.
 * @fd     -- file descriptor of directory to be read
 * @udents -- preallocated array of 'struct udirentry' records
 * @count  -- max number of entries to be read
 * returns actual number of read direntries or
 *         -EBADF for bad file descriptor (fd)
 *         -EINVAL if 'readdir' is not defined for this directory.
 */
int readdir(int fd, struct udirentry *udents, int count)
{
	struct file *file;
	struct inode *inode;

	file = fd2file(fd);
	if (!file)
		return -EBADF;

	if (!file->f_ops->readdir)
		return -EINVAL;

	if (!file->f_inode)
		bug();

	inode = file->f_inode;

	return file->f_ops->readdir(file, udents, count);
}

