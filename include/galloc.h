
/*
 * include/galloc.h
 *
 * Copyright (C) 2006 Alexey Zaytsev
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

#ifndef __GALLOC_H__
#define __GALLOC_H__

u32 * galloc(u32 flags, size_t size);
void gfree(u32 * chunk);

#define gobj_alloc(__t)                  \
	((__t *) galloc(0, sizeof(__t)))
#define gobj_free(__p) (gfree((u32 *)__p))

#endif /* __GALLOC_H__ */
