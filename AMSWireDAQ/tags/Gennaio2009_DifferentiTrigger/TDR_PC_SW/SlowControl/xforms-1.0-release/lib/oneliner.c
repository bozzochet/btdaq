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
 * $Id: oneliner.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao 
 *  All rights reserved.
 *.
 *
 *
 * Show message in a top-level unmanaged window.
 *
 */

#include "forms.h"

static int fntstyle = FL_NORMAL_STYLE, fntsize = FL_DEFAULT_SIZE;
static FL_COLOR background = FL_YELLOW, textcolor = FL_BLACK;
static FL_FORM *oneliner;
static FL_OBJECT *text;

static void
create_it(void)
{
    if (!oneliner)
    {
	oneliner = fl_bgn_form(FL_NO_BOX, 5, 5);
	text = fl_add_box(FL_BORDER_BOX, 0, 0, 5, 5, "");
	fl_set_object_lstyle(text, fntstyle);
	fl_set_object_lsize(text, fntsize);
	fl_set_object_lcol(text, textcolor);
	fl_set_object_color(text, background, background);
	fl_end_form();
    }
}

void
fl_show_oneliner(const char *s, int x, int y)
{
    int maxw = 0, maxh = 0;

    if (!s)
	return;

    create_it();

    fl_get_string_dimension(fntstyle, fntsize, s, strlen(s), &maxw, &maxh);

    maxw += 6;
    maxh += 6;

    fl_freeze_form(oneliner);
    fl_set_form_geometry(oneliner, x, y, maxw, maxh);
    fl_set_object_label(text, s);
    fl_unfreeze_form(oneliner);

    if (!oneliner->visible)
	fl_show_form(oneliner, FL_PLACE_GEOMETRY | FL_FREE_SIZE,
		     FL_NOBORDER, "OneLiner");
    fl_update_display(1);
}

void
fl_hide_oneliner(void)
{
    create_it();
    if (oneliner->visible)
	fl_hide_form(oneliner);
}

void
fl_set_oneliner_color(FL_COLOR tc, FL_COLOR bc)
{
    create_it();
    fl_set_object_lcol(text, textcolor = tc);
    background = bc;
    fl_set_object_color(text, background, background);
}

void
fl_set_oneliner_font(int style, int size)
{
    create_it();
    fl_set_object_lstyle(text, fntstyle = style);
    fl_set_object_lsize(text, fntsize = size);
}
