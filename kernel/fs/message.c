/*
 * kernel/fs/message.c
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

#include <koowaldah.h>
#include <message.h>
#include <textio.h>
#include <slice.h>
#include <mem_area.h>
#include <mm_zone.h>
#include <thread.h>
#include <scheduler.h>
#include <error.h>
#include <lib.h>
#include <bug.h>

static struct slice_pool *msg_pool;

/* list of processes sleeping on msg_retrieve */
static struct thread_queue sleepers;

void __init messaging_init()
{
	msg_pool = slice_pool_create(0, sizeof(struct message));
	bug_on(!msg_pool);
	tq_init(&sleepers);
}

/*
 * Send a message to a proccess
 * @recp   -- recipient process' pid
 * @buf    -- message buffer
 * @len    -- length of the message
 * @flags  -- message's flags (see message.h for details)
 * returns -- the length of the message recieved
 */
int msg_send(pid_t recp, char *buf, size_t len, u32 flags)
{
	struct thread *me = CURRENT();
	struct thread *thread; /* recipient */
	struct klist0_node *tmp;
	struct thread *ttmp;
	struct mapping *map;
	struct message *msg;
	void *ppg;
	struct page *pg;

	kprintf("# sending message [%s] from %d to %d, flags %x\n",
			buf, CURRENT()->pid, recp, flags);
	/* these patterns are to be implemented */
	if (flags & (MF_REMAP|MF_NOTIFY|MF_GRANU))
		return -ENOSYS;

	if (len > PAGE_SIZE)
		return -ENOSYS;

	thread = thread_get(recp);
	if (!thread)
		return -EINVAL;

	msg = slice_alloc(msg_pool);
	if (!msg)
		return -ENOMEM;

	ppg = get_page(ZONE_USER);
	if (!ppg) {
		slice_free(msg, msg_pool);
		return -ENOMEM;
	}
	
	memory_copy(ppg, buf, len);

	pg = addr_to_page(ppg);
	map = thread->map;

	/* check if thread has 'mbox' mma and create if necessary */
	if (map->m_nmma == 4) {
		map->m_mma[map->m_nmma++] =
			mem_area_alloc_new(map, USERMEM_MBOX, 0, MMA_GROWS);
		if (!map->m_mma[4])
			return -ENOMEM;
	}
	
	/* add a page to recipient's 'mbox' mma */
	map_page(thread->map, thread->map->m_mma[4]->m_end,
			(u32)ppg, thread->map->m_mma[4]->m_prot);
	mem_area_add_page(thread->map->m_mma[4], pg);

	/* initialize struct message fields */
	KLIST0_INIT(&msg->m_mbox);
	msg->m_sender = me->pid;
	msg->m_flags  = flags;
	msg->m_buf    = (char *)pg->virt;
	msg->m_len    = len;
	klist0_prepend(&msg->m_mbox, &thread->mbox);

	/* check if our recipient is on the sleepers queue */
	klist0_for_each(tmp, &sleepers.threads) {
		ttmp = klist0_entry(tmp, struct thread, krunq);
		if (thread == ttmp) {
			/* and wake it */
			struct thread_queue tq;

			tq_init(&tq);
			spin_lock(&sleepers.lock);
			__tq_remove_thread(thread);
			spin_unlock(&sleepers.lock);

			tq_insert_head(thread, &tq);
			scheduler_enqueue(&tq);
			scheduler_yield(); /* XXX */
			break;
		}
	}

	return len;
}

/*
 * Retrieve a message from a process
 * @sender -- sending process' pid, 0 for kernel
 * @buf    -- ptr to a variable that will hold the address of message buffer
 * @len    -- length of the message (should be 0 if the buffer address
 *            is to be obtained in the course of retrieving
 * @flags  -- message's flags (see message.h for details)
 * returns -- the length of the message recieved
 */
int msg_retrieve(pid_t sender, char **buf, size_t len, u32 flags)
{
	struct thread *me = CURRENT();
	struct message *msg;
	struct klist0_node *tmp;
	void *phys;

	/* patterns to be implemented */
	if (!sender || !len || !*buf)
		return -ENOSYS;

	/* make sure we have something in mbox */
	if (klist0_empty(&me->mbox)) {
		if (flags & MF_BLOCK) {
			scheduler_dequeue(&sleepers);

			scheduler_yield();
		} else
			return -EAGAIN;
	}

	/* find the first message from this sender */
	klist0_for_each(tmp, &me->mbox) {
		msg = klist0_entry(tmp, struct message, m_mbox);

		if (msg->m_sender == sender)
			break;
	}

	/* we should more likely go back to sleep here if flags & MF_BLOCK */
	if (msg->m_sender != sender)
		return 0;

	memory_copy(*buf, msg->m_buf, msg->m_len);

	/* shrink the heap */
	phys = (void *)__virt2physpg((u32)msg->m_buf);
	unmap_page(me->map, (u32)msg->m_buf);
	mem_area_remove_page(me->map->m_mma[4], addr_to_page(phys));
	
	/* remove the message */
	klist0_unlink(&msg->m_mbox);
	slice_free(msg, msg_pool);

	return 0;
}

