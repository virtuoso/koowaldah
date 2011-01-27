/*
 * include/khui/message.h
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

#ifndef __KHUI_MESSAGE_H__
#define __KHUI_MESSAGE_H__

/*
 * Message flags
 */
#define MF_COPY     (1 << 0) /* simple copy */
#define MF_REMAP    (1 << 1) /* page remapping */
#define MF_BLOCK    (1 << 2) /* block caller */
#define MF_NOTIFY   (1 << 3) /* notify recipient */
#define MF_GRANU    (1 << 4) /* page granularity */
#define MF_REPLY    (1 << 5) /* 3-way messaging */

/* shorthand for send-recieve pattern */
#define MF_SENDRECV (MF_COPY | MF_BLOCK)
/* shorthand for send-recieve-reply pattern */
#define MF_SRR      (MF_SENDRECV | MF_REPLY)

#endif

