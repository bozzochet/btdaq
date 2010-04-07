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
 * $Id: clipbd.h,v 0.89 1999/07/17 03:17:52 zhao Beta $
 *
 * prototypes for clipboard stuff
 *
 */
#ifndef FL_CLIPBD_H
#define FL_CLIPBD_H

typedef Atom FL_CPTYPE;

typedef int (*FL_LOSE_SELECTION_CB)(FL_OBJECT *ob, long type);
typedef int (*FL_SELECTION_CB)(FL_OBJECT *ob, long type,
			     const void *data, long size);

#define FL_SELECTION_CALLBACK        FL_SELECTION_CB
#define FL_LOSE_SELECTION_CALLBACK   FL_LOSE_SELECTION_CB

FL_EXPORT int fl_stuff_clipboard(
		FL_OBJECT *ob,
		long type,
		const void *data,
		long size,
		FL_LOSE_SELECTION_CB lose_callback
		);


FL_EXPORT int fl_request_clipboard(
		FL_OBJECT *ob,
		long type,
		FL_SELECTION_CB got_it_callback
		);


#endif /* !def CLIPBD_H */

