/*
 * kernel/fs/message.c
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

	/*kprintf("# sending message [%s] from %d to %d, flags %x\n",
			buf, CURRENT()->pid, recp, flags);*/
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

	if (!(flags & MF_BLOCK)) {
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
				mem_area_alloc_new(map, USERMEM_MBOX, 0,
						MMA_GROWS);
			if (!map->m_mma[4])
				return -ENOMEM;
		}

		/* add a page to recipient's 'mbox' mma */
		map_page(thread->map, thread->map->m_mma[4]->m_end,
				(u32)ppg, thread->map->m_mma[4]->m_prot);
		mem_area_add_page(thread->map->m_mma[4], pg);

		msg->m_buf = (char *)pg->virt;
	} else
		msg->m_buf = (char *)__virt2phys((u32)buf);

	/* initialize struct message fields */
	KLIST0_INIT(&msg->m_mbox);
	msg->m_sender = me->pid;
	msg->m_flags  = flags;
	msg->m_len    = len;
	klist0_prepend(&msg->m_mbox, &thread->mbox);
	tq_init(&msg->m_tq);

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
			break;
		}
	}

	if (flags & MF_BLOCK)
		scheduler_dequeue(&msg->m_tq);

	scheduler_yield();

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
	struct message *msg = NULL;
	struct klist0_node *tmp;
	void *phys;
	size_t ret;

	/* patterns to be implemented */
	if (!sender || !len || !*buf)
		return -ENOSYS;

	/* make sure we have something in mbox */
	if (klist0_empty(&me->mbox)) {
sleep:
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
		goto sleep;

	memory_copy(*buf, msg->m_buf, msg->m_len);
	if (!(msg->m_flags & MF_BLOCK)) {
		/* shrink the heap */
		phys = (void *)__virt2physpg((u32)msg->m_buf);
		unmap_page(me->map, (u32)msg->m_buf);
		mem_area_remove_page(me->map->m_mma[4], addr_to_page(phys));
		put_pages(phys);
	}

	if (msg->m_flags & MF_BLOCK && !klist0_empty(&msg->m_tq.threads))
		scheduler_enqueue(&msg->m_tq);

	ret = msg->m_len;

	/* remove the message */
	klist0_unlink(&msg->m_mbox);
	slice_free(msg, msg_pool);

	return ret;
}

