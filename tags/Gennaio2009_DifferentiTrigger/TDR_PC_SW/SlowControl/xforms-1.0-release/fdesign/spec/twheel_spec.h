/*
 *
 * This file is part of XForms.
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


/** Header file generated with fdesign on Fri Sep 25 00:08:29 1998.**/

#ifndef FD_twheelattrib_h_
#define FD_twheelattrib_h_

/** Callbacks, globals and object handlers **/
extern void twheel_adjust_precision(FL_OBJECT *, long);
extern void twheel_minmax_change(FL_OBJECT *, long);
extern void twheel_returnsetting_change(FL_OBJECT *, long);
extern void twheel_initialvalue_change(FL_OBJECT *, long);
extern void twheel_step_change(FL_OBJECT *, long);


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *twheelattrib;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *background;
	FL_OBJECT *prec;
	FL_OBJECT *minval;
	FL_OBJECT *maxval;
	FL_OBJECT *returnsetting;
	FL_OBJECT *initial_val;
	FL_OBJECT *step;
} FD_twheelattrib;

extern FD_twheelattrib * create_form_twheelattrib(void);

#endif /* FD_twheelattrib_h_ */
