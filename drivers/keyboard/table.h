/*
 * Copyright (c) 1997 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Juergen Hannken-Illjes.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
