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


/** Header file generated with fdesign on Sat Jun 10 14:18:40 2000.**/

#ifndef FD_twheelform_h_
#define FD_twheelform_h_

/** Callbacks, globals and object handlers **/
extern void valchange_cb(FL_OBJECT *, long);
extern void returnchange_cb(FL_OBJECT *, long);


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *twheelform;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *vert;
	FL_OBJECT *hor;
	FL_OBJECT *report;
	FL_OBJECT *returnsetting;
} FD_twheelform;

extern FD_twheelform * create_form_twheelform(void);

#endif /* FD_twheelform_h_ */
