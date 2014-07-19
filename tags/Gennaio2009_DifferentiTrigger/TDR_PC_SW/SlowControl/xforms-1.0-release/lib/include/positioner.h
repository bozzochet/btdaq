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
 * $Id: positioner.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 */
#ifndef FL_POSITIONER_H
#define FL_POSITIONER_H		/* { */


#define FL_NORMAL_POSITIONER	0
#define FL_OVERLAY_POSITIONER	1

/***** Defaults *****/

#define FL_POSITIONER_BOXTYPE	FL_DOWN_BOX
#define FL_POSITIONER_COL1	FL_COL1
#define FL_POSITIONER_COL2	FL_RED
#define FL_POSITIONER_LCOL	FL_LCOL
#define FL_POSITIONER_ALIGN	FL_ALIGN_BOTTOM

/***** Others   *****/


/***** Routines *****/

FL_EXPORT FL_OBJECT *fl_create_positioner(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT FL_OBJECT *fl_add_positioner(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT void fl_set_positioner_xvalue(
		FL_OBJECT *ob,
		double val
		);

FL_EXPORT double fl_get_positioner_xvalue(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_set_positioner_xbounds(
		FL_OBJECT *ob,
		double min,
		double max
		);

FL_EXPORT void fl_get_positioner_xbounds(
		FL_OBJECT *ob,
		double *min,
		double *max
		);

FL_EXPORT void fl_set_positioner_yvalue(
		FL_OBJECT *ob,
		double val
		);

FL_EXPORT double fl_get_positioner_yvalue(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_set_positioner_ybounds(
		FL_OBJECT *ob,
		double min,
		double max
		);

FL_EXPORT void fl_get_positioner_ybounds(
		FL_OBJECT *ob,
		double *min,
		double *max
		);

FL_EXPORT void fl_set_positioner_xstep(
		FL_OBJECT *ob,
		double value
		);

FL_EXPORT void fl_set_positioner_ystep(
		FL_OBJECT *ob,
		double value
		);

FL_EXPORT void fl_set_positioner_return(
		FL_OBJECT *ob,
		int value
		);



#endif /* Positioner.H } */
