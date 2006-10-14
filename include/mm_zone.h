#ifndef __MM_ZONE_H__
#define __MM_ZOME_H__

#include <page_alloc.h>
#include <klist0.h>

struct mem_zone {
	u32 * base;
	u32 total_pages;
	u32 free_pages;
	struct klist0_node alloc_levels[MAX_ORDER];
	struct klist0_node list;
};

struct mem_info {
	struct klist0_node zone_list;
};

void mem_zone_init(struct mem_zone * zone);


#endif /* __MM_ZONE_H__ */
