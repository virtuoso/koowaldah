/*
 * include/message.h
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

/*
 * Message passing
 */
#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <koowaldah.h>
#include <klist0.h>
#include <khui/message.h>
#include <thread.h>

struct message {
	struct klist0_node  m_mbox;   /* thread's message list link */
	pid_t               m_sender; /* sender's pid               */
	u32                 m_flags;  /* message's flags            */
	char                *m_buf;   /* message buffer             */
	size_t              m_len;    /* message length             */
	struct thread_queue m_tq;     /* thread queue to sleep on   */
};

void __init messaging_init();
int msg_send(pid_t recp, char *buf, size_t len, u32 flags);
int msg_retrieve(pid_t sender, char **buf, size_t len, u32 flags);

#endif

