/*
 *
 *  This file is part of the XForms library package.
 *
 * XForms is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1, or
 * (at your option) any later version.
 *
 * XForms is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with XForms; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 *
 */


/*
 * $Id: global.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-1998  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 * All gloabl varialbes used in XForms. It is important to start all
 * variables wth fl/FL to avoid name space pollution.
 *
 */

#ifndef FL_GLOBAL_H
#define FL_GLOBAL_H

Window fl_root, fl_vroot;
int fl_screen;

/* memory routines */
void *(*fl_calloc) (size_t, size_t) = calloc;
void *(*fl_malloc) (size_t) = malloc;
void *(*fl_realloc) (void *, size_t) = realloc;
void (*fl_free) (void *) = free;

FL_State fl_state[6];
int fl_vmode = -1, fl_vdepth;
int fl_bestvmode, fl_mostdepth;

int fl_scrh, fl_scrw;
int fl_safe;
unsigned int fl_keymask;
char *fl_ul_magic_char = "\010";

FL_COLOR fl_shadow_col = FL_RIGHT_BCOL;
XKeyboardControl fl_keybdcontrol;
unsigned long fl_keybdmask = (1L << 8) - 1;

FL_WM_STUFF fl_wmstuff;
float fl_dpi = 80.0;
int fl_inverted_y;
long fl_requested_vid;
FL_IOPT fl_cntl;

FL_CONTEXT *fl_context;
char fl_curfnt[127];
FL_FORM *fl_current_form;
FL_OBJECT *fl_current_group;
int fl_no_connection;
FL_WIN *fl_app_win;
FL_OBJECT *FL_EVENT = (FL_OBJECT *) (-1L);	/* The special event object */

/* to improve link profile */
void (*fl_xyplot_return) (FL_OBJECT * ob, int);

FL_TARGET *flx;

#endif /* !def GLOBAL_H */
