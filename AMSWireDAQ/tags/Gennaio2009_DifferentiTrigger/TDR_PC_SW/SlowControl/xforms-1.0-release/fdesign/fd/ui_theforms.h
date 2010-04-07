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


/** Header file generated with fdesign on Sat Feb 27 23:18:13 1999.**/

#ifndef FD_control_h_
#define FD_control_h_

/** Callbacks, globals and object handlers **/
extern void esc_exit_cb(FL_OBJECT *, long);
extern void func_cb(FL_OBJECT *, long);
extern void form_cb(FL_OBJECT *, long);
extern void group_cb(FL_OBJECT *, long);
extern void object_cb(FL_OBJECT *, long);
extern void addform_cb(FL_OBJECT *, long);
extern void deleteform_cb(FL_OBJECT *, long);
extern void changename_cb(FL_OBJECT *, long);
extern void align_cb(FL_OBJECT *, long);
extern void test_cb(FL_OBJECT *, long);
extern void help_cb(FL_OBJECT *, long);
extern void optionmenu_callback(FL_OBJECT *, long);
extern void filemenu_callback(FL_OBJECT *, long);
extern void formmenu_callback(FL_OBJECT *, long);
extern void objectmenu_callback(FL_OBJECT *, long);
extern void groupmenu_callback(FL_OBJECT *, long);

extern void doalign_cb(FL_OBJECT *, long);
extern void snap_cb(FL_OBJECT *, long);
extern void undoalign_cb(FL_OBJECT *, long);
extern void exitalign_cb(FL_OBJECT *, long);

extern void stoptest_cb(FL_OBJECT *, long);
extern void clearlog_cb(FL_OBJECT *, long);

extern void showhelp_cb(FL_OBJECT *, long);
extern void exithelp_cb(FL_OBJECT *, long);


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *control;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *title;
	FL_OBJECT *selmsg;
	FL_OBJECT *oristatus;
	FL_OBJECT *fkey_group;
	FL_OBJECT *formbrowser;
	FL_OBJECT *groupbrowser;
	FL_OBJECT *sizestatus;
	FL_OBJECT *objectbrowser;
	FL_OBJECT *shortcut_group;
	FL_OBJECT *renameform;
	FL_OBJECT *menubar_group;
	FL_OBJECT *optionmenu;
	FL_OBJECT *filemenu;
	FL_OBJECT *formmenu;
	FL_OBJECT *objectmenu;
	FL_OBJECT *groupmenu;
} FD_control;

extern FD_control * create_form_control(void);
typedef struct {
	FL_FORM *align;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *left;
	FL_OBJECT *hcenter;
	FL_OBJECT *right;
	FL_OBJECT *hequal;
	FL_OBJECT *snapobj;
	FL_OBJECT *bottom;
	FL_OBJECT *vcenter;
	FL_OBJECT *top;
	FL_OBJECT *vequal;
	FL_OBJECT *undo;
	FL_OBJECT *dismiss;
} FD_align;

extern FD_align * create_form_align(void);
typedef struct {
	FL_FORM *test;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *browser;
	FL_OBJECT *stoptesting;
} FD_test;

extern FD_test * create_form_test(void);
typedef struct {
	FL_FORM *helpform;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *browser;
	FL_OBJECT *vbutt;
} FD_helpform;

extern FD_helpform * create_form_helpform(void);

#endif /* FD_control_h_ */
