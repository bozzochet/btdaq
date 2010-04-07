/*
 *
 * This file is part of XForms.
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
 * $Id: fd_groups.c,v 1.0 2002/05/08 05:27:39 zhao Release $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 * This file is part of the Form Designer.
 *
 * It maintains a list of groups in the form. Each group can be
 * assigned a name.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "forms.h"
#include "fd_main.h"
#include "ui_theforms.h"

/**** Data Structure maintaining groups in current form ****/

static FL_OBJECT *begobj[MAXGROUP];	/* Begin objects of the groups */

/**** Call-back routines ****/

/* Sets the current group in the form */
void
group_cb(FL_OBJECT * obj, long arg)
{
    int nn = fl_get_browser(fd_control->groupbrowser);

    if (nn > 0)
	addgroupto_selection(begobj[nn]);
    else if (nn < 0)
	deletegroupfrom_selection(begobj[-nn]);
    redraw_the_form(0);
}

/* Changes the name of the current group */
void
changegroupname_cb(FL_OBJECT * obj, long arg)
{
    char name[MAX_VAR_LEN], cbname[MAX_VAR_LEN], argname[MAX_VAR_LEN];
    int i, numb = 0;
    const char *s;

    if (cur_form == NULL)
	return;

    for (i = 1; i <= fl_get_browser_maxline(fd_control->groupbrowser); i++)
    {
	if (fl_isselected_browser_line(fd_control->groupbrowser, i))
	{
	    if (numb != 0)
	    {
		fl_show_messages("Cannot change name of multiple groups");
		return;
	    }
	    numb = i;
	}
    }

    if (numb == 0)
    {
	fl_show_messages("No group was selected");
	return;
    }

    get_object_name(begobj[numb], name, cbname, argname);
    if (!(s = fl_show_input("Give the new group name", name)))
	return;

    strcpy(name, s);
    set_object_name(begobj[numb], name, cbname, argname);
    fillin_groups();
    changed = 1;
    return;
}

/************ MAIN ROUTINES ******************/

/* fills in the current list of groups. */
void
fillin_groups(void)
{
    char name[MAX_VAR_LEN], cbname[MAX_VAR_LEN], argname[MAX_VAR_LEN];
    FL_OBJECT *obj, *obj2;
    int i = 1;

    if (cur_form == NULL)
    {
	fl_clear_browser(fd_control->groupbrowser);
	return;
    }

    fl_freeze_form(fd_control->groupbrowser->form);
    fl_clear_browser(fd_control->groupbrowser);

    /* Check whether groups are empty. */
    obj = cur_form->first;
    while (obj != NULL)
    {
	obj2 = obj;
	obj = obj->next;
	if (obj2->objclass == FL_END_GROUP &&
	    (obj2->prev != NULL && obj2->prev->objclass == FL_BEGIN_GROUP))
	{
	    fl_delete_object(obj2->prev);
	    fl_delete_object(obj2);
	}
    }

    /* Put the still existing groups in the list */
    obj = cur_form->first;
    while (obj != NULL)
    {
	if (obj->objclass == FL_BEGIN_GROUP)
	{
	    begobj[i] = obj;
	    get_object_name(obj, name, cbname, argname);
	    if (name[0] == '\0')
		strcpy(name, "<no name>");
	    fl_add_browser_line(fd_control->groupbrowser, name);
	    if (is_selected(obj))
		fl_select_browser_line(fd_control->groupbrowser, i);
	}
	else if (obj->objclass == FL_END_GROUP)
	    i++;
	obj = obj->next;
    }
    fl_unfreeze_form(fd_control->groupbrowser->form);
}
