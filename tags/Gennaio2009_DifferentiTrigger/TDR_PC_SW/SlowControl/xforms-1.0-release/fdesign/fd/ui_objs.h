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


/** Header file generated with fdesign on Sun Feb 22 00:34:23 1998.**/

#ifndef FD_buttonform_h_
#define FD_buttonform_h_

/** Callbacks, globals and object handlers **/




/**** Forms and Objects ****/
typedef struct {
	FL_FORM *buttonform;
	void *vdata;
	char *cdata;
	long  ldata;
} FD_buttonform;

extern FD_buttonform * create_form_buttonform(void);
typedef struct {
	FL_FORM *valuatorform;
	void *vdata;
	char *cdata;
	long  ldata;
} FD_valuatorform;

extern FD_valuatorform * create_form_valuatorform(void);
typedef struct {
	FL_FORM *staticform;
	void *vdata;
	char *cdata;
	long  ldata;
} FD_staticform;

extern FD_staticform * create_form_staticform(void);

#endif /* FD_buttonform_h_ */
