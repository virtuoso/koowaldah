
/*
 * include/kqueue.h
 *
 * Copyright (C) 2006 Alexey Zaytsev
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
#ifndef __KQUEUE_H__
#define __KQUEUE_H__

#include <koowaldah.h>


struct kqueue_t *kqueue_create();
void kqueue_destroy(struct kqueue_t *q);

void kqueue_info(struct kqueue_t *q);

size_t kqueue_size(struct kqueue_t *q);

size_t queue_shrink(struct kqueue_t *q, size_t len);
size_t queue_shrink_tail(struct kqueue_t *q, size_t len);

int kqueue_push(struct kqueue_t *q, char *data, size_t len);
int kqueue_push_tail(struct kqueue_t *q, char *data, size_t len);

size_t kqueue_pull(struct kqueue_t *q, char *data, size_t len);
size_t kqueue_pull_tail(struct kqueue_t *q, char *data, size_t len);

size_t kqueue_peek(struct kqueue_t *q, char *data, size_t len);
size_t kqueue_peek_tail(struct kqueue_t *q, char *data, size_t len);

#endif /* __KQUEUE_H__ */

