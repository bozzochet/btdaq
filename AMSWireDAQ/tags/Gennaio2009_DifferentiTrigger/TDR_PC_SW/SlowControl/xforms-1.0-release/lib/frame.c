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
 * $Id: frame.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 */
#if defined(F_ID) || defined(DEBUG)
char *fl_id_frame = "$Id: frame.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include "forms.h"

static int
handle_frame(FL_OBJECT * ob, int event, FL_Coord mx, FL_Coord my,
	     int key, void *ev)
{
    switch (event)
    {
    case FL_DRAW:
	fl_drw_frame(ob->type, ob->x, ob->y, ob->w, ob->h, ob->col1, ob->bw);
	fl_drw_text(ob->align, ob->x, ob->y, ob->w, ob->h,
		    ob->lcol, ob->lstyle, ob->lsize, ob->label);
	break;
    case FL_DRAWLABEL:
	if (ob->align != FL_ALIGN_CENTER)
	    fl_drw_text(ob->align, ob->x, ob->y, ob->w, ob->h,
			ob->lcol, ob->lstyle, ob->lsize, ob->label);
	break;
    }
    return 0;
}

FL_OBJECT *
fl_create_frame(int type, FL_Coord x, FL_Coord y, FL_Coord w,
		FL_Coord h, const char *label)
{
    FL_OBJECT *ob;

    ob = fl_make_object(FL_FRAME, type, x, y, w, h, label, handle_frame);
    ob->boxtype = FL_NO_BOX;
    ob->col1 = FL_FRAME_COL1;
    ob->col2 = FL_FRAME_COL2;
    ob->lcol = FL_FRAME_LCOL;
    ob->align = FL_ALIGN_CENTER;
    ob->active = 0;
    return ob;
}

FL_OBJECT *
fl_add_frame(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
	     const char *label)
{
    FL_OBJECT *ob;
    ob = fl_create_frame(type, x, y, w, h, label);
    fl_add_object(fl_current_form, ob);
    return ob;
}
