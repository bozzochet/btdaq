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
 * $Id: lightbut.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *
 */

#if defined(F_ID) || defined(DEBUG)
char *fl_id_lbut = "$Id: lightbut.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include <sys/types.h>
#include "forms.h"

typedef FL_BUTTON_STRUCT SPEC;

/* Draws the lightbutton */
static void
draw_lightbutton(FL_OBJECT * ob)
{
    int c1, c2, libox;
    FL_Coord xx, yy, ww, hh, absbw = FL_abs(ob->bw), bw2;
    SPEC *sp = ob->spec;

    c1 = ob->belowmouse ? FL_LIGHTBUTTON_MCOL : FL_LIGHTBUTTON_TOPCOL;
    c2 = sp->val ? ob->col2 : ob->col1;

    fl_drw_box(ob->boxtype, ob->x, ob->y, ob->w, ob->h, c1, ob->bw);

    if (ob->boxtype == FL_NO_BOX || ob->boxtype == FL_FLAT_BOX)
	absbw = FL_abs(FL_BOUND_WIDTH);

    /* otherwise too close to the edge */
    if (absbw < 3)
	absbw = 3;

    /* Calculate and draw the light */
    hh = ob->h - 3 * absbw - 1;

    if (hh < FL_LIGHTBUTTON_MINSIZE)
	hh = FL_LIGHTBUTTON_MINSIZE;

    ww = hh / 2;
    if (ww < FL_LIGHTBUTTON_MINSIZE)
	ww = FL_LIGHTBUTTON_MINSIZE;
    if (ww > ob->w / 6)
	ww = ob->w / 6;

    xx = ob->x + 1.5 * absbw + 1;
    yy = ob->y + ob->h / 2 - hh / 2;

    absbw = FL_abs(ob->bw);

    /* adjustment for non-rectangular boxes */
    if (ob->boxtype == FL_ROUNDED3D_UPBOX ||
	ob->boxtype == FL_ROUNDED3D_DOWNBOX)
    {
	hh -= 2;
	yy += 1;
	xx += 3 + (ob->w * 0.01f);
	ww -= 1;
    }
    else if (ob->boxtype == FL_RSHADOW_BOX)
    {
	hh -= 1;
	xx += 1;
    }

    switch (ob->boxtype)
    {
    case FL_UP_BOX:
    case FL_ROUNDED3D_UPBOX:
	libox = FL_DOWN_BOX;
	break;
    case FL_DOWN_BOX:
    case FL_ROUNDED3D_DOWNBOX:
	libox = FL_DOWN_BOX;
	break;
    case FL_FRAME_BOX:
    case FL_EMBOSSED_BOX:
    case FL_ROUNDED_BOX:
	libox = ob->boxtype;
	break;
    case FL_RFLAT_BOX:
	libox = FL_ROUNDED_BOX;
	break;
    case FL_RSHADOW_BOX:
	libox = FL_ROUNDED_BOX;
	break;
    default:
	libox = FL_BORDER_BOX;
	break;
    }

    bw2 = (absbw > 2) ? (absbw - 1) : absbw;
    fl_drw_box(libox, xx, yy, ww, hh, c2, bw2);


    /* Draw the label */
    if ((ob->align & ~FL_ALIGN_INSIDE) == FL_ALIGN_CENTER)
	fl_drw_text(FL_ALIGN_LEFT, xx + ww + 1, ob->y, ob->w - ww - 3,
		    ob->h, ob->lcol, ob->lstyle, ob->lsize, ob->label);
    else
	fl_draw_object_label(ob);

    ww = 0.75f * ob->h;
    if (ww < absbw + 1)
	ww = absbw + 1;

    if (ob->type == FL_RETURN_BUTTON)
	fl_drw_text(0,
		    (FL_Coord) (ob->x + ob->w - ww),
		    (FL_Coord) (ob->y + 0.2f * ob->h),
		    (FL_Coord) (0.6f * ob->h),
		    (FL_Coord) (0.6f * ob->h),
		    ob->lcol, 0, 0, "@returnarrow");
}

/* creates an object */
FL_OBJECT *
fl_create_lightbutton(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		      const char *label)
{
    FL_OBJECT *ob;

    fl_add_button_class(FL_LIGHTBUTTON, draw_lightbutton, 0);
    ob = fl_create_generic_button(FL_LIGHTBUTTON, type, x, y, w, h, label);
    ob->boxtype = FL_LIGHTBUTTON_BOXTYPE;
    ob->col1 = FL_LIGHTBUTTON_COL1;
    ob->col2 = FL_LIGHTBUTTON_COL2;
    ob->align = FL_LIGHTBUTTON_ALIGN;
    ob->lcol = FL_LIGHTBUTTON_LCOL;
    return ob;
}


/* Adds an object */
FL_OBJECT *
fl_add_lightbutton(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		   const char *label)
{
    FL_OBJECT *ob;

    ob = fl_create_lightbutton(type, x, y, w, h, label);
    fl_add_object(fl_current_form, ob);
    return ob;
}
