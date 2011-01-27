/*
 * include/dummy/irq.h
 *
 * Copyright (C) 2006 Alexey Zaytsev
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

#ifndef __ARCH_IRQ_H__
#define __ARCH_IRQ_H__

#define local_irq_save(flags) do { if (0) flags++; } while (0)
#define local_irq_restore(flags) do { if (0) flags++; } while (0)

#endif

