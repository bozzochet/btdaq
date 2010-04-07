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

#ifndef FD_browserattrib_h_
#define FD_browserattrib_h_

/** Callbacks, globals and object handlers **/
extern void add_item_cb(FL_OBJECT *, long);
extern void delete_item_cb(FL_OBJECT *, long);
extern void insert_cb(FL_OBJECT *, long);
extern void hscb_pref_cb(FL_OBJECT *, long);
extern void vscb_pref_cb(FL_OBJECT *, long);
extern void clear_field_cb(FL_OBJECT *, long);
extern void replace_item_cb(FL_OBJECT *, long);


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *browserattrib;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *content_br;
	FL_OBJECT *input;
	FL_OBJECT *hscb_pref;
	FL_OBJECT *vscb_pref;
	FL_OBJECT *auto_clear;
} FD_browserattrib;

extern FD_browserattrib * create_form_browserattrib(void);

#endif /* FD_browserattrib_h_ */
