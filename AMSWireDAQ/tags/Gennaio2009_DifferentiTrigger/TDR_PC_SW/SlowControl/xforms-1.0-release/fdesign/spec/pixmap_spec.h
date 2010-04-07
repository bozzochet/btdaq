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

#ifndef FD_pixmapattrib_h_
#define FD_pixmapattrib_h_

/** Callbacks, globals and object handlers **/
extern void pixmap_filename_change(FL_OBJECT *, long);
extern void pixmapalign_change(FL_OBJECT *, long);
extern void pixmaplookfor_pixmapfile_cb(FL_OBJECT *, long);
extern void pixmapusedata_change(FL_OBJECT *, long);
extern void pixmapfullpath_cb(FL_OBJECT *, long);


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *pixmapattrib;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *background;
	FL_OBJECT *filename;
	FL_OBJECT *pixalign;
	FL_OBJECT *browse;
	FL_OBJECT *use_data;
	FL_OBJECT *fullpath;
} FD_pixmapattrib;

extern FD_pixmapattrib * create_form_pixmapattrib(void);

#endif /* FD_pixmapattrib_h_ */
