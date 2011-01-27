/*
 * include/khui/fs.h
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
 *
 */
#ifndef __KHUI_FS_H__
#define __KHUI_FS_H__

#include <khui/types.h>

#define FILENAME_MAX (__size_t)256
#define FS_MAX_FDS 256

struct udirentry {
	char name[FILENAME_MAX];
};

struct stat {
	unsigned long st_ino;
	unsigned int st_dev;
	unsigned int st_mode;
	unsigned long st_size;
};

#endif

