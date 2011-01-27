
/*
 * arch/i386/vga_console.c
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

#include <koowaldah.h>
#include <arch/asm.h>

#define VGA_BASE (void *) 0xB8000

#define VGA_LINES 25
#define VGA_COLUMNS 80

static u8 vga_color;

static void vga_scroll()
{
        size_t t;

	/* Copy the video buffer contents one line upper */
	for(t = 0; t < VGA_COLUMNS * (VGA_LINES - 1) * 2; t++){
                ((char *)VGA_BASE)[t] = ((char *)(VGA_BASE + VGA_COLUMNS * 2))[t];
        }

	/* Clear the last line */
        for(t = 0; t < VGA_COLUMNS * 2; t = t + 2){
                *((char *)(VGA_BASE + VGA_COLUMNS * (VGA_LINES - 1) * 2 + t)) = 0;
        }
}

void vga_set_cursor(unsigned short cursor)
{
        outb(0x3D5, (unsigned char)(cursor));
        outb(0x3D4, 14);
        outb(0x3D5, (unsigned char)(cursor >> 8));
}

unsigned short vga_get_cursor()
{
        unsigned short cursor;

        outb(0x3D4, 14);
        cursor = inb(0x3D5) << 8;
        outb(0x3D4, 15);
        cursor |= inb(0x3D5);

        return cursor;
}

void __init early_vga_init()
{
	/* bogus man! */
	vga_color = 0x0f;
}

void vga_setcolor(u8 c)
{
	vga_color = c;
}

void early_vga_put_char(char c)
{
        unsigned short cursor;

        cursor = vga_get_cursor();

	if('\n' == c){
		if(cursor < VGA_COLUMNS * (VGA_LINES - 1)){
			cursor -= cursor % VGA_COLUMNS;
			cursor += VGA_COLUMNS;
		}else{
			vga_scroll();
			cursor -= cursor % VGA_COLUMNS;
		}
	}else{
		if(cursor == VGA_LINES * VGA_COLUMNS){
			/* if we are in the lower right corner, we
			* have to scroll the screen one line up*/
			cursor -= VGA_COLUMNS;
			vga_scroll();
		}

		*((char *)(VGA_BASE + cursor * 2)) = c;
		*((char *)(VGA_BASE + cursor * 2 + 1)) = vga_color;

		cursor++;
	}
	vga_set_cursor(cursor);
}

