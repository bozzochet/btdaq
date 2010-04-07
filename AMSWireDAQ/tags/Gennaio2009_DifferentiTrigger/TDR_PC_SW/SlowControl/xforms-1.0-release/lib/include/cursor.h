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
 * $Id: cursor.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 * Cursor defs and prototypes
 *
 */

#ifndef FL_CURSOR_H
#define FL_CURSOR_H

#include <X11/cursorfont.h>

enum
{
    FL_DEFAULT_CURSOR = -1,
    FL_INVISIBLE_CURSOR = -2,
    FL_BUSY_CURSOR = XC_watch,
    FL_CROSSHAIR_CURSOR = XC_tcross,
    FL_KILL_CURSOR = XC_pirate,
    FL_NWARROW_CURSOR = XC_top_left_arrow,
    FL_NEARROW_CURSOR = XC_arrow
};

#ifndef XC_invisible
#define XC_invisible   FL_INVISIBLE_CURSOR
#endif

FL_EXPORT void fl_set_cursor(
		Window win,
		int name
		);

FL_EXPORT void fl_set_cursor_color(
		int name,
		FL_COLOR fg,
		FL_COLOR bg
		);

FL_EXPORT int fl_create_bitmap_cursor(
		const char *source,
		const char *mask,
		int w,
		int h,
		int hotx,
		int hoty
		);

FL_EXPORT int fl_create_animated_cursor(
		int *cur_names,
		int timeout
		);


FL_EXPORT Cursor fl_get_cursor_byname(
		int name
		);


#define fl_reset_cursor(win) fl_set_cursor(win, FL_DEFAULT_CURSOR);

#endif
