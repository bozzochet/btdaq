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
 * $Id: browser.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *  Object class Browser
 */
#ifndef FL_BROWSER_H
#define FL_BROWSER_H


/***** Types    *****/
typedef enum
{
    FL_NORMAL_BROWSER,
    FL_SELECT_BROWSER,
    FL_HOLD_BROWSER,
    FL_MULTI_BROWSER
}
FL_BROWSER_TYPE;

/***** Defaults *****/

#define FL_BROWSER_BOXTYPE	FL_DOWN_BOX
#define FL_BROWSER_COL1		FL_COL1
#define FL_BROWSER_COL2		FL_YELLOW
#define FL_BROWSER_LCOL		FL_LCOL
#define FL_BROWSER_ALIGN	FL_ALIGN_BOTTOM

/***** Others   *****/

#define FL_BROWSER_SLCOL	FL_COL1
#define FL_BROWSER_LINELENGTH	2048
#define FL_BROWSER_FONTSIZE     FL_SMALL_FONT

/***** Routines *****/

FL_EXPORT FL_OBJECT *fl_create_browser(
		int type,
		int x,
		int y,
		int w,
		int h,
		const char *l
		);

FL_EXPORT FL_OBJECT *fl_add_browser(
		int type,
		int x,
		int y,
		int w,
		int h,
		const char *l
		);

FL_EXPORT void fl_clear_browser(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_add_browser_line(
		FL_OBJECT *ob,
		const char *newtext
		);

FL_EXPORT void fl_addto_browser(
		FL_OBJECT *ob,
		const char *newtext
		);

FL_EXPORT void fl_addto_browser_chars(
		FL_OBJECT *ob,
		const char *str
		);

#define fl_append_browser  fl_addto_browser_chars
FL_EXPORT void fl_insert_browser_line(
		FL_OBJECT *ob,
		int linenumb,
		const char *newtext
		);

FL_EXPORT void fl_delete_browser_line(
		FL_OBJECT *ob,
		int linenumb
		);

FL_EXPORT void fl_replace_browser_line(
		FL_OBJECT *ob,
		int linenumb,
		const char *newtext
		);

FL_EXPORT const char *fl_get_browser_line(
		FL_OBJECT *ob,
		int linenumb
		);

FL_EXPORT int fl_load_browser(
		FL_OBJECT *ob,
		const char *f
		);


FL_EXPORT void fl_select_browser_line(
		FL_OBJECT *ob,
		int line
		);

FL_EXPORT void fl_deselect_browser_line(
		FL_OBJECT *ob,
		int line
		);

FL_EXPORT void fl_deselect_browser(
		FL_OBJECT *ob
		);

FL_EXPORT int fl_isselected_browser_line(
		FL_OBJECT *ob,
		int line
		);


FL_EXPORT int fl_get_browser_topline(
		FL_OBJECT *ob
		);

FL_EXPORT int fl_get_browser(
		FL_OBJECT *ob
		);

FL_EXPORT int fl_get_browser_maxline(
		FL_OBJECT *ob
		);

FL_EXPORT int fl_get_browser_screenlines(
		FL_OBJECT *ob
		);


FL_EXPORT void fl_set_browser_topline(
		FL_OBJECT *ob,
		int topline
		);

FL_EXPORT void fl_set_browser_fontsize(
		FL_OBJECT *ob,
		int size
		);

FL_EXPORT void fl_set_browser_fontstyle(
		FL_OBJECT *ob,
		int style
		);

FL_EXPORT void fl_set_browser_specialkey(
		FL_OBJECT *ob,
		int specialkey
		);

FL_EXPORT void fl_set_browser_vscrollbar(
		FL_OBJECT *ob,
		int on
		);

FL_EXPORT void fl_set_browser_hscrollbar(
		FL_OBJECT *ob,
		int on
		);

FL_EXPORT void fl_set_browser_line_selectable(
		FL_OBJECT *ob,
		int line,
		int flag
		);

FL_EXPORT void fl_get_browser_dimension(
		FL_OBJECT *ob,
		FL_Coord *x,
		FL_Coord *y,
		FL_Coord *w,
		FL_Coord *h
		);

FL_EXPORT void fl_set_browser_dblclick_callback(
		FL_OBJECT *ob,
		FL_CALLBACKPTR cb,
		long a
		);


FL_EXPORT void fl_set_browser_xoffset(
		FL_OBJECT *ob,
		FL_Coord npixels
		);

FL_EXPORT FL_Coord fl_get_browser_xoffset(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_set_browser_scrollbarsize(
		FL_OBJECT *ob,
		int hh,
		int vw
		);

FL_EXPORT void fl_show_browser_line(
		FL_OBJECT *ob,
		int j
		);

FL_EXPORT int fl_set_default_browser_maxlinelength(
		int n
		);


#ifndef FL_BROWSER_SCROLL_CALLBACKt
#define FL_BROWSER_SCROLL_CALLBACKt
typedef void (*FL_BROWSER_SCROLL_CALLBACK)(FL_OBJECT *ob, int, void *);
#endif

FL_EXPORT void fl_set_browser_hscroll_callback(
		FL_OBJECT *ob,
		FL_BROWSER_SCROLL_CALLBACK cb,
		void *data
		);

FL_EXPORT void fl_set_browser_vscroll_callback(
		FL_OBJECT *ob,
		FL_BROWSER_SCROLL_CALLBACK cb,
		void *data
		);


FL_EXPORT FL_BROWSER_SCROLL_CALLBACK fl_get_browser_hscroll_callback(
		FL_OBJECT *ob
		);

FL_EXPORT FL_BROWSER_SCROLL_CALLBACK fl_get_browser_vscroll_callback(
		FL_OBJECT *ob
		);

#endif
