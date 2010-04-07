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


#ifndef FD_scb_h_
#define FD_scb_h_
/* Header file generated with fdesign. */

/** Callback routines and free object handlers **/

extern void noop_cb(FL_OBJECT *, long);
extern void hide_cb(FL_OBJECT *, long);
extern void deactivate_cb(FL_OBJECT *, long);
extern void done_cb(FL_OBJECT *, long);


/**** Forms and Objects ****/

typedef struct {
	FL_FORM *scb;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *hor;
	FL_OBJECT *hor_thin;
	FL_OBJECT *hor_nice;
	FL_OBJECT *vert;
	FL_OBJECT *vert_thin;
	FL_OBJECT *hide;
	FL_OBJECT *deactivate;
	FL_OBJECT *vert_nice;
} FD_scb;

extern FD_scb * create_form_scb(void);

#endif /* FD_scb_h_ */
