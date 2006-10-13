
/*
 * arch/i386/mm.c
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 * MM in KucaOS is a big fiction. I'll implement it later, and for now we 
 * have some functions who behave like real. In particular you can't allocate 
 * more than one page with pages_get(), and memory_release() does nothing.
 */

#include <kuca.h>
#include <mm.h>
#include <arch/paging.h>
#include <klist.h>
#include <bug.h>
#include <error.h>
#include <textio.h>


/* from arch/i386/kernel-*.lds */
extern char alloc_mem_start;
extern char alloc_mem_end;

#define ALLOC_BUF_START (void *) &alloc_mem_start
#define ALLOC_BUF_END (void *) &alloc_mem_end
#define MAX_ALLOCATIONS 1000

#define MAX_PAGES 10
#define FAKE_GETPAGES 1

typedef struct memory_block{
	        u8 * start;
	        size_t size;
} memory_block;

struct klist * page_list;
struct klist * used_page_list;

memory_block free_blocks[MAX_ALLOCATIONS / 2];
memory_block used_blocks[MAX_ALLOCATIONS + 1];

memory_block * last_free_block;
memory_block * last_used_block;


#if FAKE_GETPAGES
/*
static void show_list(struct klist * list)
{
	struct klist * tmp;

        if(!klist_is_empty(&list)){
                tmp = list;
                do{
                        kprintf("> %x\n", (u32) list->data);
                        klist_iter(&list);

                }while(tmp != list);
        }
}
*/

int page_is_allocated(u32 * page)
{

	struct klist * tmp;
	
        if(!klist_is_empty(&used_page_list)){
                tmp = used_page_list;
                do{
                        if(page == (u32 *)used_page_list->data)
				return 1;
                        klist_iter(&used_page_list);

                }while(tmp != used_page_list);
	}

	return 0;
	
}


u32 * pages_get(u32 num)
{
	struct klist * tmp;
	
	if(1 != num)
		return NULL;
/*
	kprintf("before allocation the page_list looks like the following:\n");
	show_list(page_list);
	kprintf("and the used_page_list:\n");
	show_list(used_page_list);
*/
	if(klist_is_empty(&page_list))
		return NULL;

	tmp = page_list;
	klist_remove(tmp, &page_list);
	kprintf("We've allocated a page, tmp->data = %x\n", *(u32**)tmp->data);
	klist_add(tmp, &used_page_list);
/*
	kprintf("after allocation page_list looks like:\n");
	show_list(page_list);
	kprintf("and the used_page_list:\n");
	show_list(used_page_list);
*/	
	return *(u32 **)tmp->data;
}

void pages_release(u32 * first_page){
	
	struct klist * tmp;
/*	
        kprintf("before release the page_list looks like the following:\n");
        show_list(page_list);
        kprintf("and the used_page_list:\n");
        show_list(used_page_list);
*/
	
	if(klist_is_empty(&used_page_list))
		bug();
	
	
	tmp = used_page_list;

	do{
		if(first_page == (u32 *)used_page_list->data){
			kprintf("Releasing page %x\n", (u32) first_page);
			tmp = used_page_list;	
			klist_remove(tmp, &used_page_list);
			klist_add(tmp, &page_list);
			return;
		}
		
		klist_iter(&used_page_list);

	}while(tmp != used_page_list);

#if 1	
	kprintf("used_page_list does not contain page %x\n", first_page);
	kprintf("used_page_list contains the following entryes:\n");

	tmp = used_page_list;
	
	do{
		kprintf("%x\n", (u32 *)used_page_list->data);
		klist_iter(&used_page_list);
	}while(tmp != used_page_list);	
	
#endif	
	bug();
	
}

