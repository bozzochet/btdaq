/*
 *
 *  This file is part of the XForms library package.
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


#ifndef FD_cmd_h_
#define FD_cmd_h_
/* Header file generated with fdesign. */

/**** Callback routines ****/

extern void fl_cmdbr_close_cb(FL_OBJECT *, long);
extern void fl_cmdbr_clear_cb(FL_OBJECT *, long);


/**** Forms and Objects ****/

typedef struct {
	FL_FORM *cmd;
	void *vdata;
	long ldata;
	FL_OBJECT *backface;
	FL_OBJECT *browser;
	FL_OBJECT *close_br;
	FL_OBJECT *clear_br;
} FD_cmd;

extern FD_cmd * create_form_cmd(void);

#endif /* FD_cmd_h_ */
