/*
 * kernel/fs/virtfs_proc.c
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

