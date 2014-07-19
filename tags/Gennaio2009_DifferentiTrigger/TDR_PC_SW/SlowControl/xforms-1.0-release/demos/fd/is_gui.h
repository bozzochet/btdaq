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


/** Header file generated with fdesign on Tue Apr 25 23:34:44 2000.**/

#ifndef FD_is_mainform_h_
#define FD_is_mainform_h_

/** Callbacks, globals and object handlers **/
extern void vscroll_callback(FL_OBJECT *, long);
extern void hscroll_callback(FL_OBJECT *, long);
extern void filemenu_callback(FL_OBJECT *, long);

extern void doublebuffer_callback(FL_OBJECT *, long);
extern void windowlevel_callback(FL_OBJECT *, long);

extern void scaling_callback(FL_OBJECT *, long);
extern void switchtopixel_callback(FL_OBJECT *, long);
extern void colorchange_callback(FL_OBJECT *, long);
extern void convolve_callback(FL_OBJECT *, long);
extern void autocrop_callback(FL_OBJECT *, long);

extern void outformat_callback(FL_OBJECT *, long);




/**** Forms and Objects ****/
typedef struct {
	FL_FORM *is_mainform;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *canvas;
	FL_OBJECT *vscroll;
	FL_OBJECT *hscroll;
	FL_OBJECT *tabfolder;
	FL_OBJECT *progressbar;
	FL_OBJECT *status;
	FL_OBJECT *errormsg;
} FD_is_mainform;

extern FD_is_mainform * create_form_is_mainform(void);
typedef struct {
	FL_FORM *viewform;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *doublebuffer;
	FL_OBJECT *headerinfo;
	FL_OBJECT *window;
	FL_OBJECT *level;
	FL_OBJECT *pixmap;
} FD_viewform;

extern FD_viewform * create_form_viewform(void);
typedef struct {
	FL_FORM *processform;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *width;
	FL_OBJECT *height;
	FL_OBJECT *subpixel;
	FL_OBJECT *aspect;
	FL_OBJECT *pixel;
	FL_OBJECT *red;
	FL_OBJECT *green;
	FL_OBJECT *blue;
	FL_OBJECT *colorobj;
	FL_OBJECT *align;
} FD_processform;

extern FD_processform * create_form_processform(void);
typedef struct {
	FL_FORM *saveAs_form;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *formcontainer;
	FL_OBJECT *outformat;
} FD_saveAs_form;

extern FD_saveAs_form * create_form_saveAs_form(void);
typedef struct {
	FL_FORM *annotationform;
	void *vdata;
	char *cdata;
	long  ldata;
} FD_annotationform;

extern FD_annotationform * create_form_annotationform(void);
typedef struct {
	FL_FORM *paintform;
	void *vdata;
	char *cdata;
	long  ldata;
} FD_paintform;

extern FD_paintform * create_form_paintform(void);

#endif /* FD_is_mainform_h_ */
