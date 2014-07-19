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
 * $Id: util.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *
 * X independent utilities
 *
 */
#if defined(F_ID) || defined(DEBUG)
char *fl_id_util = "$Id: util.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "forms.h"

/* Sets the form windom */
void
fl_set_form_window(FL_FORM * form)
{
    if (form && form->window > 0)
	flx->win = form->window;
}


static int showerrors = 1;
void
fl_show_errors(int y)
{
    showerrors = y;
}

void
fl_error(const char *where, const char *why)
{
    int resp;

    M_err(where, why);

    if (showerrors)
    {
	resp = fl_show_choice("XForms Error", where, why, 3,
			      "Continue", "Exit", "HideErrors", 2);
	if (resp == 2)
	    exit(-1);
	else if (resp == 3)
	    showerrors = 0;
    }

}

/*
 * for debugging
 */
#define VN(v)  {v,#v}
static FL_VN_PAIR flevent[] =
{
    VN(FL_ENTER), VN(FL_LEAVE), VN(FL_PUSH), VN(FL_RELEASE),
    VN(FL_STEP), VN(FL_SHORTCUT), VN(FL_MOUSE), VN(FL_MOTION),
    VN(FL_KEYBOARD), VN(FL_DRAW), VN(FL_FOCUS), VN(FL_UNFOCUS),
    VN(FL_FREEMEM), VN(FL_DRAWLABEL), VN(FL_DBLCLICK),
    VN(FL_OTHER), VN(FL_ATTRIB),
    VN(-1)
};

const char *
fl_event_name(int ev)
{
    return fl_get_vn_name(flevent, ev);
}

static FL_VN_PAIR flclass[] =
{
    VN(FL_BUTTON), VN(FL_LIGHTBUTTON),
    VN(FL_ROUNDBUTTON), VN(FL_ROUND3DBUTTON),
    VN(FL_CHECKBUTTON), VN(FL_BITMAPBUTTON), VN(FL_PIXMAPBUTTON),
    VN(FL_BITMAP), VN(FL_PIXMAP), VN(FL_BOX), VN(FL_TEXT),
    VN(FL_MENU), VN(FL_CHART), VN(FL_CHOICE),
    VN(FL_COUNTER), VN(FL_SLIDER), VN(FL_VALSLIDER), VN(FL_INPUT),
    VN(FL_BROWSER), VN(FL_DIAL), VN(FL_TIMER), VN(FL_CLOCK),
    VN(FL_POSITIONER), VN(FL_FREE), VN(FL_XYPLOT),
    VN(FL_FRAME), VN(FL_LABELFRAME), VN(FL_CANVAS),
    VN(FL_GLCANVAS), VN(FL_TABFOLDER), VN(FL_SCROLLBAR),
    VN(FL_SCROLLBUTTON), VN(FL_MENUBAR), VN(FL_IMAGECANVAS),
    VN(FL_TEXTBOX),
    VN(-1)
};

const char *
fl_object_class_name(FL_OBJECT * ob)
{
    return !ob ? "null" :
	(ob == FL_EVENT ? "fl_event" : fl_get_vn_name(flclass, ob->objclass));
}

void
fl_print_form_object(FL_FORM * form, const char *msg)
{
    FL_OBJECT *ob;

    if (msg && *msg)
	fprintf(stderr, "**** %s ****\n", msg);

    fprintf(stderr, "dumping form: %s\n", form ?
	    (form->label ? form->label : "Unknown") : "null");

    if (!form)
	return;

    for (ob = form->first; ob; ob = ob->next)
	fprintf(stderr, "Next: %s (parent: %s)\n",
		fl_object_class_name(ob),
		fl_object_class_name(ob->parent));

    fprintf(stderr, "\n");

    for (ob = form->last; ob; ob = ob->prev)
	fprintf(stderr, "Prev: %s (parent: %s) \n",
		fl_object_class_name(ob),
		fl_object_class_name(ob->parent));
}
