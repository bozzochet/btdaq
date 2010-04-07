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
 * $Id: goodie_choice.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *
 */

#include "forms.h"

/****************** Make a choice ***********************{**/

typedef struct
{
    FL_FORM *form;
    FL_OBJECT *str;
    FL_OBJECT *but[4];		/* 0 not used    */
    const char *sc[4];		/* shortcut keys */
}
FD_choice;

static FD_choice *fd_choice;
static int default_choice;


static FD_choice *
create_choice(void)
{
    FD_choice *fdui = (FD_choice *) fl_calloc(1, sizeof(*fdui));
    int oldy = fl_inverted_y;
    int oldu = fl_get_coordunit();

    fl_inverted_y = 0;
    fl_set_coordunit(FL_COORD_PIXEL);
    fdui->form = fl_bgn_form(FL_UP_BOX, 460, 130);
    fl_set_form_title(fdui->form, "Choice");
    fdui->str = fl_add_box(FL_FLAT_BOX, 20, 15, 420, 65, "");
    fdui->but[1] = fl_add_button(FL_NORMAL_BUTTON, 40, 93, 90, 27, "");
    fdui->but[2] = fl_add_button(FL_NORMAL_BUTTON, 185, 93, 90, 27, "");
    fdui->but[3] = fl_add_button(FL_NORMAL_BUTTON, 330, 93, 90, 27, "");
    fdui->sc[1] = "1";
    fdui->sc[2] = "2";
    fdui->sc[3] = "3";
    fl_end_form();
    fl_inverted_y = oldy;
    fl_set_coordunit(oldu);
    return fdui;
}

int
fl_show_choices(const char *msg, int numb,
		const char *c1, const char *c2, const char *c3, int def)
{
    FL_OBJECT *retobj;
    const char *c[4];
    int i;

    c[1] = c1;
    c[2] = c2;
    c[3] = c3;

    if (!fd_choice)
	fd_choice = create_choice();

    fl_handle_goodie_font(fd_choice->but[1], fd_choice->but[2]);
    fl_handle_goodie_font(fd_choice->but[3], fd_choice->str);
    fl_set_object_label(fd_choice->str, msg);

    fl_hide_object(fd_choice->but[1]);
    fl_hide_object(fd_choice->but[2]);
    fl_hide_object(fd_choice->but[3]);

    default_choice = def;

    switch (numb)
    {
    case 3:
	for (i = 1; i <= 3; i++)
	{
	    fl_set_object_label(fd_choice->but[i], c[i]);
	    fl_set_object_shortcut(fd_choice->but[i], fd_choice->sc[i], 1);
	    fl_show_object(fd_choice->but[i]);
	    fl_fit_object_label(fd_choice->but[i], 1, 1);
	}
	break;
    case 2:
	/* pick button 1 and 3 */
	fl_set_object_label(fd_choice->but[1], c[1]);
	fl_set_object_shortcut(fd_choice->but[1], fd_choice->sc[1], 1);
	fl_show_object(fd_choice->but[1]);
	fl_fit_object_label(fd_choice->but[1], 1, 1);

	fl_set_object_label(fd_choice->but[3], c[2]);
	fl_set_object_shortcut(fd_choice->but[3], fd_choice->sc[2], 1);
	fl_show_object(fd_choice->but[3]);
	fl_fit_object_label(fd_choice->but[3], 1, 1);
	break;
    case 1:
	fl_set_object_label(fd_choice->but[1], c[1]);
	fl_set_object_shortcut(fd_choice->but[1], fd_choice->sc[1], 1);
	fl_show_object(fd_choice->but[1]);
	fl_fit_object_label(fd_choice->but[1], 1, 1);
	break;
    default:
	return 0;
    }

    fl_get_goodie_title(fd_choice->form, FLChoiceTitle);

    if (!fd_choice->form->visible)
	fl_deactivate_all_forms();

    if (def > 0 && def <= 3)
	fl_set_form_hotobject(fd_choice->form, fd_choice->but[def]);
    else
	fl_set_form_hotspot(fd_choice->form, -1, -1);

    fl_show_form(fd_choice->form, FL_PLACE_HOTSPOT, FL_TRANSIENT,
		 fd_choice->form->label);

    fl_update_display(0);

    do
	retobj = fl_do_only_forms();
    while (retobj != fd_choice->but[1] &&
	   retobj != fd_choice->but[2] &&
	   retobj != fd_choice->but[3]);

    fl_hide_form(fd_choice->form);
    fl_activate_all_forms();

    /* reset shortcut */
    fd_choice->sc[1] = "1";
    fd_choice->sc[2] = "2";
    fd_choice->sc[3] = "3";

    return (retobj == fd_choice->but[1]) ? 1 :
	((retobj == fd_choice->but[2] || numb == 2) ? 2 : 3);
}

int
fl_show_choice(const char *m1, const char *m2, const char *m3, int numb,
	       const char *c1, const char *c2, const char *c3, int def)
{
    char buf[1024];

    snprintf(buf, sizeof(buf), "%s\n%s\n%s", m1 ? m1 : "",
	    m2 ? m2 : "",
	    m3 ? m3 : "");
    return fl_show_choices(buf, numb, c1, c2, c3, def);
}

void
fl_set_choices_shortcut(const char *a, const char *b, const char *c)
{
    if (!fd_choice)
	fd_choice = create_choice();
    fd_choice->sc[1] = a;
    fd_choice->sc[2] = b;
    fd_choice->sc[3] = c;
}

void
fl_hide_choice(void)
{
    if (fd_choice && fd_choice->form->visible)
    {
	if (default_choice <= 0 || default_choice > 3)
	    default_choice = 1;
	fl_object_qenter(fd_choice->but[default_choice]);
    }
}
