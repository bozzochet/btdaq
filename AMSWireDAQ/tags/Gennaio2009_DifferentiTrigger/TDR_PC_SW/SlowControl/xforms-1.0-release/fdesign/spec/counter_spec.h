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


/** Header file generated with fdesign on Sat Feb 21 23:59:25 1998.**/

#ifndef FD_counterattrib_h_
#define FD_counterattrib_h_

/** Callbacks, globals and object handlers **/
extern void cnt_precision_cb(FL_OBJECT *, long);
extern void cnt_minmax_change(FL_OBJECT *, long);
extern void cnt_returnsetting_change(FL_OBJECT *, long);
extern void cnt_initialvalue_change(FL_OBJECT *, long);
extern void cnt_stepchange_cb(FL_OBJECT *, long);


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *counterattrib;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *background;
	FL_OBJECT *prec;
	FL_OBJECT *minval;
	FL_OBJECT *maxval;
	FL_OBJECT *returnsetting;
	FL_OBJECT *initialval;
	FL_OBJECT *step1;
	FL_OBJECT *step2;
} FD_counterattrib;

extern FD_counterattrib * create_form_counterattrib(void);

#endif /* FD_counterattrib_h_ */
