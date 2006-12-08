
/*
 * kernel/lib/queue.c
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
 *
 */

#include <koowaldah.h>
#include <textio.h>
#include <klist0.h>
#include <arch/asm.h>
#include <page_alloc.h>
#include <slice.h>
#include <lib.h>
#include <error.h>
#include <bug.h>

struct kqueue_t {
	struct klist0_node pages;
	size_t count;
};

struct kqueue_page {
	struct klist0_node pages;
	char *base;
	int count;
};

#define MAX_COUNT (PAGE_SIZE - sizeof(struct kqueue_page))

struct slice_pool *kqueue_sp;

void kqueue_init()
{
	kqueue_sp = slice_pool_create(0, sizeof(struct kqueue_t));
	bug_on(!kqueue_sp);
}

struct kqueue_t *kqueue_create()
{
	struct kqueue_t *queue;

	queue = (struct kqueue_t *)slice_alloc(kqueue_sp);
	if (queue) {
		queue->count = 0;
		KLIST0_INIT(&queue->pages);
	}

	return queue;
}


void kqueue_destroy(struct kqueue_t *q)
{
	struct kqueue_page *page;
	struct klist0_node *tmp;

	klist0_for_each(tmp, &q->pages) {
		page = klist0_entry(tmp, struct kqueue_page, pages);	
		put_page((char * ) page);
	}

	slice_free(q, kqueue_sp);
}

void kqueue_info(struct kqueue_t *q)
{
	struct klist0_node *tmp;
	struct kqueue_page *page;

	kprintf("Queue size = %d,  pages:", q->count);
	klist0_for_each(tmp, &q->pages) {
		page = klist0_entry(tmp, struct kqueue_page, pages);
		kprintf(" [0x%x]: base = 0x%x, count = %d",
			page, page->base, page->count);
		
	}
	kprintf(".\n");
}

size_t kqueue_size(struct kqueue_t *q)
{
	return q->count;
}
size_t queue_shrink(struct kqueue_t *q, size_t len)
{
	bug();
	return 0;
}

size_t queue_shrink_tail(struct kqueue_t *q, size_t len)
{
	bug();
	return 0;
}

int kqueue_push(struct kqueue_t *q, char *data, size_t len)
{
	struct kqueue_page *tmp;

	int t;
	size_t pushed  = 0;
	
	if (!klist0_empty(&q->pages)) {
		tmp = klist0_entry(q->pages.next, struct kqueue_page, pages);
		if (tmp->count < MAX_COUNT) {
			if (len <= MAX_COUNT - tmp->count) {
				memory_copy(tmp->base + tmp->count, data, len);
				tmp->count += len;
				q->count += len;
				return 0;
			} else {
				t = MAX_COUNT - tmp->count;
				memory_copy(tmp->base + tmp->count, data, t);
				tmp->count = MAX_COUNT;
				q->count += t;
				pushed = t;
				len -= t;
			}
		}
	}

	/* This chunk didn't fit in the existing queue, so
	 * add a page or two. */
	do {	
		tmp = (struct kqueue_page *) get_page(0);
		if (!tmp) {
			if (pushed)
				queue_shrink(q, pushed);
			return -ENOMEM;
		}

		KLIST0_INIT(&tmp->pages);
		tmp->base = ((char *)tmp) + sizeof(struct kqueue_page);
		klist0_append(&tmp->pages, &q->pages);

		t = MIN(len, (size_t) MAX_COUNT);
		memory_copy(tmp->base, data, t);
		
		tmp->count = t;
		q->count += t;
		pushed += t;
		len -= t;
		
	} while (len);
	
	return 0;
}

int kqueue_push_tail(struct kqueue_t q, char *data, size_t len)
{
	bug();
	return -1;
}

size_t kqueue_pull(struct kqueue_t *q, char *data, size_t len)
{
	bug();
	return -1;
}

size_t kqueue_pull_tail(struct kqueue_t *q, char *data, size_t len)
{
	struct kqueue_page * tmp;
	size_t pulled = 0;

	while (!klist0_empty(&q->pages)) {
		tmp = klist0_entry(q->pages.prev, struct kqueue_page, pages);
		if (len < tmp->count) { 
			memory_copy(data + pulled, tmp->base, len);
			tmp->count -= len;
			tmp->base += len;
			q->count -= len;
			return len;
		} else {
			memory_copy(data + pulled, tmp->base, tmp->count);
			q->count -= tmp->count;
			pulled += tmp->count;
			len -= tmp->count;
			
			klist0_unlink(q->pages.prev);
			put_page((char *) tmp);
		}
	}

	return pulled;
}

size_t kqueue_peek(struct kqueue_t *q, char *data, size_t len)
{
	bug();
	return -1;
}

size_t kqueue_peek_tail(struct kqueue_t *q, char *data, size_t len)
{
	bug();
	return -1;
}
