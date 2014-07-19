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
 * $Id: counter.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 */

#ifndef FL_COUNTER_H
#define FL_COUNTER_H

typedef enum
{
    FL_NORMAL_COUNTER,
    FL_SIMPLE_COUNTER
}
FL_COUNTER_TYPE;

/***** Defaults *****/

#define FL_COUNTER_BOXTYPE	FL_UP_BOX
#define FL_COUNTER_COL1		FL_COL1
#define FL_COUNTER_COL2		FL_BLUE		/* ct label     */
#define FL_COUNTER_LCOL		FL_LCOL		/* ct reporting */
#define FL_COUNTER_ALIGN	FL_ALIGN_BOTTOM

/***** Others   *****/

#define FL_COUNTER_BW		(FL_BOUND_WIDTH-1)

/***** Routines *****/
FL_EXPORT FL_OBJECT *fl_create_counter(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT FL_OBJECT *fl_add_counter(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT void fl_set_counter_value(
		FL_OBJECT *ob,
		double val
		);

FL_EXPORT void fl_set_counter_bounds(
		FL_OBJECT *ob,
		double min,
		double max
		);

FL_EXPORT void fl_set_counter_step(
		FL_OBJECT *ob,
		double s,
		double l
		);

FL_EXPORT void fl_set_counter_precision(
		FL_OBJECT *ob,
		int prec
		);

FL_EXPORT void fl_set_counter_return(
		FL_OBJECT *ob,
		int how
		);

FL_EXPORT double fl_get_counter_value(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_get_counter_bounds(
		FL_OBJECT *ob,
		double *min,
		double *max
		);

FL_EXPORT void fl_get_counter_step(
		FL_OBJECT *ob,
		float *s,
		float *l
		);

FL_EXPORT void fl_set_counter_filter(
		FL_OBJECT *ob,
		FL_VAL_FILTER filter
		);


#endif
