/*
 * kernel/fs/mpipe.c
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
#include <lib.h>
#include <bug.h>
#include <file.h>
#include <namespace.h>
#include <thread.h>
#include <scheduler.h>
#include <virtfs.h>
#include <page_alloc.h>
#include <slice.h>
#include <khui/stat.h>
#include <error.h>
#include <mutex.h>

/* list of processes sleeping on read() */
static struct slice_pool *mpipe_pool;

struct mpipe_ctl {
	char *buf;
	size_t len;
	u32 state;
	struct thread_queue tq;
	struct mutex mutex;
};

enum {
	MC_CLOSED = 0,
	MC_OPENED,
	MC_READ_FROM,
	MC_WRITTEN_TO
};

static int mpipe_read(struct file *file, char *buf, size_t len)
{
	struct mpipe_ctl *mc = file->f_inode->i_ctl;

	/* TODO: we need a semaphore here */

	mc->buf = (char *)__virt2phys((u32)buf);
	mc->len = len;

	scheduler_dequeue(&mc->tq);
	scheduler_yield();
	mc->state = MC_READ_FROM;

	return mc->len;
}

static int mpipe_write(struct file *file, char *buf, size_t len)
{
	struct mpipe_ctl *mc = file->f_inode->i_ctl;
	size_t done = 0;
	size_t left = len;

	mutex_lock(&mc->mutex);

	do {
		memory_copy(mc->buf, buf, MIN(mc->len, left));
		done += mc->len;
		buf  += mc->len;
		left -= mc->len;
		mc->state = MC_WRITTEN_TO;

		/* wake the reader */
		scheduler_enqueue(&mc->tq);
		while (mc->state != MC_READ_FROM)
			scheduler_yield();
	} while (done < len);

	mutex_unlock(&mc->mutex);

	return len;
}

static struct file_operations mpipe_fops = {
	.open    = generic_open,
	.close   = generic_close,
	.read    = mpipe_read,
	.write   = mpipe_write,
	.readdir = NULL,
};

/*
 * Create and open a communication pipe
 * name -- name of a pipe to be created; its absolute pathname
 *         will be /koowaldah/srv/<name>
 * returns fd number upon success, otherwise
 *         -EBUSY  == entry with this name already exists
 *         -ENOMEM == unable to allocate enough memory
 */
int mpipe_open(char *name)
{
	struct virtfs_entry *ve;
	struct file *file;
	struct direntry *dent;
	char pathname[FILENAME_MAX];
	size_t name_len = kstrlen_(name);
	struct thread *thread;
	struct mpipe_ctl *mc;

	/* make sure the entry does not yet exist */
	/* XXX: we should be using srv's inode's ->lookup() instead,
	 * that is, we should reliably make sure the inode's there */
	snprintf(pathname, FILENAME_MAX, "/koowaldah/srv/%s", name);
	dent = lookup_path(pathname);
	if (dent) {
		release_direntry(dent);
		return -EBUSY;
	}

	/* setup a virtfs_entry for the pipe */
	ve = new_ve();
	if (!ve)
		return -ENOMEM;

	memory_copy(ve->ve_name, name, name_len);
	ve->ve_mode = S_IFIFO;
	ve->ve_dev = NODEV;
	ve->ve_ops = &veops_nop;

	/* this is not in open() call path, so we have to do it manually */
	file = new_file();
	if (!file) {
		free_ve(ve);
		return -ENOMEM;
	}

	mc = slice_alloc(mpipe_pool);
	if (!mc) {
		free_ve(ve);
		kill_file(file);
		return -ENOMEM;
	}

	/* insert our new pipe into virtfs directory */
	VE_ADD_SIBLING(&virtfs_srv, ve);

	thread = CURRENT();
	dent = lookup_path(pathname);

	/* this part could be taken out, it's fairly close to what open()
	 * does to 'struct file' */
	klist0_append(&file->f_tlist, &thread->files);
	file->f_fd = thread->last_fd++;
	file->f_inode = dent->d_inode;
	file->f_sb = dent->d_inode->i_sb;
	file->f_offset = 0;
	file->f_ops = &mpipe_fops;
	file->f_inode->i_size = 1; /* something's broken up there */
	file->f_inode->i_fops = &mpipe_fops; /* for clients */
	file->f_inode->i_ctl = mc;

	/* initialize the control structure */
	tq_init(&mc->tq);
	mc->len = 0;
	mc->buf = NULL;
	mc->state = MC_OPENED;
	mutex_init(&mc->mutex);

	return file->f_fd;
}

int mpipe_open2(int fd)
{
	struct file *file;
	struct mpipe_ctl *mc;

	mc = slice_alloc(mpipe_pool);
	if (!mc)
		return -ENOMEM;

	file = fd2file(fd);
	if (!file)
		return -EBADF;

	file->f_ops = &mpipe_fops; /* for successive calls on this fd */
	file->f_inode->i_size = 1; /* something's broken up there */
	file->f_inode->i_fops = &mpipe_fops; /* for clients */
	file->f_inode->i_ctl = mc;

	/* initialize the control structure */
	tq_init(&mc->tq);
	mc->len = 0;
	mc->buf = NULL;
	mc->state = MC_OPENED;
	mutex_init(&mc->mutex);

	return 0;
}

void __init fs_mpipe_init()
{
	mpipe_pool = slice_pool_create(0, sizeof(struct mpipe_ctl));
	bug_on(!mpipe_pool);
}

