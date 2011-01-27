/*
 * kernel/symtab.c
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
 *
 */

#include <koowaldah.h>
#include <lib.h>
#include <symtab.h>
#include <bug.h>

int symtab_lookup_addr(unsigned long addr)
{
	int i;

	if (addr < start_addr || addr > end_addr)
		return 0;

	for (i = 0; i < total_symbols; i++)
		if (SYMTAB_IS_HIT(i, addr))
			return i;

	return 0;
}

int symtab_lookup_name(char *name)
{
	int i;

	for (i = 0; i < total_symbols; i++)
		if (!kstrcmp(symbol_table[i].name, name))
			return i;

	return 0;
}

