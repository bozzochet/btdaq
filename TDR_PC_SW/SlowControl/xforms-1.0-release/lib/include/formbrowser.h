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

/********************** crop here for forms.h **********************/

/*
 * $Id: formbrowser.h,v 0.89 1999/07/17 03:17:52 zhao Beta $
 *
 */
#ifndef FL_FORMBROWSER_H
#define FL_FORMBROWSER_H

enum
{
    FL_NORMAL_FORMBROWSER
};

enum 
{
   FL_SMOOTH_SCROLL,
   FL_JUMP_SCROLL
};

#define   FL_FORMBROWSER_BOXTYPE  FL_DOWN_BOX
#define   FL_FORMBROWSER_COL1     FL_COL1
#define   FL_FORMBROWSER_ALIGN    FL_ALIGN_TOP


FL_EXPORT int fl_addto_formbrowser(
		FL_OBJECT *ob,
		FL_FORM *form
		);

FL_EXPORT FL_FORM *fl_delete_formbrowser_bynumber(
		FL_OBJECT *ob,
		int num
		);

FL_EXPORT int fl_delete_formbrowser(
		FL_OBJECT *ob,
		FL_FORM *candidate_form
		);

FL_EXPORT FL_FORM *fl_replace_formbrowser(
		FL_OBJECT *ob,
		int num,
		FL_FORM *form
		);

FL_EXPORT int fl_insert_formbrowser(
		FL_OBJECT *ob,
		int line,
		FL_FORM *new_form
		);

FL_EXPORT int fl_get_formbrowser_area(
		FL_OBJECT *ob,
		int *x,
		int *y,
		int *w,
		int *h
		);

FL_EXPORT void fl_set_formbrowser_scroll(
		FL_OBJECT *ob,
		int how
		);

FL_EXPORT void fl_set_formbrowser_hscrollbar(
		FL_OBJECT *ob,
		int how
		);

FL_EXPORT void fl_set_formbrowser_vscrollbar(
		FL_OBJECT *ob,
		int how
		);

FL_EXPORT FL_FORM *fl_get_formbrowser_topform(
		FL_OBJECT *ob
		);

FL_EXPORT int fl_set_formbrowser_topform(
		FL_OBJECT *ob,
		FL_FORM *form
		);

FL_EXPORT FL_FORM *fl_set_formbrowser_topform_bynumber(
		FL_OBJECT *ob,
		int n
		);


FL_EXPORT int fl_set_formbrowser_xoffset(
		FL_OBJECT *ob,
		int offset
		);

FL_EXPORT int fl_set_formbrowser_yoffset(
		FL_OBJECT *ob,
		int offset
		);

FL_EXPORT int fl_get_formbrowser_xoffset(
		FL_OBJECT *ob
		);

FL_EXPORT int fl_get_formbrowser_yoffset(
		FL_OBJECT *ob
		);


FL_EXPORT int fl_find_formbrowser_form_number(
		FL_OBJECT *ob,
		FL_FORM *candidate_form
		);


FL_EXPORT FL_OBJECT *fl_add_formbrowser(
		int type,
		int x,
		int y,
		int w,
		int h,
		const char *label
		);

FL_EXPORT FL_OBJECT *fl_create_formbrowser(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT int fl_get_formbrowser_numforms(
		FL_OBJECT *ob
		);

#define    fl_get_formbrowser_forms fl_get_formbrowser_numforms
FL_EXPORT FL_FORM *fl_get_formbrowser_form(
		FL_OBJECT *ob,
		int n
		);


#endif /* !def TEXT_H } */

