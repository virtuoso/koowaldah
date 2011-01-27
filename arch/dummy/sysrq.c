/*
 * arch/dummy/sysrq.c
 *
 * Copyright (C) 2008 Alexander Shishkin
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

#include <koowaldah.h>
#include <dummy/loader.h>

void dummy_sysrq_handler(int ch)
{
#ifdef OPT_SYSRQ_DEBUG
	sysrq_hit(0xff);
	sysrq_hit(ch);
#endif
}

