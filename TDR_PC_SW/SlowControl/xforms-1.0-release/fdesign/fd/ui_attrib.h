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


/** Header file generated with fdesign on Sat Feb 27 23:18:54 1999.**/

#ifndef FD_attrib_h_
#define FD_attrib_h_

/** Callbacks, globals and object handlers **/
extern void apply_cb(FL_OBJECT *, long);
extern void restore_cb(FL_OBJECT *, long);
extern void autoapply_setting_cb(FL_OBJECT *, long);
extern void folder_switch_cb(FL_OBJECT *, long);

extern void auto_apply_cb(FL_OBJECT *, long);
extern void validate_cvar_name_cb(FL_OBJECT *, long);
extern void setcolor_cb(FL_OBJECT *, long);



/**** Forms and Objects ****/
typedef struct {
	FL_FORM *attrib;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *background;
	FL_OBJECT *readyobj;
	FL_OBJECT *cancelobj;
	FL_OBJECT *applyobj;
	FL_OBJECT *restoreobj;
	FL_OBJECT *autoapplyobj;
	FL_OBJECT *attrib_folder;
} FD_attrib;

extern FD_attrib * create_form_attrib(void);
typedef struct {
	FL_FORM *generic_attrib;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *background;
	FL_OBJECT *generic_group;
	FL_OBJECT *typeobj;
	FL_OBJECT *boxobj;
	FL_OBJECT *nameobj;
	FL_OBJECT *cbnameobj;
	FL_OBJECT *argobj;
	FL_OBJECT *scobj;
	FL_OBJECT *labelobj;
	FL_OBJECT *fontobj;
	FL_OBJECT *styleobj;
	FL_OBJECT *sizeobj;
	FL_OBJECT *lcolobj;
	FL_OBJECT *col1obj;
	FL_OBJECT *col2obj;
	FL_OBJECT *align;
	FL_OBJECT *nwgravity;
	FL_OBJECT *segravity;
	FL_OBJECT *inside;
	FL_OBJECT *resize;
	FL_OBJECT *text[4];
} FD_generic_attrib;

extern FD_generic_attrib * create_form_generic_attrib(void);
typedef struct {
	FL_FORM *nullattrib;
	void *vdata;
	char *cdata;
	long  ldata;
} FD_nullattrib;

extern FD_nullattrib * create_form_nullattrib(void);

#endif /* FD_attrib_h_ */
