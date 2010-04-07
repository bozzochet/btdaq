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

/********************** crop here for forms.h **********************/

/*
 * $Id: menu.h,v 0.86 1997/03/22 06:04:08 zhao Beta $
 *
 */
#ifndef FL_MENUBAR_H
#define FL_MENUBAR_H		/* { */


/************   Object Class: MenuBar         ************/

enum
{
    FL_NORMAL_MENUBAR
};

/***** Defaults *****/

#define FL_MENUBAR_BOXTYPE	FL_UP_BOX
#define FL_MENUBAR_COL1		FL_COL1
#define FL_MENUBAR_COL2		FL_MCOL
#define FL_MENUBAR_LCOL		FL_LCOL

/***** Routines *****/

FL_EXPORT FL_OBJECT *fl_create_menubar(int, FL_Coord, FL_Coord, FL_Coord, FL_Coord,
				 const char *);


FL_EXPORT FL_OBJECT *fl_add_menubar(int, FL_Coord, FL_Coord, FL_Coord, FL_Coord,
			      const char *);


FL_EXPORT void fl_clear_menubar(FL_OBJECT *);

FL_EXPORT void fl_set_menubar(FL_OBJECT *, const char *);

FL_EXPORT void fl_set_menubar_entries(FL_OBJECT *, const char *, FL_PUP_ENTRY *);


#endif /* MENUBAR } */
