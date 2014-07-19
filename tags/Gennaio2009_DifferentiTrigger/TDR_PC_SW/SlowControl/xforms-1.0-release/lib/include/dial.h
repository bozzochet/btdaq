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
 * $Id: dial.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 */
#ifndef FL_DIAL_H
#define FL_DIAL_H

typedef enum
{
    FL_NORMAL_DIAL,
    FL_LINE_DIAL,
    FL_FILL_DIAL
}
FL_DIAL_TYPE;

enum
{
    FL_DIAL_CW, FL_DIAL_CCW
};

/***** Defaults *****/

#define FL_DIAL_BOXTYPE		FL_FLAT_BOX
#define FL_DIAL_COL1		FL_COL1
#define FL_DIAL_COL2		FL_RIGHT_BCOL
#define FL_DIAL_LCOL		FL_LCOL
#define FL_DIAL_ALIGN		FL_ALIGN_BOTTOM

/***** Others   *****/

#define FL_DIAL_TOPCOL		FL_COL1

/***** Routines *****/

FL_EXPORT FL_OBJECT *fl_create_dial(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_add_dial(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT void fl_set_dial_value(
		FL_OBJECT *ob,
		double val
		);

FL_EXPORT double fl_get_dial_value(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_set_dial_bounds(
		FL_OBJECT *ob,
		double min,
		double max
		);

FL_EXPORT void fl_get_dial_bounds(
		FL_OBJECT *ob,
		double *min,
		double *max
		);


FL_EXPORT void fl_set_dial_step(
		FL_OBJECT *ob,
		double value
		);

FL_EXPORT void fl_set_dial_return(
		FL_OBJECT *ob,
		int value
		);

FL_EXPORT void fl_set_dial_angles(
		FL_OBJECT *ob,
		double amin,
		double amax
		);

FL_EXPORT void fl_set_dial_cross(
		FL_OBJECT *ob,
		int flag
		);

#define fl_set_dial_crossover  fl_set_dial_cross

FL_EXPORT void fl_set_dial_direction(
		FL_OBJECT *ob,
		int dir
		);


#endif
