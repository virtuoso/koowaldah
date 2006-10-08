
/*
 * include/mm.h
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#ifndef __MM_H__
#define __MM_H__

#define PAGE_SIZE 4096

#if CONFIG_DEBUG_MEMALLOC
void debug_print_blocks();
#endif /* CONFIG_DEBUG_MEMALLOC */		

void mm_init();
void * memory_alloc(size_t n);
void memory_release(void * pointer);

/* get num consecutive pages */
u32 * pages_get(u32 num);
int page_is_allocated(u32 * page);
void pages_release(u32 * first_page);


#endif /* __MM_H__ */

