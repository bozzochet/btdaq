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


/** Header file generated with fdesign on Sat Jan 16 15:26:17 1999.**/

#ifndef FD_dialattrib_h_
#define FD_dialattrib_h_

/** Callbacks, globals and object handlers **/
extern void dial_returnsetting_change(FL_OBJECT *, long);
extern void dial_minmax_change(FL_OBJECT *, long);
extern void dial_initialvalue_change(FL_OBJECT *, long);
extern void dial_stepchange_cb(FL_OBJECT *, long);
extern void dial_thetachange_cb(FL_OBJECT *, long);
extern void dir_cb(FL_OBJECT *, long);


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *dialattrib;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *background;
	FL_OBJECT *returnsetting;
	FL_OBJECT *minval;
	FL_OBJECT *maxval;
	FL_OBJECT *initialval;
	FL_OBJECT *step;
	FL_OBJECT *thetai;
	FL_OBJECT *thetaf;
	FL_OBJECT *dir;
} FD_dialattrib;

extern FD_dialattrib * create_form_dialattrib(void);

#endif /* FD_dialattrib_h_ */
