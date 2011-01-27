/*
 * arch/i386/loader/loader.h
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

#ifndef __I386_LOADER_H__
#define __I386_LOADER_H__

#define __initcode __attribute__((section(".init.code")))
#define __initdata __attribute__((section(".init.data")))
#define __text32   __attribute__((section(".text32")))
#define __data32   __attribute__((section(".data32")))

/* these come from header in linux_proto.S */
extern unsigned long code32_start;
extern unsigned short start_sys;
extern unsigned long realmode_switch;
extern unsigned char type_of_loader;

/* trampoline to 32-bit/protected mode */
void prot_switch();

#ifndef BITS32
asm(".code16gcc");
#endif

#endif

