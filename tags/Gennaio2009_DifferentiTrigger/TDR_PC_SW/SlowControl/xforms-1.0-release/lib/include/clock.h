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
 * $Id: clock.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 */
#ifndef FL_CLOCK_H
#define FL_CLOCK_H

enum
{
    FL_ANALOG_CLOCK,
    FL_DIGITAL_CLOCK
};

#define FL_CLOCK_BOXTYPE   FL_UP_BOX
#define FL_CLOCK_COL1      FL_INACTIVE_COL
#define FL_CLOCK_COL2      FL_BOTTOM_BCOL
#define FL_CLOCK_LCOL      FL_BLACK
#define FL_CLOCK_ALIGN     FL_ALIGN_BOTTOM

#define FL_CLOCK_TOPCOL  FL_COL1

FL_EXPORT FL_OBJECT *fl_create_clock(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *s
		);


FL_EXPORT FL_OBJECT *fl_add_clock(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *s
		);

FL_EXPORT void fl_get_clock(
		FL_OBJECT *ob,
		int *h,
		int *m,
		int *s
		);

FL_EXPORT long fl_set_clock_adjustment(
		FL_OBJECT *ob,
		long offset
		);

FL_EXPORT void fl_set_clock_ampm(
		FL_OBJECT *ob,
		int y
		);


#endif
