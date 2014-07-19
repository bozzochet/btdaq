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
 * $Id: checkbut.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao
 *  All rights reserved.
 *.
 *
 *
 */

#if defined(F_ID) || defined(DEBUG)
char *fl_id_cbut = "$Id: checkbut.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include <sys/types.h>
#include "forms.h"

typedef FL_BUTTON_STRUCT SPEC;

/**************************************************************************
 * Check buttons
 **********************************************************************{*/
static void
draw_checkbutton(FL_OBJECT * ob)
{
    FL_Coord xx, yy, ww, hh, bw = FL_abs(ob->bw);


    if (((SPEC *) ob->spec)->event == FL_ENTER ||
	((SPEC *) ob->spec)->event == FL_LEAVE)
	return;

    fl_drw_box(ob->boxtype, ob->x, ob->y, ob->w, ob->h, ob->col1, ob->bw);

    ww = hh = (FL_Coord)(0.6f * FL_min(ob->w, ob->h));
    xx = (FL_Coord)(ob->x + 1.5f * (bw < 2 ? 2 : bw) + 1.5f);
    yy = ob->y + (ob->h - hh) / 2;

    if (((SPEC *) ob->spec)->val)
	fl_drw_checkbox(FL_DOWN_BOX, xx, yy, ww, hh, ob->col2, FL_abs(ob->bw));
    else
	fl_drw_checkbox(FL_UP_BOX, xx, yy, ww, hh, ob->col1, FL_abs(ob->bw));

    if (ob->align == FL_ALIGN_CENTER)
	fl_drw_text(FL_ALIGN_LEFT, xx + ww + 1, ob->y, ob->w - ww - 3, ob->h,
		    ob->lcol, ob->lstyle, ob->lsize, ob->label);
    else
	fl_drw_text_beside(ob->align, ob->x, ob->y, ob->w, ob->h,
			   ob->lcol, ob->lstyle, ob->lsize, ob->label);

    if (ob->type == FL_RETURN_BUTTON)
	fl_drw_text(0,
		    (FL_Coord) (ob->x + ob->w - 0.8f * ob->h),
		    (FL_Coord) (ob->y + 0.2f * ob->h),
		    (FL_Coord) (0.6f * ob->h), (FL_Coord) (0.6f * ob->h),
		    ob->lcol, 0, 0, "@returnarrow");
}

FL_OBJECT *
fl_create_checkbutton(int type, FL_Coord x, FL_Coord y, FL_Coord w,
		      FL_Coord h, const char *label)
{
    FL_OBJECT *ob;

    fl_add_button_class(FL_CHECKBUTTON, draw_checkbutton, 0);
    ob = fl_create_generic_button(FL_CHECKBUTTON, type, x, y, w, h, label);
    ob->boxtype = FL_CHECKBUTTON_BOXTYPE;
    ob->col1 = FL_CHECKBUTTON_COL1;
    ob->col2 = FL_CHECKBUTTON_COL2;
    ob->align = FL_CHECKBUTTON_ALIGN;
    ob->lcol = FL_CHECKBUTTON_LCOL;
    return ob;
}

FL_OBJECT *
fl_add_checkbutton(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		   const char *label)
{
    FL_OBJECT *ob;
    ob = fl_create_checkbutton(type, x, y, w, h, label);
    fl_add_object(fl_current_form, ob);
    return ob;
}