void fake_getpage_init()
{
	int i;
	struct klist * tmp;
	u32 * page_array;

	used_page_list = NULL;
	
        page_array = memory_alloc(0x1000 * MAX_PAGES);
        if(!page_array)
                bug();

        for(i = 0; i < MAX_PAGES; i++){
                tmp = klist_new(sizeof(u32 *));
                if(!tmp)
                        bug();
                *(u32 *)tmp->data = page_array + (0x1000 * i);
		kprintf("[%x]", *(u32*)tmp->data);

                klist_add(tmp, &page_list);
//              kprintf("added entry, tmp->data = %x\n", (u32) tmp->data);
        }
/*
        if(!klist_is_empty(&page_list)){
                tmp = page_list;
                do{
                        kprintf("got entry, page_list->data = %x\n", (u32) page_list->data);
                        klist_iter(&page_list);
                }while(tmp != page_list);
        }
*/
	
}

#endif /* FAKE_GETPAGES */

/* this should be called before any allocation functions */
void mm_init() 
{
	paging_init();

	free_blocks[0].start = ALLOC_BUF_START;
	free_blocks[0].size = ALLOC_BUF_END - ALLOC_BUF_START;

	free_blocks[1].start = NULL;
	used_blocks[0].start = NULL;

	last_free_block = free_blocks; /* there is only one block now */
	last_used_block = NULL;
	
	fake_getpage_init();
}

#if CONFIG_DEBUG_MEMALLOC
void debug_print_blocks(){
	unsigned int i;
	
	rintf("\nFree blocks:\n");
		
	for(i = 0; NULL != free_blocks[i].start; i++)
		kprintf("%d. start = %d (0x%x), size = %d (0x%x)\n", i, 
						free_blocks[i].start, free_blocks[i].start, 
						free_blocks[i].size, free_blocks[i].size);
	
	kprintf("Used blocks:\n");

	for(i = 0; NULL != used_blocks[i].start; i++)
		kprintf("%d. start = %d (0x%x), size = %d (0x%x)\n", i, 
						used_blocks[i].start, used_blocks[i].start, 
						used_blocks[i].size, used_blocks[i].size);
}
#endif /* CONFIG_DEBUG_MEMALLOC */

void * memory_alloc(size_t n)
{
	unsigned int i, j;
	
	for(i = 0; NULL != free_blocks[i].start; i++){ 
		if(free_blocks[i].size >= n){ /* search for the first suitable block */
				
			for(j=0; NULL != used_blocks[j].start; j++);
			used_blocks[j].start = free_blocks[i].start;
			used_blocks[j].size = n;
			used_blocks[j + 1].start = NULL;
			last_used_block = used_blocks + j;
			
			if(free_blocks[i].size == n){ /* this block is now empty */
/* move the last block here. stays correct even if this was the last block. */
				free_blocks[i] = *last_free_block; 
				last_free_block->start = NULL;
				if(last_free_block != free_blocks)
						last_free_block--;
			}else{ /*there still is some memory here */
				free_blocks[i].size -= n;
				free_blocks[i].start += n;
			}
			return used_blocks[j].start;
		}
	}
	return NULL; /*there is no suitable memory block ;( */
}

#if 1 
void memory_release(void * pointer)
{

}

#else

void memory_release(void * pointer)
{
	unsigned int i, j;
	for(i = 0; NULL != used_blocks[i].start; i++){
		if(used_blocks[i].start == pointer){
			for(j = 0; NULL != free_blocks[j].start; j++){
				if(free_blocks[j].start + free_blocks[j].size == pointer){
					free_blocks[j].size += used_blocks[i].size;
					if(free_blocks[j].start + free_blocks[j].size == free_blocks[j + 1].start){
						free_blocks[j].size += free_blocks[j + 1].size;
						free_blocks[j + 1] = *last_free_block;
						last_free_block->start = NULL;
					}
					break;				
				}
			}
			if(NULL != last_used_block)
				used_blocks[i] = *last_used_block;
			else
				used_blocks[i].start = NULL;

			last_used_block->start = NULL;
		}
	}
}

#endif


