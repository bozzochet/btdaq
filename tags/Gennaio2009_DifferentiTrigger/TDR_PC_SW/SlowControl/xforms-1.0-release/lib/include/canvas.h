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
 * $Id: canvas.h,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 * Header for FL_CANVAS
 *
 */

#ifndef FL_CANVAS_H_
#define FL_CANVAS_H_

typedef enum
{
    FL_NORMAL_CANVAS,
    FL_SCROLLED_CANVAS
}
FL_CANVAS_TYPE;


typedef int (*FL_HANDLE_CANVAS) (FL_OBJECT * ob,
				 Window,
				 int, int,
				 XEvent *, void *);

typedef int (*FL_MODIFY_CANVAS_PROP) (FL_OBJECT *);

/******************** Default *********************/

#define FL_CANVAS_BOXTYPE   FL_DOWN_BOX		/* really the decoration
						   frame */
#define FL_CANVAS_ALIGN     FL_ALIGN_TOP


/************ Interfaces    ************************/


FL_EXPORT FL_OBJECT *fl_create_generic_canvas(
		int canvas_class,
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT FL_OBJECT *fl_add_canvas(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT FL_OBJECT *fl_create_canvas(
		int type,
		FL_Coord x,
		FL_Coord y,
		FL_Coord w,
		FL_Coord h,
		const char *label
		);


FL_EXPORT FL_OBJECT *fl_create_mesacanvas(int, FL_Coord, FL_Coord, FL_Coord,
					  FL_Coord, const char *);


FL_EXPORT FL_OBJECT *fl_add_mesacanvas(int, FL_Coord, FL_Coord, FL_Coord,
				       FL_Coord, const char *);



/* backward compatibility */
#define fl_set_canvas_decoration fl_set_object_boxtype

FL_EXPORT void fl_set_canvas_colormap(
		FL_OBJECT *ob,
		Colormap colormap
		);

FL_EXPORT void fl_set_canvas_visual(
		FL_OBJECT *obj,
		Visual *vi
		);

FL_EXPORT void fl_set_canvas_depth(
		FL_OBJECT *obj,
		int depth
		);

FL_EXPORT void fl_set_canvas_attributes(
		FL_OBJECT *ob,
		unsigned mask,
		XSetWindowAttributes *xswa
		);


FL_EXPORT FL_HANDLE_CANVAS fl_add_canvas_handler(
		FL_OBJECT *ob,
		int ev,
		FL_HANDLE_CANVAS h,
		void *udata
		);


FL_EXPORT Window fl_get_canvas_id(
		FL_OBJECT *ob
		);

FL_EXPORT Colormap fl_get_canvas_colormap(
		FL_OBJECT *ob
		);

FL_EXPORT int fl_get_canvas_depth(
		FL_OBJECT *obj
		);

FL_EXPORT void fl_remove_canvas_handler(
		FL_OBJECT *ob,
		int ev,
		FL_HANDLE_CANVAS h
		);

FL_EXPORT void fl_hide_canvas(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_share_canvas_colormap(
		FL_OBJECT *ob,
		Colormap colormap
		);

FL_EXPORT void fl_clear_canvas(
		FL_OBJECT *ob
		);

FL_EXPORT void fl_modify_canvas_prop(
		FL_OBJECT *obj,
		FL_MODIFY_CANVAS_PROP init,
		FL_MODIFY_CANVAS_PROP activate,
		FL_MODIFY_CANVAS_PROP cleanup
		);


FL_EXPORT void fl_canvas_yield_to_shortcut(
		FL_OBJECT *ob,
		int yes
		);

#if defined(__GLX_glx_h__) || defined(GLX_H)
#include <X11/glcanvas.h>
#endif

#endif
/****FL_CANVAS_H ******/
