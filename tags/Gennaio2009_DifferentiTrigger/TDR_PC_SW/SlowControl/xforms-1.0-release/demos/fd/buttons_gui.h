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


#ifndef FD_buttform_h_
#define FD_buttform_h_
/* Header file generated with fdesign. */

/**** Callback routines ****/

extern void done_cb(FL_OBJECT *, long);
extern void bw_cb(FL_OBJECT *, long);


/**** Forms and Objects ****/

typedef struct {
	FL_FORM *buttform;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *backface;
	FL_OBJECT *done;
	FL_OBJECT *objsgroup;
	FL_OBJECT *bbutt;
	FL_OBJECT *pbutt;
	FL_OBJECT *bw_obj;
} FD_buttform;

extern FD_buttform * create_form_buttform(void);

#endif /* FD_buttform_h_ */
