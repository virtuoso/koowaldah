
/*
 * include/klist0.h 
 *
 * Copyright (C) 2006 Alexander Shishkin
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
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *
 * Linux-alike doubly-linked list implementation, take 1
 * "full intrusion".
 *
 * Abstract. Apparently any kind of less intrusive list implementation
 * (as in klist.c+klist.h) is not quite suitable for:
 *   a) having the same object linked in more than one list (which is handy
 *      for many types of objects, including threads)
 *   b) linking objects that are allocated differently and/or separately
 *      compared to normal list objects
 *   c) linking static objects.
 * Intrusive lists are also much nicer with respect to cpu cache lines.
 * This is an attempt at implementation of an idea of linked lists as seen
 * in linux (include/linux/list.h).
 * Note1: this is not a copy of that in linux so be prepared for nasty bugs.
 *
 * Note2: a list should never ever contain invalid pointers as long as it
 *   exists (namely NULL, list poison etc). Instead, an empty list is a node
 *   linked to itselfs at both ends.
 */

#ifndef __KLIST0_H__
#define __KLIST0_H__

/*
 * Simple as ever. This structure gets embedded into node objects, one per
 * list.
 */
struct klist0_node {
	struct klist0_node *prev, *next;
};

#define KLIST0_DECLARE(list) \
	struct klist0_node list = { &(list), &(list) };

#define KLIST0_INIT(list) \
	do { (list)->next = (list)->prev = (list); } while (0);

/*
 * Add a node before the given one into the list
 * @node -- node to be added
 * @list -- node of the list
 */
static inline void klist0_prepend(struct klist0_node *node,
		struct klist0_node *list)
{
	node->next = list;
	node->prev = list->prev;
	list->prev->next = node;
	list->prev = node;
}

/*
 * Add a node after the given one into the list
 * @node -- node to be added
 * @list -- node of the list
 */
static inline void klist0_append(struct klist0_node *node,
		struct klist0_node *list)
{
	node->next = list->next; \
	node->prev = list; \
	list->next->prev = node; \
	list->next = node; \
	
}

/*
 * Remove a node from it's list and relink it so that it be included
 * in a valid empty list, as we're always consistent
 * @node -- node to be removed
 * Has no effect on an empty list: does no harm, but makes no sense.
 */
static inline void klist0_unlink(struct klist0_node *node)
{
	node->prev->next = node->next; \
	node->next->prev = node->prev; \
	node->next = node; \
	node->prev = node; \
}

/*
 * Access the container of the list node
 * @node -- node of the list
 * @type -- type of the container
 * @list -- member of container's structure that is this node
 */
#define klist0_entry(node, type, list) \
	( (type *)((char *)node - offsetof(type, list)) )

/*
 * Iterate through list nodes.
 * @node -- roaming node pointer
 * @list -- head of the list
 */
#define klist0_for_each(node, list) \
	for (node = (list)->next; node != (list); node = node->next)

/*
 * Check is list is empty (next/prev point to itself).
 * @list -- the list
 */
static inline int klist0_empty(struct klist0_node *list)
{
	return list->next == list;
}

#endif

