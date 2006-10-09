
/*
 * include/klist.h 
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 * Doubly-linked list implementation, take 1.5.
 */

#ifndef __KLIST_H__
#define __KLIST_H__

/*
 * A node (and silently, a list).
 * This is also a container to underlying data.
 */
struct klist {
	struct klist * next;
	struct klist * prev;
	//void * data;
	char data[0];
};

/*
 * Accessor for data held within the node.
 * @node -- list node ptr
 * @type -- type of data within the node.
 */
#define klist_node(node, type) \
		((type *)((char *)node + 2*sizeof(struct klist)))

#if 0
#define klist_do_for_each(pos, list) 	\
	if(*list)			\
		pos = *list;
		while(){
			
		}
//		for( pos = (*list)->next; pos != *list; pos = pos->next )

#define klist_do_for_each_reverse(pos, list)	\
	if(*list) 			\
		for( pos = (*list)->next; pos != *list; pos = pos->next )

#endif

#define klist_is_empty(list) \
	!(*list)

#define klist_iter(list) \
	(*list) = (*list)->next;

#define klist_iter_backeards(list) \
	(*list) = (*list)->prev;

struct klist *klist_new(u32 size);

void klist_add(struct klist *new, struct klist **list);

void klist_add_tail(struct klist *new, struct klist **list);

void klist_remove(struct klist *node, struct klist **list);

void klist_del(struct klist *node, struct  klist **list);

#endif /* __KLIST_H__ */
