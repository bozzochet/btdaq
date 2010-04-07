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

#ifndef FD_posattrib_h_
#define FD_posattrib_h_

/** Callbacks, globals and object handlers **/
extern void pos_returnsetting_change(FL_OBJECT *, long);
extern void pos_xminmax_change(FL_OBJECT *, long);
extern void pos_initialxvalue_change(FL_OBJECT *, long);
extern void pos_xstepchange_cb(FL_OBJECT *, long);
extern void pos_yminmax_change(FL_OBJECT *, long);
extern void pos_initialyvalue_change(FL_OBJECT *, long);
extern void pos_ystepchange_cb(FL_OBJECT *, long);


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *posattrib;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *background;
	FL_OBJECT *returnsetting;
	FL_OBJECT *xminval;
	FL_OBJECT *xmaxval;
	FL_OBJECT *initialxval;
	FL_OBJECT *xstep;
	FL_OBJECT *yminval;
	FL_OBJECT *ymaxval;
	FL_OBJECT *initialyval;
	FL_OBJECT *ystep;
} FD_posattrib;

extern FD_posattrib * create_form_posattrib(void);

#endif /* FD_posattrib_h_ */
