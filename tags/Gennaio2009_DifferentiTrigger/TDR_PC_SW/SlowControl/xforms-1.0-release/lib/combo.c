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
 * $Id: combo.c,v 0.10 1998/09/21 00:27:58 zhao Exp $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *  All Buttons. Additional button class can be added via
 *  fl_add_button_class and fl_create_generic_button
 */

#if defined(F_ID) || defined(DEBUG)
char *fl_id_combo = "$Id: combo.c,v 0.10 1998/09/21 00:27:58 zhao Exp $";
#endif

#include <sys/types.h>
#include "forms.h"

typedef struct
{
    FL_OBJECT *input;
    FL_OBJECT *browser;
}
SPEC;



static int
handle(FL_OBJECT * ob, int ev, int mx, int my, int key, void *xev)
{
    return 0;
}


/* creates an object */
FL_OBJECT *
fl_create_combobox(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		   const char *label)
{
    FL_OBJECT *ob;

    ob = fl_make_object(FL_COMBOBOX, type, x, y, w, h, label, handle);

    return ob;
}

/* Adds an object */
FL_OBJECT *
fl_add_combobox(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		const char *label)
{
    FL_OBJECT *ob;

    ob = fl_create_combobox(type, x, y, w, h, label);
    fl_add_object(fl_current_form, ob);
    return ob;
}
