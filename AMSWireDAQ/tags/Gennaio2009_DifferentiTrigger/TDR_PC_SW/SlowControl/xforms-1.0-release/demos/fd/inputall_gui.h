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


#ifndef FD_input_h_
#define FD_input_h_
/* Header file generated with fdesign. */

/**** Callback routines ****/

extern void input_cb(FL_OBJECT *, long);
extern void done_cb(FL_OBJECT *, long);
extern void hide_show_cb(FL_OBJECT *, long);


/**** Forms and Objects ****/

typedef struct {
	FL_FORM *input;
	void *vdata;
	long ldata;
	FL_OBJECT *norminput;
	FL_OBJECT *multiinput;
	FL_OBJECT *report;
} FD_input;

extern FD_input * create_form_input(void);

#endif /* FD_input_h_ */
