
/*
 * kernel/klist.c 
 * 
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#include <kuca.h>
#include <mm.h>
#include <klist.h>

struct klist * klist_new()
{
	struct klist * list;

	list = memory_alloc(sizeof(struct klist));
	if(list){
		list->next = list->prev = list;
		list->data = NULL;
	}

	return list;
}

static inline void klist_pointer_add(struct klist * new, 
		struct klist * prev_node, struct klist * next_node)
{
	next_node->prev = new;
	new->next = next_node;
	new->prev = prev_node;
	prev_node->next = new;
}


void klist_add(struct klist * new, struct klist ** list)
{
	if(*list)
		klist_pointer_add(new, *list, (*list)->next);
	else
		*list = new;
}

void klist_add_tail(struct klist * new, struct klist ** list)
{
	if(*list)
		klist_pointer_add(new, (*list)->prev, *list);
	else
		*list = new;
}

static inline void klist_pointer_remove(struct klist * prev_node, struct klist * next_node)
{
	next_node->prev = prev_node;
	prev_node->next = next_node;
}

void klist_remove(struct klist * node, struct klist ** list)
{

	if(node == node->next){
		*list = NULL;
	}else{
		if(*list == node)
			*list = node->next;
		
		klist_pointer_remove(node->prev, node->next);
		node->next = node->prev = node;
	}
}

void klist_del(struct klist * node, struct klist ** list)
{
	if(node == node->next){
		*list = NULL;
	}else{
		if(*list == node)
			*list = node->next;
		klist_pointer_remove(node->prev, node->next);
	}

	memory_release(node);
}


