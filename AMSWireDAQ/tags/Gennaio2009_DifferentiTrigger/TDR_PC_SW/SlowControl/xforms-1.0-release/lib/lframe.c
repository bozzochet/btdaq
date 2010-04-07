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
 * $Id: lframe.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao
 *  All rights reserved.
 *.
 *
 *  similar to FL_FRAME, but label is drawn On the frame
 *
 */
#if defined(F_ID) || defined(DEBUG)
char *fl_id_lframe = "$Id: lframe.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include "forms.h"

static int
handle_lframe(FL_OBJECT * ob, int event, FL_Coord mx, FL_Coord my,
	      int key, void *ev)
{
    int sx, sy, sw, sh, align, bw = FL_abs(ob->bw), dy;
    int margin, len;

    switch (event)
    {
    case FL_DRAW:
	fl_drw_frame(ob->type, ob->x, ob->y, ob->w, ob->h, ob->col1, ob->bw);

    case FL_DRAWLABEL:
	if (!(len = strlen(ob->label)))
	    return 0;

	fl_get_string_dimension(ob->lstyle, ob->lsize,
				ob->label, len, &sw, &sh);

	align = ob->align & ~FL_ALIGN_INSIDE;

	sw += 8;
	margin = 11 + (ob->w * 0.02);
	dy = 0;

	if (ob->type == FL_ROUNDED_FRAME)
	    margin += 7;

	if ((ob->w - sw) < 2 * margin)
	{
	    margin /= 2;
	    sw -= 2;
	}

	if ((ob->w - sw) < 2 * margin)
	{
	    margin /= 2;
	    sw -= 2;
	}

	if (ob->type == FL_UP_FRAME || ob->type == FL_DOWN_FRAME)
	    dy = (bw + 1) / 2;

	if (align == FL_ALIGN_TOP_RIGHT || align == FL_ALIGN_RIGHT)
	{
	    sx = ob->x + ob->w - margin - sw;
	    sy = ob->y - sh / 2 - dy;
	}
	else if (align == FL_ALIGN_TOP)
	{
	    sx = ob->x + (ob->w - sw) / 2;
	    sy = ob->y - sh / 2 - dy;
	}
	else if (align == FL_ALIGN_LEFT_BOTTOM)
	{
	    sx = ob->x + margin;
	    sy = ob->y + ob->h - sh / 2 + dy;
	}
	else if (align == FL_ALIGN_BOTTOM_RIGHT)
	{
	    sx = ob->x + ob->w - margin - sw;
	    sy = ob->y + ob->h - sh / 2 + dy;
	}
	else if (align == FL_ALIGN_BOTTOM)
	{
	    sx = ob->x + (ob->w - sw) / 2;
	    sy = ob->y + ob->h - sh / 2 + dy;
	}
	else
	{
	    sx = ob->x + margin;
	    sy = ob->y - sh / 2 - dy;
	}


	fl_drw_box(FL_FLAT_BOX, sx, sy, sw, sh, ob->col2, 0);

	fl_drw_text(FL_ALIGN_CENTER, sx, sy, sw, sh,
		    ob->lcol, ob->lstyle, ob->lsize, ob->label);
	break;
    }
    return 0;
}

FL_OBJECT *
fl_create_labelframe(int type, FL_Coord x, FL_Coord y, FL_Coord w,
		     FL_Coord h, const char *label)
{
    FL_OBJECT *ob;

    ob = fl_make_object(FL_LABELFRAME, type, x, y, w, h, label, handle_lframe);
    ob->boxtype = FL_NO_BOX;
    ob->col1 = FL_FRAME_COL1;
    ob->col2 = FL_FRAME_COL2;
    ob->lcol = FL_FRAME_LCOL;
    ob->align = FL_ALIGN_TOP_LEFT;
    ob->active = 0;
    return ob;
}

FL_OBJECT *
fl_add_labelframe(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		  const char *label)
{
    FL_OBJECT *ob;
    ob = fl_create_labelframe(type, x, y, w, h, label);
    fl_add_object(fl_current_form, ob);
    return ob;
}
