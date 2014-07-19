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


/** Header file generated with fdesign on Sat Oct 23 13:50:08 1999.**/

#ifndef FD_ibcanvas_h_
#define FD_ibcanvas_h_

/** Callbacks, globals and object handlers **/
extern void browse_file(FL_OBJECT *, long);
extern void quit_callback(FL_OBJECT *, long);
extern void convert_type(FL_OBJECT *, long);
extern void write_image(FL_OBJECT *, long);
extern void scale_image(FL_OBJECT *, long);
extern void rotate_image(FL_OBJECT *, long);
extern void flip_image(FL_OBJECT *, long);
extern void show_comments(FL_OBJECT *, long);
extern void window_level(FL_OBJECT *, long);
extern void convolve_it(FL_OBJECT *, long);
extern void tint_it(FL_OBJECT *, long);
extern void enhance_it(FL_OBJECT *, long);
extern void crop_it(FL_OBJECT *, long);
extern void warp_it(FL_OBJECT *, long);
extern void annotate_cb(FL_OBJECT *, long);
extern void render_cb(FL_OBJECT *, long);


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *ibcanvas;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *canvas;
	FL_OBJECT *text;
	FL_OBJECT *status;
	FL_OBJECT *error;
	FL_OBJECT *format;
	FL_OBJECT *xfloat;
	FL_OBJECT *yfloat;
	FL_OBJECT *angle;
	FL_OBJECT *anti_aliasing;
	FL_OBJECT *comments;
	FL_OBJECT *subimage;
	FL_OBJECT *level;
	FL_OBJECT *width;
	FL_OBJECT *annotate;
} FD_ibcanvas;

extern FD_ibcanvas * create_form_ibcanvas(void);

#endif /* FD_ibcanvas_h_ */
