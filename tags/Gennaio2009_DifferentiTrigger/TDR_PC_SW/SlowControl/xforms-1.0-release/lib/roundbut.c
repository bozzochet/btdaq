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
 * $Id: roundbut.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *
 */

#if defined(F_ID) || defined(DEBUG)
char *fl_id_rbut = "$Id: roundbut.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include <sys/types.h>
#include "forms.h"

typedef FL_BUTTON_STRUCT SPEC;

/* Draws a round button */
static void
draw_roundbutton(FL_OBJECT * ob)
{
    int c1;
    FL_Coord xx, yy, rr, cr, bw = FL_abs(ob->bw);
    SPEC *sp = ob->spec;

    if (ob->boxtype == FL_NO_BOX
	&& (sp->event == FL_ENTER || sp->event == FL_LEAVE))
	return;

    c1 = ob->belowmouse ? FL_ROUNDBUTTON_MCOL : FL_ROUNDBUTTON_TOPCOL;

    fl_drw_box(ob->boxtype, ob->x, ob->y, ob->w, ob->h, c1, ob->bw);

    rr = 0.3f * FL_min(ob->w, ob->h);
    rr = (int) (rr + 0.5f);
    xx = ob->x + 1.5f * (bw < 2 ? 2 : bw) + rr + 1.1;
    yy = ob->y + ob->h / 2;

    fl_circf(xx, yy, rr, ob->col1);
    fl_circ(xx, yy, rr, FL_BLACK);

    if (sp->val)
    {
	cr = (0.8f * rr);
	fl_circf(xx, yy, cr, ob->col2);
	fl_circ(xx, yy, cr, FL_BLACK);
    }

    if (ob->align == FL_ALIGN_CENTER)
	fl_drw_text(FL_ALIGN_LEFT, xx + rr + 1, ob->y, 0, ob->h,
		    ob->lcol, ob->lstyle, ob->lsize, ob->label);
    else
	fl_draw_object_label_outside(ob);

    if (ob->type == FL_RETURN_BUTTON)
	fl_drw_text(0,
		    (FL_Coord) (ob->x + ob->w - 0.8f * ob->h),
		    (FL_Coord) (ob->y + 0.2f * ob->h),
		    (FL_Coord) (0.6f * ob->h), (FL_Coord) (0.6f * ob->h),
		    ob->lcol, 0, 0, "@returnarrow");
}

/* creates an object */
FL_OBJECT *
fl_create_roundbutton(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		      const char *label)
{
    FL_OBJECT *ob;

    fl_add_button_class(FL_ROUNDBUTTON, draw_roundbutton, 0);
    ob = fl_create_generic_button(FL_ROUNDBUTTON, type, x, y, w, h, label);
    ob->boxtype = FL_ROUNDBUTTON_BOXTYPE;
    ob->col1 = FL_ROUNDBUTTON_COL1;
    ob->col2 = FL_ROUNDBUTTON_COL2;
    ob->align = FL_ROUNDBUTTON_ALIGN;
    ob->lcol = FL_ROUNDBUTTON_LCOL;
    return ob;
}

/* Adds an object */
FL_OBJECT *
fl_add_roundbutton(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		   const char *label)
{
    FL_OBJECT *ob;
    ob = fl_create_roundbutton(type, x, y, w, h, label);
    fl_add_object(fl_current_form, ob);
    return ob;
}
