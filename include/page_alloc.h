#ifndef __PAGE_ALLOC_H__
#define __PAGE_ALLOC_H__

#include <kuca.h>
#include <klist0.h>

#define PAGE_SHIFT 12
#define PAGE_SIZE (1UL << PAGE_SHIFT)
#define MAX_ORDER 11


struct mem_zone;

struct page {
	u32 index;
	struct klist0_node list;
	union {
		u32 order;
	} private;
	struct mem_zone * zone;
};

struct page * alloc_pages(u32 flags, u32 order);
char * get_pages(u32 flags, u32 order);
char * page_to_addr(struct page * page);

#define alloc_page(flags) alloc_pages(flags, 0) 
#define get_page(flags) get_pages(flags, 0)

void free_pages(struct page * pg);

void print_alloc_info();

#endif /* __MM_H__ */



