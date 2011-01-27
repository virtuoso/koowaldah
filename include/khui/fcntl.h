/*
 * include/khui/fcntl.h
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

#ifndef __KHUI_FCNTL_H__
#define __KHUI_FCNTL_H__

/* NetBSD */
#define O_RDONLY        0x00000000      /* open for reading only */
#define O_WRONLY        0x00000001      /* open for writing only */
#define O_RDWR          0x00000002      /* open for reading and writing */
#define O_ACCMODE       0x00000003      /* mask for above modes */
#define O_NONBLOCK      0x00000004      /* no delay */
#define O_APPEND        0x00000008      /* set append mode */
#define O_SYNC          0x00000080      /* synchronous writes */
#define O_NOFOLLOW      0x00000100      /* don't follow symlinks on the last */
#define O_CREAT         0x00000200      /* create if nonexistent */
#define O_TRUNC         0x00000400      /* truncate to zero length */
#define O_EXCL          0x00000800      /* error if already exists */
#define O_FORCE         0x10000000      /* force open in ENODEV condition */

/*
 * Constants used for fcntl(2)
 */

/* command values */
#define F_DUPFD         0               /* duplicate file descriptor */
#define F_GETFD         1               /* get file descriptor flags */
#define F_SETFD         2               /* set file descriptor flags */
#define F_GETFL         3               /* get file status flags */
#define F_SETFL         4               /* set file status flags */

/* file descriptor flags (F_GETFD, F_SETFD) */
#define FD_CLOEXEC      1               /* close-on-exec flag */

#endif
