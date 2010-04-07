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
 * $Id: tooltip.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1998-2002  T.C. Zhao 
 *  All rights reserved.
 *.
 *
 * Show message in a top-level unmanaged window. Used
 * for tooltips
 *
 */

#include "forms.h"

typedef struct
{
     FL_FORM*   tooltipper;
     void*      vdata;
     char*      cdata;
     long       ldate;
     FL_OBJECT* text;
     int        fntstyle;
     int        fntsize;
     FL_COLOR   background;
     FL_COLOR   textcolor;
     int        boxtype;
     int        lalign;
} TOOLTIP;

static TOOLTIP *tip;

static void
create_it(void)
{
    if (!tip)
    {
        FL_OBJECT *text;
        tip = fl_calloc(1, sizeof(*tip));
        tip->fntstyle = FL_NORMAL_STYLE;
        tip->fntsize = FL_DEFAULT_SIZE;
        tip->boxtype = FL_BORDER_BOX;
        tip->lalign = FL_ALIGN_LEFT|FL_ALIGN_INSIDE;
        tip->textcolor = FL_BLACK;
        tip->background = FL_YELLOW;
	tip->tooltipper = fl_bgn_form(FL_NO_BOX, 5, 5);
	tip->text = text = fl_add_box(tip->boxtype, 0, 0, 5, 5, "");
	fl_set_object_bw(text, -1);
	fl_set_object_lstyle(text, tip->fntstyle);
	fl_set_object_lsize(text, tip->fntsize);
	fl_set_object_lcol(text, tip->textcolor);
	fl_set_object_lalign(text, tip->lalign);
	fl_set_object_color(text, tip->background, tip->background);
	fl_end_form();
    }
}

void
fl_set_tooltip_boxtype(int bt)
{
    create_it();
    tip->boxtype = bt;
    fl_set_object_boxtype(tip->text, bt);
}

void
fl_show_tooltip(const char *s, int x, int y)
{
    int maxw = 0, maxh = 0, extra;

    if (!s)
	return;

    create_it();

    extra = 1 + !(tip->boxtype == FL_FLAT_BOX || tip->boxtype == FL_BORDER_BOX);
    fl_get_string_dimension(tip->fntstyle, tip->fntsize, 
                            s, strlen(s), &maxw, &maxh);

    maxw += 7 + extra;
    maxh += 7 + extra;

    if(maxw > 800)
       maxw = 800;
    if(maxh > 800)
       maxh = 800;

    fl_freeze_form(tip->tooltipper);
    fl_set_form_geometry(tip->tooltipper, x, y, maxw, maxh);
    fl_set_object_label(tip->text, s);
    fl_unfreeze_form(tip->tooltipper);

    if (!tip->tooltipper->visible)
	fl_show_form(tip->tooltipper, FL_PLACE_GEOMETRY | FL_FREE_SIZE,
		     FL_NOBORDER, "Tooltip");
    fl_update_display(1);
}

void
fl_hide_tooltip(void)
{
    create_it();
    if (tip->tooltipper->visible)
	fl_hide_form(tip->tooltipper);
}

void
fl_set_tooltip_color(FL_COLOR tc, FL_COLOR bc)
{
    create_it();
    fl_set_object_lcol(tip->text, tip->textcolor = tc);
    tip->background = bc;
    fl_set_object_color(tip->text, tip->background, tip->background);
}

void
fl_set_tooltip_font(int style, int size)
{
    create_it();
    fl_set_object_lstyle(tip->text, tip->fntstyle = style);
    fl_set_object_lsize(tip->text, tip->fntsize = size);
}

void 
fl_set_tooltip_lalign(int align)
{
    create_it();
    fl_set_object_lalign(tip->text, tip->lalign = align);
}
