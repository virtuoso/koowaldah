/*
 * include/virtfs.h
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

#ifndef __VIRTFS_H__
#define __VIRTFS_H__

/* forward */
struct virtfs_entry;

/*
 * Hooks to read from/write to virtual files and directories
 */
struct virtfs_operations {
	/* --- directories */
	/*
	 * readdir: overrides default behavior of readdir;
	 * should provide 'udents' buffer (with 'count entries) to user.
	 * readdir is called on directories (S_ISDIR(ve_mode) != 0)
	 * to obtain a list of its children
	 */
	int (*readdir)(struct udirentry *udents, int count);
	/*
	 * lookup: check if 'name' is a valid name for an entry
	 * in this directory (called on directories)
	 */
	struct virtfs_entry *(*lookup)(char *name);

	/* --- generic */
	/*
	 * open: a hook which is called when an inode associated
	 * with this virtfs_entry is open()ed
	 */
	int (*open)(struct file *file, char *name);
};

/*
 * Dummy virtfs_operations, use for static entries
 */
extern struct virtfs_operations veops_nop;

/*
 * Entry in a virtual filesystem
 */
struct virtfs_entry {
	char                ve_name[12];  /* entry name                      */
	mode_t              ve_mode;      /* file mode                       */
	dev_t               ve_dev;       /* device number, if any           */
	u32                 ve_flags;     /* entry flags                     */
	struct inode        *ve_inode;    /* associated inode: a private
					     matter of kernel/fs/virtfs.c    */
	struct virtfs_entry *ve_child;    /* child virtfs_entry              */
	struct virtfs_operations *ve_ops; /* operations, see above           */
	struct klist0_node  ve_sibling;   /* list of siblings, used for static
					     entries                         */
};

extern struct virtfs_entry virtfs_tree;
extern struct virtfs_entry _dot;
extern struct virtfs_entry virtfs_srv;

/*
 * Helper macros
 */

/*
 * SYN: VE_ENTRY(symbol_name, "quoted_name", mode, device, operations);
 * May be used in arrays if you feel you have to have those.
 */
#define VE_ENTRY(__n, __qn, __md, __dev, __op)                               \
	{                                                                    \
		.ve_name = __qn,                                             \
		.ve_mode = __md,                                             \
		.ve_dev  = __dev,                                            \
		.ve_ops    = __op,                                           \
		.ve_child    = NULL,                                         \
		.ve_inode    = NULL,                                         \
		.ve_sibling  = { &((__n).ve_sibling), &((__n).ve_sibling) }, \
	}

/*
 * SYN: VE_DECLARE(symbol_name, "quoted_name", mode, device, operations);
 * Declares a symbol with name 'symbol_name', globally visible. You want this
 * once.
 */
#define VE_DECLARE(__n, __qn, __md, __dev, __op)                             \
	struct virtfs_entry __n = VE_ENTRY(__n, __qn, __md, __dev, __op)

/*
 * SYN: VE_DECLARE_STATIC(symbol_name, "quoted_name",
 *                        mode, device, operations);
 * Declares a static symbol. Useful for your little part of our virfs.
 */
#define VE_DECLARE_STATIC(__n, __qn, __md, __dev, __op)                      \
	static VE_DECLARE(__n, __qn, __md, __dev, __op)

/*
 * Make one entry a child of another
 */
#define VE_ADD_SUBTREE(__t, __s)                                             \
	do { (__t)->ve_child = __s; } while (0)

/*
 * Make two entries sit side-by-side
 */
#define VE_ADD_SIBLING(__t, __s)                                             \
	do {                                                                 \
		klist0_append(&(__s)->ve_sibling, &(__t)->ve_sibling);       \
	} while (0)

/*
 * Parts of big virtfs
 */
void __init virtfs_proc_init();
void __init virtfs_dev_init();

struct virtfs_entry *new_ve();
void free_ve(struct virtfs_entry *ve);

#endif
