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
 * $Id: labelbut.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1997-2002  T.C. Zhao
 *  All rights reserved.
 *.
 *
 *  All Buttons. Additional button class can be added via
 *  fl_add_button_class and fl_create_generic_button
 */

#if defined(F_ID) || defined(DEBUG)
char *fl_id_fbut = "$Id: labelbut.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include <sys/types.h>
#include "forms.h"

typedef FL_BUTTON_STRUCT SPEC;

/********** DRAWING *************/

/* Draws the object */
static void
draw_labelbutton(FL_OBJECT * ob)
{
    FL_COLOR scol = ob->lcol;
    FL_COLOR col = ob->lcol;
    FL_Coord dh, dw, ww, absbw = FL_abs(ob->bw);

    if (ob->belowmouse)
	col = ob->col1;
    if (((SPEC *) (ob->spec))->val)
	col = ob->col2;

    ob->lcol = col;

    dh = 0.6f * ob->h;
    dw = FL_min(0.6f * ob->w, dh);

    ww = 0.75f * ob->h;
    if (ww < (dw + absbw + 1 + (ob->bw > 0)))
	ww = dw + absbw + 1 + (ob->bw > 0);

    if (ob->type == FL_RETURN_BUTTON)
    {
	fl_drw_text(0,
		    (FL_Coord) (ob->x + ob->w - ww),
		    (FL_Coord) (ob->y + 0.2f * ob->h),
		    dw, dh, ob->lcol, 0, 0, "@returnarrow");

    }

    fl_draw_object_label(ob);
    ob->lcol = scol;
}




/* creates an object */
FL_OBJECT *
fl_create_labelbutton(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		      const char *label)
{
    FL_OBJECT *ob;

    fl_add_button_class(FL_LABELBUTTON, draw_labelbutton, 0);
    ob = fl_create_generic_button(FL_LABELBUTTON, type, x, y, w, h, label);
    ob->boxtype = FL_FLAT_BOX;
    ob->col1 = FL_RED;
    ob->col2 = FL_BLUE;
    ob->align = FL_LIGHTBUTTON_ALIGN;
    ob->lcol = FL_LIGHTBUTTON_LCOL;
    return ob;
}


/* Adds an object */
FL_OBJECT *
fl_add_labelbutton(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		   const char *label)
{
    FL_OBJECT *ob;

    ob = fl_create_labelbutton(type, x, y, w, h, label);
    fl_add_object(fl_current_form, ob);
    return ob;
}
