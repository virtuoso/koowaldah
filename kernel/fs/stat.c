/*
 * kernel/fs/stat.c
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
#include <namespace.h>
#include <error.h>

int stat(char *path, struct stat *buf)
{
	struct direntry *dent;

	dent = lookup_path(path);
	if (!dent)
		return -ENOENT;

	buf->st_ino  = dent->d_inode->i_ino;
	buf->st_dev  = dent->d_inode->i_dev;
	buf->st_mode = dent->d_inode->i_mode;
	buf->st_size = dent->d_inode->i_size;

	return 0;
}

