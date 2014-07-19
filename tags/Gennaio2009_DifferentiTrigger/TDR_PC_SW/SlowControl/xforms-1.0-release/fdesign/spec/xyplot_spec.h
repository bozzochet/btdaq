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


/** Header file generated with fdesign on Sun Jan 24 02:14:18 1999.**/

#ifndef FD_xyplotattrib_h_
#define FD_xyplotattrib_h_

/** Callbacks, globals and object handlers **/
extern void grid_change_cb(FL_OBJECT *, long);
extern void xscale_change_cb(FL_OBJECT *, long);
extern void yscale_change_cb(FL_OBJECT *, long);
extern void ymajorminor_change_cb(FL_OBJECT *, long);
extern void xmajorminor_change_cb(FL_OBJECT *, long);
extern void xyplot_returnsetting_change(FL_OBJECT *, long);
extern void gridstyle_change_cb(FL_OBJECT *, long);
extern void markactive_change_cb(FL_OBJECT *, long);


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *xyplotattrib;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *xgrid;
	FL_OBJECT *ygrid;
	FL_OBJECT *xscale;
	FL_OBJECT *yscale;
	FL_OBJECT *ymajor;
	FL_OBJECT *yminor;
	FL_OBJECT *xmajor;
	FL_OBJECT *xminor;
	FL_OBJECT *xbase;
	FL_OBJECT *ybase;
	FL_OBJECT *how_return;
	FL_OBJECT *gridstyle;
	FL_OBJECT *mark_active;
} FD_xyplotattrib;

extern FD_xyplotattrib * create_form_xyplotattrib(void);

#endif /* FD_xyplotattrib_h_ */
