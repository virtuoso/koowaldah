
/*
 * kernel/klist.c 
 * 
 * Copyright (C) 2005 Alexey Zaytsev
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
 * 3. Neither the name of the Koowaldah developers nor the names of their 
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
 */


#include <koowaldah.h>
#include <mm.h>
#include <klist.h>

/*
 * Create a new list (or a node) for objects of given size.
 * @size -- sizeof of what is held in the list nodes.
 */
struct klist *klist_new(u32 size)
{
	struct klist *list;

	list = memory_alloc(sizeof(struct klist) + size);
	if (list)
		list->next = list->prev = list;

	return list;
}

static __inline void klist_pointer_add(struct klist *new, 
		struct klist *prev_node, struct klist *next_node)
{
	next_node->prev = new;
	new->next = next_node;
	new->prev = prev_node;
	prev_node->next = new;
}

/*
 * Append a (previously allocated) node to the list.
 * @new -- node ptr
 * @list -- the list.
 */
void klist_add(struct klist *new, struct klist **list)
{
	if (*list)
		klist_pointer_add(new, *list, (*list)->next);
	else
		*list = new;
}

/*
 * Prepend a (previously allocated) node to the list.
 * @new -- node ptr
 * @list -- the list.
 */
void klist_add_tail(struct klist *new, struct klist **list)
{
	if (*list)
		klist_pointer_add(new, (*list)->prev, *list);
	else
		*list = new;
}

static __inline void klist_pointer_remove(struct klist *prev_node, struct klist *next_node)
{
	next_node->prev = prev_node;
	prev_node->next = next_node;
}

/*
 * Remove a node from a list.
 * A node can be reused/relinked afterwards.
 * @node -- node to be removed
 * @list -- guess what
 */
void klist_remove(struct klist *node, struct klist **list)
{

	if (node == node->next)
		*list = NULL;
	else {
		if (*list == node)
			*list = node->next;
		
		klist_pointer_remove(node->prev, node->next);
		node->next = node->prev = node;
	}
}

/*
 * Remove and deallocate a list node. Practically kill it.
 * @node -- node to be removed
 * @list -- guess what
 */
void klist_del(struct klist *node, struct klist **list)
{
	if (node == node->next)
		*list = NULL;
	else {
		if (*list == node)
			*list = node->next;
		klist_pointer_remove(node->prev, node->next);
	}

	memory_release(node);
}

