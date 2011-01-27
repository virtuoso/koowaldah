/*
 * Copyright (c) 1997 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Juergen Hannken-Illjes.
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

#ifndef __PCKBD_TABLE_H__
#define __PCKBD_TABLE_H__
#include "keysyms.h"

static const u16 pckbd_sctrans[] = {
	0,
	KS_Escape,
	KS_exclam,
	KS_at,
	KS_numbersign,
	KS_dollar,
	KS_percent,
	KS_asciicircum,
	KS_ampersand,
	KS_asterisk,
	KS_parenleft,
	KS_parenright,
	KS_underscore,
	KS_plus,
	KS_Delete,
	KS_Tab,
	KS_q,
	KS_w,
	KS_e,
	KS_r,
	KS_t,
	KS_y,
	KS_u,
	KS_i,
	KS_o,
	KS_p,
	KS_braceleft,
	KS_braceright,
	KS_Return,
	KS_Control_L,
	KS_a,
	KS_s,
	KS_d,
	KS_f,
	KS_g,
	KS_h,
	KS_j,
	KS_k,
	KS_l,
	KS_colon,
	KS_quotedbl,
	KS_asciitilde,
	KS_Shift_L,
	KS_bar,
	KS_z,
	KS_x,
	KS_c,
	KS_v,
	KS_b,
	KS_n,
	KS_m,
	KS_less,
	KS_greater,
	KS_question,
	KS_Shift_R,
	KS_KP_Multiply,
	KS_Alt_L,
	KS_space,
	KS_Caps_Lock,
	KS_f1,
	KS_f2,
	KS_f3,
	KS_f4,
	KS_f5,
	KS_f6,
	KS_f7,
	KS_f8,
	KS_f9,
	KS_f10,
	KS_Num_Lock,
	KS_Hold_Screen,
	KS_KP_7,
	KS_KP_8,
	KS_KP_9,
	KS_KP_Subtract,
	KS_KP_4,
	KS_KP_5,
	KS_KP_6,
	KS_KP_Add,
	KS_KP_1,
	KS_KP_2,
	KS_KP_3,
	KS_KP_0,
	KS_KP_Decimal,
	KS_f11,
	KS_f12,
	KS_Pause,
	KS_KP_Enter,
	KS_Control_R,
	KS_Print_Screen,
	KS_KP_Divide,
	KS_Print_Screen,
	KS_Multi_key,
	KS_Home,
	KS_Up,
	KS_Prior,
	KS_Left,
	KS_Right,
	KS_End,
	KS_Down,
	KS_Next,
	KS_Insert,
	KS_Delete,
	KS_Meta_L,
	KS_Meta_R,
	KS_Menu,
};

#endif
