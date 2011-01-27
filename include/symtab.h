/*
 * include/symtab.h
 *
 * Copyright (C) 2007 Alexander Shishkin
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
 */

#ifndef __SYMTAB_H__
#define __SYMTAB_H__

struct symbol {
	unsigned long addr;
	unsigned long size;
	char *section;
	char *name;
};

extern struct symbol symbol_table[];
extern unsigned long start_addr;
extern unsigned long end_addr;
extern unsigned long total_symbols;

#define SYMTAB_IS_HIT(__idx, __addr)                             \
	(                                                        \
	 (symbol_table[__idx].addr <= (unsigned long)__addr) &&  \
	 (symbol_table[__idx].addr + symbol_table[__idx].size >= \
		  (unsigned long)__addr)                         \
	)

int symtab_lookup_addr(unsigned long addr);
int symtab_lookup_name(char *name);

#endif

