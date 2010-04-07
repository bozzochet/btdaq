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
 * $Id: text.h,v 1.1 1995/04/01 00:39:31 zhao Exp $
 *
 */
#ifndef FL_TEXT_H
#define FL_TEXT_H

enum
{
    FL_NORMAL_TEXT
};

#define FL_TEXT_BOXTYPE    FL_FLAT_BOX
#define FL_TEXT_COL1       FL_COL1
#define FL_TEXT_COL2       FL_MCOL
#define FL_TEXT_LCOL       FL_LCOL
#define FL_TEXT_ALIGN      FL_ALIGN_LEFT

FL_EXPORT FL_OBJECT *fl_create_text(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT FL_OBJECT *fl_add_text(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


#endif 

