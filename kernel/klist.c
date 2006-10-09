
/*
 * kernel/klist.c 
 * 
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 * This implements doubly linked list mechanism.
 *
 * 2005 Alexey Zaytsev -- original implementation
 * 2006 shisha         -- style fixes and a bit of
 * 	a redesign to avoid double allocation
 */

#include <kuca.h>
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

static inline void klist_pointer_add(struct klist *new, 
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

static inline void klist_pointer_remove(struct klist *prev_node, struct klist *next_node)
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

