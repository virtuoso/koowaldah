
/*
 * include/klist.h 
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#ifndef __KLIST_H__
#define __KLIST_H__

struct klist {
	struct klist * next;
	struct klist * prev;
	void * data;
};

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

struct klist * klist_new();

void klist_add(struct klist * new, struct klist ** list);

void klist_add_tail(struct klist * new, struct klist ** list);

void klist_remove(struct klist * node, struct klist ** list);

void klist_del(struct klist * node, struct  klist ** list);

#endif /* __KLIST_H__ */
