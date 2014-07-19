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
 * $Id: goodie_yesno.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 */

#include "forms.h"
#include <ctype.h>

/************* Ask yes/no question **********************{****/
typedef struct
{
    FL_FORM *form;
    FL_OBJECT *str;
    FL_OBJECT *yes;
    FL_OBJECT *no;
}
FD_yesno;

static FD_yesno *
create_yesno(void)
{
    FD_yesno *fdui = (FD_yesno *) fl_calloc(1, sizeof(*fdui));
    int oldy = fl_inverted_y;
    int oldu = fl_get_coordunit();

    fl_inverted_y = 0;
    fl_set_coordunit(FL_COORD_PIXEL);

    fdui->form = fl_bgn_form(FL_UP_BOX, 460, 130);
    fl_set_form_title(fdui->form, "Question");
    fdui->str = fl_add_box(FL_FLAT_BOX, 20, 15, 420, 65, "");
    fdui->yes = fl_add_button(FL_NORMAL_BUTTON, 85, 90, 80, 27, "Yes");
    fl_set_button_shortcut(fdui->yes, "Yy", 1);
    fdui->no = fl_add_button(FL_NORMAL_BUTTON, 295, 90, 80, 27, "No");
    fl_set_button_shortcut(fdui->no, "Nn", 1);
    fl_add_q_icon(10, 20, 33, 33);
    fl_end_form();
    fl_register_raw_callback(fdui->form, FL_ALL_EVENT, fl_goodies_preemptive);
    fl_set_form_atclose(fdui->form, fl_goodies_atclose, fdui->no);
    if (fl_cntl.buttonFontSize != FL_DEFAULT_SIZE)
	fl_fit_object_label(fdui->no, 22, 2);

    fl_inverted_y = oldy;
    fl_set_coordunit(oldu);
    return fdui;
}

static FD_yesno *fd_yesno;
static int default_ans;

/* Shows a question with two buttons, yes, and no */
int
fl_show_question(const char *str, int ans)
{
    static int first = 1;
    FL_OBJECT *retobj;

    if (!fd_yesno)
	fd_yesno = create_yesno();

    default_ans = ans;

    if (first)
    {
	char shortcut[4];
	int k = 0;

	first = 1;		/* force parsing every time */
	fl_parse_goodies_label(fd_yesno->yes, FLQuestionYesLabel);
	fl_parse_goodies_label(fd_yesno->no, FLQuestionNoLabel);
	shortcut[0] = fd_yesno->yes->label[0];
	shortcut[1] = tolower(fd_yesno->yes->label[0]);
	shortcut[2] = toupper(fd_yesno->yes->label[0]);
	shortcut[3] = '\0';
	fl_set_button_shortcut(fd_yesno->yes, shortcut, 1);
	k = tolower(fd_yesno->yes->label[0]) == tolower(fd_yesno->no->label[0]);
	shortcut[0] = fd_yesno->no->label[k];
	shortcut[1] = toupper(fd_yesno->no->label[k]);
	shortcut[2] = tolower(fd_yesno->no->label[k]);
	fl_set_button_shortcut(fd_yesno->no, shortcut, 1);
    }

    fl_get_goodie_title(fd_yesno->form, FLQuestionTitle);
    fl_handle_goodie_font(fd_yesno->yes, fd_yesno->str);
    fl_handle_goodie_font(fd_yesno->no, 0);

    fl_set_object_label(fd_yesno->str, str);


    if (ans == 1)
	fl_set_form_hotobject(fd_yesno->form, fd_yesno->yes);
    else if (ans == 0)
	fl_set_form_hotobject(fd_yesno->form, fd_yesno->no);
    else
	fl_set_form_hotspot(fd_yesno->form, -1, -1);

    if (!fd_yesno->form->visible)
	fl_deactivate_all_forms();

    fl_show_form(fd_yesno->form, FL_PLACE_HOTSPOT, FL_TRANSIENT,
		 fd_yesno->form->label);
    fl_update_display(0);

    while ((retobj = fl_do_only_forms()) != fd_yesno->yes &&
	   retobj != fd_yesno->no)
	;

    fl_hide_form(fd_yesno->form);
    fl_activate_all_forms();
    return (retobj == fd_yesno->yes);
}

void
fl_hide_question(void)
{
    if (fd_yesno && fd_yesno->form->visible)
	fl_object_qenter(default_ans == 1 ? fd_yesno->yes : fd_yesno->no);
}
