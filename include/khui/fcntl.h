/*
 * include/khui/fcntl.h
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
