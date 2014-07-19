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
 * $Id: slider.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 * Object Class: Slider
 *
 */
#ifndef FL_SLIDER_H
#define FL_SLIDER_H


typedef enum
{
    FL_VERT_SLIDER,
    FL_HOR_SLIDER,
    FL_VERT_FILL_SLIDER,
    FL_HOR_FILL_SLIDER,
    FL_VERT_NICE_SLIDER,
    FL_HOR_NICE_SLIDER,
    FL_HOR_BROWSER_SLIDER,
    FL_VERT_BROWSER_SLIDER,
    FL_HOR_BROWSER_SLIDER2,	/* for internal use only */
    FL_VERT_BROWSER_SLIDER2,	/* for internal use only */
    FL_HOR_THIN_SLIDER,		/* internal use          */
    FL_VERT_THIN_SLIDER,	/* internal use          */
    FL_HOR_BASIC_SLIDER,	/* internal use          */
    FL_VERT_BASIC_SLIDER	/* internal use          */
}
FL_SLIDER_TYPE;


/***** Defaults *****/

#define FL_SLIDER_BW1           FL_BOUND_WIDTH
#define FL_SLIDER_BW2           (FL_abs(FL_BOUND_WIDTH)-1)
#define FL_SLIDER_BOXTYPE	FL_DOWN_BOX
#define FL_SLIDER_COL1		FL_COL1
#define FL_SLIDER_COL2		FL_COL1
#define FL_SLIDER_LCOL		FL_LCOL
#define FL_SLIDER_ALIGN		FL_ALIGN_BOTTOM

/***** Others   *****/

#define FL_SLIDER_FINE		0.05
#define FL_SLIDER_WIDTH		0.10


/***** Routines *****/

FL_EXPORT FL_OBJECT *fl_create_slider(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_add_slider(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT FL_OBJECT *fl_create_valslider(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_add_valslider(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT void fl_set_slider_value(
		FL_OBJECT *ob,
		double val
		);

FL_EXPORT double fl_get_slider_value(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_set_slider_bounds(
		FL_OBJECT *ob,
		double min,
		double max
		);

FL_EXPORT void fl_get_slider_bounds(
		FL_OBJECT *ob,
		double *min,
		double *max
		);


FL_EXPORT void fl_set_slider_return(
		FL_OBJECT *ob,
		int value
		);


FL_EXPORT void fl_set_slider_step(
		FL_OBJECT *ob,
		double value
		);

FL_EXPORT void fl_set_slider_increment(
		FL_OBJECT *ob,
		double l,
		double r
		);

FL_EXPORT void fl_get_slider_increment(
		FL_OBJECT *ob,
		float *l,
		float *r
		);

FL_EXPORT void fl_set_slider_size(
		FL_OBJECT *ob,
		double size
		);

FL_EXPORT void fl_set_slider_precision(
		FL_OBJECT *ob,
		int prec
		);

FL_EXPORT void fl_set_slider_filter(
		FL_OBJECT *ob,
		FL_VAL_FILTER filter
		);



#endif 
