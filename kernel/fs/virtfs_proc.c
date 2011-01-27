/*
 * kernel/fs/virtfs_proc.c
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
#include <namespace.h>
#include <khui/stat.h>
#include <klist0.h>
#include <lib.h>
#include <virtfs.h>
#include <scheduler.h>
#include <galloc.h>

static int virtfs_proc_readdir(struct udirentry *udents, int count)
{
	struct klist0_node *tmp;
	struct thread *t;
	int c = 0;

	klist0_for_each(tmp, &thread_list.threads) {
                t = klist0_entry(tmp, struct thread, kthreads);

		snprintf(udents->name, FILENAME_MAX, "[task:%d]", t->pid);
		/*memory_copy(udents->name, t->name,
				kstrlen(t->name) + 1);*/
		udents++;
		c++;
	}

	return c;
}

static struct virtfs_entry *virtfs_proc_lookup(char *name)
{
	struct virtfs_entry *ve;
	/*pid_t pid = strtoul(&name[1]);

	kprintf("# looking up proc/%s, pid=%d\n", name, pid);*/

	/* let's assume 'name' exists :) */
	ve = (struct virtfs_entry *)galloc(0, sizeof(struct virtfs_entry)); /*XXXXXX!*/
	memory_set(ve, 0, sizeof(struct virtfs_entry));
	memory_copy(ve->ve_name, name, kstrlen(name) + 1);
	ve->ve_mode = S_IFREG;
	ve->ve_dev = NODEV;
	ve->ve_ops = &veops_nop;
	KLIST0_INIT(&ve->ve_sibling);
	return ve;
}

static struct virtfs_operations veops_proc = {
	.readdir = virtfs_proc_readdir,
	.lookup  = virtfs_proc_lookup,
};

VE_DECLARE_STATIC(proc, "proc", S_IFDIR, NODEV, &veops_proc);

void __init virtfs_proc_init()
{
	VE_ADD_SIBLING(&_dot, &proc);
}

