
/*
 * include/slice.h
 *
 * Copyright (C) 2005 Alexey Zaytsev
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 */

#ifndef __SLICE_H__
#define __SLICE_H__

struct slice_pool {
	struct klist0_node slice_list;
	
	/* Pages with some free space. */
	struct klist0_node pages_active; 
	
	/* Pages with no space left. */
	struct klist0_node pages_inactive;

	u32 obj_size;
	u32 flags;
};

void slice_pool_info(struct slice_pool *);
struct slice_pool *slice_pool_create(u32 flags, int obj_size);
void slice_pool_shrink(struct slice_pool *pool);
void slice_pool_recycle(struct slice_pool *pool);
void *slice_alloc(struct slice_pool *pool);
void slice_free(void *slice, struct slice_pool *pool);

#endif /* __SLICE_H__ */

