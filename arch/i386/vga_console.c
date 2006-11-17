
/*
 * arch/i386/vga_console.c
 *
 * Copyright (C) 2005 Alexey Zaytsev
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Koowaldah developers nor the names of theyr 
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
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

