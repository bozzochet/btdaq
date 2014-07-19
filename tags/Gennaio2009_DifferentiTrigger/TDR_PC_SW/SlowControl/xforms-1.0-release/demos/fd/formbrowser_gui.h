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


/** Header file generated with fdesign on Sat Feb 27 20:52:53 1999.**/

#ifndef FD_buttonform_h_
#define FD_buttonform_h_

/** Callbacks, globals and object handlers **/


extern void done_cb(FL_OBJECT *, long);
extern void hide_show_cb(FL_OBJECT *, long);
extern void reshow_cb(FL_OBJECT *, long);
extern void scroll_cb(FL_OBJECT *, long);
extern void deactivate_cb(FL_OBJECT *, long);






/**** Forms and Objects ****/
typedef struct {
	FL_FORM *buttonform;
	void *vdata;
	char *cdata;
	long  ldata;
} FD_buttonform;

extern FD_buttonform * create_form_buttonform(void);
typedef struct {
	FL_FORM *staticform;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *chart;
} FD_staticform;

extern FD_staticform * create_form_staticform(void);
typedef struct {
	FL_FORM *mainform;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *done;
	FL_OBJECT *hide;
	FL_OBJECT *show;
	FL_OBJECT *reshow;
	FL_OBJECT *set;
	FL_OBJECT *deactivate;
	FL_OBJECT *formbrowser;
} FD_mainform;

extern FD_mainform * create_form_mainform(void);
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
	FL_OBJECT *pulldown;
	FL_OBJECT *choice;
	FL_OBJECT *browser;
	FL_OBJECT *pushmenu;
} FD_choiceform;

extern FD_choiceform * create_form_choiceform(void);
typedef struct {
	FL_FORM *inputform;
	void *vdata;
	char *cdata;
	long  ldata;
} FD_inputform;

extern FD_inputform * create_form_inputform(void);
typedef struct {
	FL_FORM *big;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *xyplot;
} FD_big;

extern FD_big * create_form_big(void);

#endif /* FD_buttonform_h_ */
