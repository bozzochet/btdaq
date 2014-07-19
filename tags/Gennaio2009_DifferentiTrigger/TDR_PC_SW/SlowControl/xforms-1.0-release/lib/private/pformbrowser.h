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
 * $Id$
 */
#ifndef PFORMBROWSER_H
#define PFORMBROWSER_H

typedef struct
{

    FL_OBJECT *canvas;
    FL_OBJECT *parent;
    int nforms;
    FL_FORM **form;
    int v_pref;
    int h_pref;
    int vw_def, vw;
    int hh_def, hh;
    FL_OBJECT *hsl, *vsl;
    int h_on, v_on;

    int x, y;
    int left_edge;
    int top_edge;
    int top_form;

    int max_height;
    int max_width;

    int processing_destroy;
    int in_draw;
    int scroll;			/* either pixel based or form based */
}
FL_FORMBROWSER_SPEC;

#define IsFormBrowserClass(ob) ((ob) && (ob)->objclass==FL_FORMBROWSER)

#define OBSPEC(ob) ( ( SPEC *) ob->spec )

#if defined( sun ) && !defined( __SVR4 )
#define memmove(a,b,c) bcopy(b,a,c)
#endif

#endif
