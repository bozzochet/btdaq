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
 * $Id: goodie_sinput.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *
 *  Input goodie
 */

#include "forms.h"

/***********    Simple input    ********************{*******/
typedef struct
{
    FL_FORM *form;
    FL_OBJECT *str1;
    FL_OBJECT *input;
    FL_OBJECT *but;
}
FD_input;


static FD_input *
create_input(void)
{
    FD_input *fdui = (FD_input *) fl_calloc(1, sizeof(*fdui));
    int oldy = fl_inverted_y;
    int oldu = fl_get_coordunit();

    fl_inverted_y = 0;
    fl_set_coordunit(FL_COORD_PIXEL);

    fdui->form = fl_bgn_form(FL_UP_BOX, 460, 130);
    fdui->str1 = fl_add_box(FL_NO_BOX, 20, 15, 420, 20, "");
    fdui->input = fl_add_input(FL_NORMAL_INPUT, 30, 50, 400, 30, "");
    fdui->but = fl_add_button(FL_RETURN_BUTTON, 185, 94, 90, 27, "OK");
    fl_set_form_hotobject(fdui->form, fdui->but);
    fl_end_form();
    fl_register_raw_callback(fdui->form, FL_ALL_EVENT, fl_goodies_preemptive);
    fl_set_form_atclose(fdui->form, fl_goodies_atclose, fdui->but);
    fl_inverted_y = oldy;
    fl_set_coordunit(oldu);

    return fdui;
}


/* Asks the user for textual input */
const char *
fl_show_simple_input(const char *str1, const char *defstr)
{
    static int first = 1;
    static FD_input *fd_input;

    if (!fd_input)
	fd_input = create_input();

    if (first)
    {
	fl_parse_goodies_label(fd_input->but, FLOKLabel);
	first = 0;
    }

    fl_handle_goodie_font(fd_input->but, fd_input->input);

    fl_set_object_label(fd_input->str1, str1);
    fl_set_input(fd_input->input, defstr);

    if (!fd_input->form->visible)
	fl_deactivate_all_forms();

    fl_show_form(fd_input->form, FL_PLACE_HOTSPOT, FL_TRANSIENT, "Input");
    fl_update_display(0);

    while (fl_do_only_forms() != fd_input->but)
	;

    fl_hide_form(fd_input->form);
    fl_activate_all_forms();
    return (fl_get_input(fd_input->input));
}
