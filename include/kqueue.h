
/*
 * include/kqueue.h
 *
 * Copyright (C) 2006 Alexey Zaytsev
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
 * 3. Neither the name of the Koowaldah developers nor the names of theyr
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

