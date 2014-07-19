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


/*
 * $Id: textbox.h,v 0.89 1999/07/17 03:17:54 zhao Beta $
 *
 *  Object textbox. For internal use only
 */
#ifndef FL_TEXTBOX_H
#define FL_TEXTBOX_H		/* { */


/***** Types    *****/
enum
{
    FL_NORMAL_TEXTBOX = FL_NORMAL_BROWSER,
    FL_SELECT_TEXTBOX = FL_SELECT_BROWSER,
    FL_HOLD_TEXTBOX = FL_HOLD_BROWSER,
    FL_MULTI_TEXTBOX = FL_MULTI_BROWSER
};

/***** Defaults *****/

#define FL_TEXTBOX_BOXTYPE	FL_DOWN_BOX
#define FL_TEXTBOX_COL1		FL_COL1
#define FL_TEXTBOX_COL2		FL_YELLOW
#define FL_TEXTBOX_LCOL		FL_LCOL
#define FL_TEXTBOX_ALIGN	FL_ALIGN_BOTTOM

/***** Others   *****/

#define FL_TEXTBOX_LINELENGTH	2048
#define FL_TEXTBOX_FONTSIZE     FL_SMALL_FONT

/***** Routines *****/

extern FL_OBJECT *fl_create_textbox(int, FL_Coord, FL_Coord, FL_Coord,
				    FL_Coord, const char *);
extern FL_OBJECT *fl_add_textbox(int, FL_Coord, FL_Coord, FL_Coord,
				 FL_Coord, const char *);
extern void fl_clear_textbox(FL_OBJECT *);
extern int  fl_set_textbox_topline(FL_OBJECT *, int);
extern int  fl_set_textbox_xoffset(FL_OBJECT *, FL_Coord);
extern FL_Coord fl_get_textbox_xoffset(FL_OBJECT *);
extern int fl_get_textbox_longestline(FL_OBJECT *);
extern void fl_calc_textbox_size(FL_OBJECT *);
extern void fl_add_textbox_line(FL_OBJECT *, const char *);
extern void fl_addto_textbox(FL_OBJECT *, const char *);
extern void fl_addto_textbox_chars(FL_OBJECT *, const char *);
extern const char *fl_get_textbox_line(FL_OBJECT *, int);
extern int fl_get_textbox(FL_OBJECT *);
extern void fl_set_textbox_fontsize(FL_OBJECT *, int);
extern void fl_set_textbox_fontstyle(FL_OBJECT *, int);
extern int fl_load_textbox(FL_OBJECT *, const char *);
extern void fl_select_textbox_line(FL_OBJECT *, int, int);
extern int fl_isselected_textbox_line(FL_OBJECT *, int);
extern void fl_deselect_textbox_line(FL_OBJECT *, int);
extern void fl_deselect_textbox(FL_OBJECT *);
extern void fl_delete_textbox_line(FL_OBJECT *, int);
extern void fl_replace_textbox_line(FL_OBJECT *, int, const char *);
extern void fl_insert_textbox_line(FL_OBJECT *, int, const char *);
extern void fl_set_textbox_dblclick_callback(FL_OBJECT *, FL_CALLBACKPTR, long);
extern void fl_get_textbox_dimension(FL_OBJECT *, int *, int *, int *, int *);
extern void fl_set_textbox_line_selectable(FL_OBJECT *, int, int);
extern int fl_set_textbox_maxlinelength(int);


#endif /* TEXTBOX_H } */
