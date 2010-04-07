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
 * $Id: extern.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-1998  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 * All global variables used in forms.  Should not be used
 * by application programs.  There is no guarantee that these
 * variables exist in future version of XForms
 *
 */

#ifndef FL_EXTERN_H_MAKING_FORMS_ONLY
#define FL_EXTERN_H_MAKING_FORMS_ONLY

extern Pixmap fl_gray_pattern[];
extern GC fl_bwgc[];
extern GC fl_whitegc;
extern int fl_safe;
extern FL_COLOR fl_shadow_col;
extern XKeyboardControl fl_keybdcontrol;
extern unsigned long fl_keybdmask;

extern FL_OBJECT *fl_current_group;

#define fl_inactive_pattern fl_gray_pattern[1]

extern Display *fl_display;
extern Window fl_root, fl_vroot;
extern int fl_screen;

extern int fl_vmode, fl_vdepth;
extern int fl_bestvmode, fl_mostdepth;

extern int fl_scrh, fl_scrw;
extern unsigned int fl_keymask;

extern FL_WM_STUFF fl_wmstuff;
extern float fl_dpi;
extern FL_IOPT fl_cntl;
extern int fl_inverted_y;

extern FL_CONTEXT *fl_context;
extern FL_TARGET  *flx;
extern long fl_requested_vid;
extern int fl_no_connection;
extern char fl_curfnt[127];

extern int fl_canvas_boxtype2frame(int);

extern void fl_drw_tbox(int, FL_Coord, FL_Coord, FL_Coord, FL_Coord,
			FL_COLOR, int);


extern void (*fl_xyplot_return) (FL_OBJECT *, int);


#endif /* !def EXTERN.H */
