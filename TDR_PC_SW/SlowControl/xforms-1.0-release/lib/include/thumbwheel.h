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
 * $Id: thumbwheel.h,v 0.10 1998/09/24 06:05:44 zhao Exp $
 */

#ifndef FL_THUMBWHEEL_H
#define FL_THUMBWHEEL_H

enum
{
    FL_VERT_THUMBWHEEL,
    FL_HOR_THUMBWHEEL
};



FL_EXPORT double fl_get_thumbwheel_value(
		FL_OBJECT *ob
		);

FL_EXPORT double fl_set_thumbwheel_value(
		FL_OBJECT *ob,
		double value
		);

FL_EXPORT double fl_get_thumbwheel_step(
		FL_OBJECT *ob
		);

FL_EXPORT double fl_set_thumbwheel_step(
		FL_OBJECT *ob,
		double step
		);

FL_EXPORT int fl_set_thumbwheel_return(
		FL_OBJECT *ob,
		int how
		);

FL_EXPORT int fl_set_thumbwheel_crossover(
		FL_OBJECT *ob,
		int flag
		);


FL_EXPORT void fl_set_thumbwheel_bounds(
		FL_OBJECT *ob,
		double min,
		double max
		);

FL_EXPORT void fl_get_thumbwheel_bounds(
		FL_OBJECT *ob,
		double *min,
		double *max
		);


FL_EXPORT FL_OBJECT *fl_create_thumbwheel(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_add_thumbwheel(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


/* defaults */

#define FL_THUMBWHEEL_COL1      FL_COL1
#define FL_THUMBWHEEL_COL2      FL_COL1
#define FL_THUMBWHEEL_LCOL      FL_BLACK
#define FL_THUMBWHEEL_BOXTYPE   FL_BORDER_BOX
#define FL_THUMBWHEEL_ALIGN     FL_ALIGN_BOTTOM

#endif    /* THUMBWHEEL_H */
