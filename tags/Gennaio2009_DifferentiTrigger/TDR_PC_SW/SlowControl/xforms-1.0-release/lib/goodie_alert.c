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
 * $Id: goodie_alert.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 */

#include "forms.h"

/****************** Alert dialog ********************{**/
typedef struct
{
    FL_FORM *form;
    FL_OBJECT *str;
    FL_OBJECT *but;
    FL_OBJECT *title;
}
FD_alert;

static FD_alert *fd_alert;

static FD_alert *
create_alert(void)
{
    FD_alert *fdui = (FD_alert *) fl_calloc(1, sizeof(*fdui));
    int oldy = fl_inverted_y;
    int oldu = fl_get_coordunit();
    FL_OBJECT *ob;

    fl_inverted_y = 0;
    fl_set_coordunit(FL_COORD_PIXEL);

    fdui->form = fl_bgn_form(FL_NO_BOX, 460, 130);
    fl_set_form_title(fdui->form, "Alert");
    ob = fl_add_box(FL_UP_BOX, 0, 0, 460, 130, "");
    fl_set_object_bw(ob, -2);


    fl_add_box(FL_FLAT_BOX, 50, 30, 380, 5, "@DnLine");
    fdui->title = fl_add_box(FL_FLAT_BOX, 50, 10, 380, 20, "");
    fl_set_object_lstyle(fdui->title, FL_BOLD_STYLE);
    fl_set_object_lsize(fdui->title, FL_NORMAL_SIZE);
    fdui->str = fl_add_text(FL_FLAT_BOX, 50, 36, 380, 45, "");
    fl_set_object_lalign(fdui->str, FL_ALIGN_CENTER);
    fdui->but = fl_add_button(FL_RETURN_BUTTON, 185, 90, 90, 27, "Dismiss");
    fl_add_warn_icon(8, 15, 35, 35);
    fl_set_form_hotobject(fdui->form, fdui->but);
    fl_end_form();
    fl_register_raw_callback(fdui->form, FL_ALL_EVENT, fl_goodies_preemptive);
    fl_set_form_atclose(fdui->form, fl_goodies_atclose, fdui->but);
    fdui->form->fdui = fdui;

    if (fl_cntl.buttonFontSize != FL_DEFAULT_SIZE)
	fl_fit_object_label(fdui->but, 20, 2);

    fl_inverted_y = oldy;
    fl_set_coordunit(oldu);

    return fdui;
}


/* show a simple message with an Alert icon and a dismiss button */
void
fl_show_alert(const char *title, const char *str1, const char *str2, int c)
{
    static int first = 1;
    char buf[1024];

    if (!fd_alert)
	fd_alert = create_alert();

    if (first)
    {
	fl_parse_goodies_label(fd_alert->but, FLAlertDismissLabel);
	first = 0;
    }

    fl_get_goodie_title(fd_alert->form, FLAlertTitle);
    fl_handle_goodie_font(fd_alert->but, fd_alert->str);

    fl_set_object_label(fd_alert->title, title);
    snprintf(buf,sizeof(buf),"%s\n%s", (str1?str1:""),(str2?str2:""));
    fl_set_object_label(fd_alert->str, buf);

    if (!fd_alert->form->visible)
	fl_deactivate_all_forms();

    fl_show_form(fd_alert->form, c ? FL_PLACE_CENTER : FL_PLACE_HOTSPOT,
		 FL_TRANSIENT, fd_alert->form->label);

    fl_update_display(1);

    while (fl_do_only_forms() != fd_alert->but)
	;

    fl_hide_form(fd_alert->form);
    fl_activate_all_forms();
}

void
fl_init_alert(void)
{
    fd_alert = create_alert();
}

void
fl_hide_alert(void)
{
    if (fd_alert && fd_alert->form->visible)
	fl_object_qenter(fd_alert->but);
}


/********************* End of alert ************************}*/
