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


/** Header file generated with fdesign on Sat Sep 19 23:00:01 1998.**/

#ifndef FD_form0_h_
#define FD_form0_h_

/** Callbacks, globals and object handlers **/
extern void menu_callback(FL_OBJECT *, long);


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *form0;
	void *vdata;
	char *cdata;
	long  ldata;
} FD_form0;

extern FD_form0 * create_form_form0(void);

#endif /* FD_form0_h_ */
