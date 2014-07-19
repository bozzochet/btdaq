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

#ifndef FD_menuattrib_h_
#define FD_menuattrib_h_

/** Callbacks, globals and object handlers **/
extern void add_menu_item_cb(FL_OBJECT *, long);
extern void delete_menu_item_cb(FL_OBJECT *, long);
extern void change_menu_item_cb(FL_OBJECT *, long);
extern void clear_menu_field_cb(FL_OBJECT *, long);
extern void replace_menu_item_cb(FL_OBJECT *, long);
extern void new_menuapi_cb(FL_OBJECT *, long);
extern void menuentry_scope_cb(FL_OBJECT *, long);


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *menuattrib;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *content_br;
	FL_OBJECT *input;
	FL_OBJECT *auto_clear;
	FL_OBJECT *mode;
	FL_OBJECT *shortcut;
	FL_OBJECT *new_menuapi;
	FL_OBJECT *scope;
} FD_menuattrib;

extern FD_menuattrib * create_form_menuattrib(void);

#endif /* FD_menuattrib_h_ */
