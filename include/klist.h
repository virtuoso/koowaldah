
/*
 * include/klist.h 
 *
 * Copyright (C) 2005 Alexey Zaytsev
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
