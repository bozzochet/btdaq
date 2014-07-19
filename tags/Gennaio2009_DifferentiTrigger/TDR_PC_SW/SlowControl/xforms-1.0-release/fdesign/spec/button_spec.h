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


/** Header file generated with fdesign on Tue May 11 23:52:44 1999.**/

#ifndef FD_buttonattrib_h_
#define FD_buttonattrib_h_

/** Callbacks, globals and object handlers **/
extern void initialval_change(FL_OBJECT *, long);
extern void iconbutton_filename_change(FL_OBJECT *, long);
extern void pixalign_change(FL_OBJECT *, long);
extern void showfocus_change(FL_OBJECT *, long);
extern void lookfor_pixmapfile_cb(FL_OBJECT *, long);
extern void usedata_change(FL_OBJECT *, long);
extern void fullpath_cb(FL_OBJECT *, long);
extern void focusiconbutton_filename_change(FL_OBJECT *, long);
extern void helper_change_cb(FL_OBJECT *, long);


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *buttonattrib;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *background;
	FL_OBJECT *initialval;
	FL_OBJECT *filename;
	FL_OBJECT *pixalign;
	FL_OBJECT *showfocus;
	FL_OBJECT *browse;
	FL_OBJECT *use_data;
	FL_OBJECT *fullpath;
	FL_OBJECT *focus_filename;
	FL_OBJECT *browse2;
	FL_OBJECT *tooltip;
} FD_buttonattrib;

extern FD_buttonattrib * create_form_buttonattrib(void);

#endif /* FD_buttonattrib_h_ */
