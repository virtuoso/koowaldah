
/*
 * arch/i386/vga_console.c
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#include <kuca.h>
#include <arch/vga_console.h>
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

static void set_cursor(unsigned short cursor)
{
        io_port_out(0x3D5, (unsigned char)(cursor));
        io_port_out(0x3D4, 14);
        io_port_out(0x3D5, (unsigned char)(cursor >> 8));
}

static unsigned short get_cursor()
{
        unsigned short cursor;

        io_port_out(0x3D4, 14);
        cursor = io_port_in(0x3D5) << 8;
        io_port_out(0x3D4, 15);
        cursor |= io_port_in(0x3D5);

        return cursor;
}

void early_vga_init()
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

        cursor = get_cursor();

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
	set_cursor(cursor);
}

