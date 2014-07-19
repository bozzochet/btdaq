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


/** Header file generated with fdesign on Tue Oct 12 23:18:57 1999.**/

#ifndef FD_pmain_h_
#define FD_pmain_h_

/** Callbacks, globals and object handlers **/
extern void dismiss_pallete(FL_OBJECT *, long);

extern void pallette_entry_callback(FL_OBJECT *, long);

extern void pallette_entry_callback(FL_OBJECT *, long);

extern void pallette_entry_callback(FL_OBJECT *, long);

extern void pallette_entry_callback(FL_OBJECT *, long);

extern void pallette_entry_callback(FL_OBJECT *, long);


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *pmain;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *folder;
} FD_pmain;

extern FD_pmain * create_form_pmain(void);
typedef struct {
	FL_FORM *buttform;
	void *vdata;
	char *cdata;
	long  ldata;
} FD_buttform;

extern FD_buttform * create_form_buttform(void);
typedef struct {
	FL_FORM *staticform;
	void *vdata;
	char *cdata;
	long  ldata;
} FD_staticform;

extern FD_staticform * create_form_staticform(void);
typedef struct {
	FL_FORM *valuatorform;
	void *vdata;
	char *cdata;
	long  ldata;
} FD_valuatorform;

extern FD_valuatorform * create_form_valuatorform(void);
typedef struct {
	FL_FORM *choiceform;
	void *vdata;
	char *cdata;
	long  ldata;
} FD_choiceform;

extern FD_choiceform * create_form_choiceform(void);
typedef struct {
	FL_FORM *miscform;
	void *vdata;
	char *cdata;
	long  ldata;
} FD_miscform;

extern FD_miscform * create_form_miscform(void);

#endif /* FD_pmain_h_ */
