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
 * $Id: goodie_input.c,v 1.0 2002/05/08 05:16:30 zhao Release $
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
#include <stdlib.h>

/* simple input form */
typedef struct
{
    FL_FORM *form;
    FL_OBJECT *input;
    FL_OBJECT *cancel;
    FL_OBJECT *clear;
    FL_OBJECT *ok;
}
FD_input;

static void
clear_cb(FL_OBJECT * ob, long data)
{
    FD_input *fd = ob->form->fdui;
    fl_set_input(fd->input, "");
}


static FD_input *
create_form_input(void)
{
    FL_OBJECT *obj;
    FD_input *fdui = (FD_input *) fl_calloc(1, sizeof(*fdui));
    int oldu = fl_get_coordunit();

    fl_set_coordunit(FL_COORD_PIXEL);
    fdui->form = fl_bgn_form(FL_NO_BOX, 410, 120);
    fl_set_form_title(fdui->form, "Input");
    obj = fl_add_box(FL_UP_BOX, 0, 0, 410, 120, "");
    fl_set_object_bw(obj, -2);
    obj = fl_add_frame(FL_ENGRAVED_FRAME, 8, 9, 394, 67, "");
    fdui->input = obj = fl_add_input(FL_NORMAL_INPUT, 20, 33, 370, 30, "");
    fl_set_object_lalign(obj, FL_ALIGN_TOP_LEFT);
    fdui->cancel = obj = fl_add_button(FL_NORMAL_BUTTON, 30, 85, 80, 26, "Cancel");
    fl_set_button_shortcut(obj, "^[", 1);
    fdui->clear = obj = fl_add_button(FL_NORMAL_BUTTON, 300, 85, 80, 26, "Clear");
    fl_set_object_callback(obj, clear_cb, 0);
    fdui->ok = obj = fl_add_button(FL_RETURN_BUTTON, 165, 85, 80, 26, "OK");
    fl_end_form();

    fl_adjust_form_size(fdui->form);
    fdui->form->fdui = fdui;
    fl_set_coordunit(oldu);

    return fdui;
}

/*---------------------------------------*/

static FD_input *fd_input;

/* Asks the user for textual input */
const char *
fl_show_input(const char *str1, const char *defstr)
{
    static int first = 1;
    FL_OBJECT *retobj;

    if (!fd_input)
    {
	int oldy = fl_inverted_y;

	fl_inverted_y = 0;
	fd_input = create_form_input();
	fl_set_form_hotobject(fd_input->form, fd_input->ok);
	fl_set_form_atclose(fd_input->form, fl_goodies_atclose, fd_input->ok);
	fl_register_raw_callback(fd_input->form, FL_ALL_EVENT,
				 fl_goodies_preemptive);
	fl_inverted_y = oldy;
    }

    fl_handle_goodie_font(fd_input->ok, fd_input->input);
    fl_handle_goodie_font(fd_input->cancel, fd_input->clear);

    if (first)
    {
	fl_parse_goodies_label(fd_input->ok, FLOKLabel);
	fl_parse_goodies_label(fd_input->clear, FLInputClearLabel);
	fl_parse_goodies_label(fd_input->cancel, FLInputCancelLabel);
	first = 0;
    }


    fl_get_goodie_title(fd_input->form, FLInputTitle);
    fl_set_object_label(fd_input->input, str1);
    fl_set_input(fd_input->input, defstr);

    if (!fd_input->form->visible)
	fl_deactivate_all_forms();

    fl_show_form(fd_input->form, FL_PLACE_HOTSPOT, FL_TRANSIENT,
		 fd_input->form->label);

    fl_update_display(0);

    /* grab keyboard focus */
    fl_winfocus(fd_input->form->window);

    do
    {
	retobj = fl_do_only_forms();
    }
    while (retobj != fd_input->ok && retobj != fd_input->cancel);

    fl_hide_form(fd_input->form);
    fl_activate_all_forms();

    return (retobj == fd_input->ok ? fl_get_input(fd_input->input) : 0);
}

void
fl_hide_input(void)
{
    if (fd_input && fd_input->form->visible)
	fl_object_qenter(fd_input->cancel);
}
