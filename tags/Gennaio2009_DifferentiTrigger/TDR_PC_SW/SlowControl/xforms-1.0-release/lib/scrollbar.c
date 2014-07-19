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
 * $Id: scrollbar.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao
 *  All rights reserved.
 *.
 *
 *  Scrollbar
 */

#include "forms.h"
#include "pscrollbar.h"

#define SPEC FL_SCROLLBAR_SPEC

static void get_geom(FL_OBJECT *);
static void attrib_change(FL_OBJECT *);

#define IsVThin(t)  (t==FL_VERT_THIN_SCROLLBAR||t==FL_VERT_BASIC_SCROLLBAR)
#define IsHThin(t)  (t==FL_HOR_THIN_SCROLLBAR ||t==FL_HOR_BASIC_SCROLLBAR)

static int
handle(FL_OBJECT * ob, int event, FL_Coord mx, FL_Coord my,
       int key, void *ev)
{
    SPEC *spec = ob->spec;

    switch (event)
    {
    case FL_ATTRIB:
	attrib_change(ob);
	get_geom(ob);
	break;
    case FL_DRAW:
	attrib_change(ob);
	get_geom(ob);
	if (IsVThin(ob->type) || IsHThin(ob->type))
	    fl_drw_box(ob->boxtype, ob->x, ob->y, ob->w, ob->h, ob->col1,
		       ob->bw);
    case FL_DRAWLABEL:
	fl_draw_object_label_outside(ob);
	break;
    case FL_FREEMEM:
	/* children will take care of themselves */
	fl_addto_freelist(spec);
	break;
    }
    return 0;
}

#define is_horiz(t)  (t== FL_HOR_SCROLLBAR      ||  \
		      t==FL_HOR_THIN_SCROLLBAR  ||  \
		      t==FL_HOR_NICE_SCROLLBAR  ||  \
		      t==FL_HOR_BASIC_SCROLLBAR)

static void
attrib_change(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;

    sp->slider->col1 = ob->col1;
    sp->slider->col2 = ob->col2;
    sp->up->col1 = sp->down->col1 = ob->col1;
    sp->up->col2 = sp->down->col2 = ob->col2;
    sp->up->boxtype = sp->down->boxtype = sp->slider->boxtype = ob->boxtype;
}

#define IS_FLATBOX(b) (b==FL_BORDER_BOX   || b==FL_FRAME_BOX||\
                       b==FL_EMBOSSED_BOX || b== FL_ROUNDED_BOX)

static void
get_geom(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    FL_OBJECT *up = sp->up, *down = sp->down, *slider = sp->slider;
    int x = ob->x, y = ob->y, w = ob->w, h = ob->h;
    int absbw = FL_abs(ob->bw);
    int t = ob->type;

    if (is_horiz(ob->type))
    {
	down->x = x;
	up->x = x + w - h;
	up->y = down->y = y;
	down->h = up->h = h;
	down->w = up->w = FL_min(w, h);

	slider->x = x + h;
	slider->y = y;
	slider->h = h;

	if ((slider->w = w - 2 * up->w) < 0)
	{
	    slider->w = up->w / 3;
	    slider->x = x + up->w / 3;
	}
    }
    else
    {
	up->x = down->x = x;
	up->y = y;
	up->w = down->w = w;
	up->h = down->h = FL_min(w, h);

	slider->x = x;
	slider->y = y + up->h;
	slider->w = w;

	if ((slider->h = h - 2 * up->h) < 0)
	{
	    slider->h = h / 3;
	    slider->y = y + up->h / 3;
	}
	down->y = y + h - down->h;
    }

    up->bw = ob->bw;
    down->bw = ob->bw;
    slider->bw = ob->bw;

    if (absbw > 2)
	absbw--;

    if (ob->bw > 0)
	up->bw = down->bw = absbw;
    else
	up->bw = down->bw = -absbw;

    if (IsVThin(t) || IsHThin(t))
    {
	absbw = IS_FLATBOX(ob->boxtype) ? 1 : absbw;

	up->boxtype = down->boxtype = FL_NO_BOX;
	up->bw = down->bw = absbw;

	/* due to slider double buffering, have to be completly clear of the
	   scrollbar bounding box, otherwise the slider will wipe out the
	   scrollbar bounding box */
	if (IsVThin(t))
	{
	    slider->x += absbw + 1;
	    slider->w -= 2 * absbw + 2;

	    slider->y -= absbw + (absbw > 1);
	    slider->h += 2 * absbw + (absbw > 1);
	}
	else
	{
	    slider->y += absbw + 1;
	    slider->h -= 2 * absbw + 2;
	    slider->x -= absbw + (absbw > 1);
	    slider->w += 2 * absbw + (absbw > 1);
	}
    }
}

static void
slider_cb(FL_OBJECT * ob, long data)
{
    SPEC *spec = ob->parent->spec;

    fl_call_object_callback(spec->scrollbar);
}

static void
up_cb(FL_OBJECT * ob, long data)
{
    SPEC *spec = ob->parent->spec;
    float oval = fl_get_slider_value(spec->slider), nval;
    double slmax, slmin;

    fl_get_slider_bounds(spec->slider, &slmin, &slmax);
    if (slmax > slmin)
	nval = oval + spec->increment;
    else
	nval = oval - spec->increment;

    fl_set_slider_value(spec->slider, nval);
    nval = fl_get_slider_value(spec->slider);

    if (nval != oval)
	fl_call_object_callback(spec->scrollbar);
}

static void
down_cb(FL_OBJECT * ob, long data)
{
    SPEC *spec = ob->parent->spec;
    float oval = fl_get_slider_value(spec->slider), nval;
    double slmax, slmin;

    fl_get_slider_bounds(spec->slider, &slmin, &slmax);

    if (slmax > slmin)
	nval = oval - spec->increment;
    else
	nval = oval + spec->increment;

    fl_set_slider_value(spec->slider, nval);
    nval = fl_get_slider_value(spec->slider);

    if (nval != oval)
	fl_call_object_callback(spec->scrollbar);
}


FL_OBJECT *
fl_create_scrollbar(int type, int x, int y, int w, int h, const char *l)
{
    SPEC *sp;
    FL_OBJECT *obj;

    obj = fl_make_object(FL_SCROLLBAR, type, x, y, w, h, l, handle);
    obj->spec_size = sizeof(*sp);
    obj->spec = sp = fl_calloc(1, obj->spec_size);
    obj->col1 = obj->col2 = FL_COL1;
    obj->align = FL_ALIGN_BOTTOM;

    if (IsHThin(type) || IsVThin(type))
	obj->boxtype = FL_DOWN_BOX;
    else if (type == FL_HOR_NICE_SCROLLBAR || type == FL_VERT_NICE_SCROLLBAR)
	obj->boxtype = FL_FRAME_BOX;
    else
	obj->boxtype = FL_UP_BOX;

    sp->scrollbar = obj;

    if (is_horiz(type))
    {
	sp->up = fl_create_scrollbutton(FL_TOUCH_BUTTON, 1, 1, 1, 1, "6");
	sp->down = fl_create_scrollbutton(FL_TOUCH_BUTTON, 1, 1, 1, 1, "4");
	fl_set_object_callback(sp->up, up_cb, 0);
	fl_set_object_callback(sp->down, down_cb, 0);
	if (type == FL_HOR_SCROLLBAR)
	    sp->slider = fl_create_slider(FL_HOR_BROWSER_SLIDER2, 1, 1, 1, 1, "");
	else if (type == FL_HOR_THIN_SCROLLBAR)
	    sp->slider = fl_create_slider(FL_HOR_THIN_SLIDER, 1, 1, 1, 1, "");
	else if (type == FL_HOR_BASIC_SCROLLBAR)
	    sp->slider = fl_create_slider(FL_HOR_BASIC_SLIDER, 1, 1, 1, 1, "");
	else if (type == FL_HOR_NICE_SCROLLBAR)
	    sp->slider = fl_create_slider(FL_HOR_NICE_SLIDER, 1, 1, 1, 1, "");
    }
    else
    {
	sp->up = fl_create_scrollbutton(FL_TOUCH_BUTTON, 1, 1, 1, 1, "8");
	sp->down = fl_create_scrollbutton(FL_TOUCH_BUTTON, 1, 1, 1, 1, "2");
	fl_set_object_callback(sp->up, down_cb, 0);
	fl_set_object_callback(sp->down, up_cb, 0);

	if (type == FL_VERT_SCROLLBAR)
	    sp->slider = fl_create_slider(FL_VERT_BROWSER_SLIDER2, 1, 1,
					  1, 1, "");
	else if (type == FL_VERT_THIN_SCROLLBAR)
	    sp->slider = fl_create_slider(FL_VERT_THIN_SLIDER, 1, 1,
					  1, 1, "");
	else if (type == FL_VERT_BASIC_SCROLLBAR)
	    sp->slider = fl_create_slider(FL_VERT_BASIC_SLIDER, 1, 1,
					  1, 1, "");
	else if (type == FL_VERT_NICE_SCROLLBAR)
	    sp->slider = fl_create_slider(FL_VERT_NICE_SLIDER, 1, 1,
					  1, 1, "");
	else
	    M_err("CreateScrollbar", "Unknown type %d", type);

    }

    sp->increment = 0.1;
    fl_set_slider_increment(sp->slider, 0, sp->increment);
    fl_set_object_callback(sp->slider, slider_cb, 0);
    get_geom(obj);

    fl_add_child(obj, sp->slider);
    fl_add_child(obj, sp->down);
    fl_add_child(obj, sp->up);

    return obj;
}




FL_OBJECT *
fl_add_scrollbar(int type, int x, int y, int w, int h, const char *l)
{
    FL_OBJECT *ob = fl_create_scrollbar(type, x, y, w, h, l);

    fl_add_object(fl_current_form, ob);

    return ob;
}

/*
 * User routines
 */

#define NOTSCROLLBAR(o)  (!o || o->objclass != FL_SCROLLBAR)

double
fl_get_scrollbar_value(FL_OBJECT * ob)
{
    SPEC *spec = ob->spec;;

    if (NOTSCROLLBAR(ob))
    {
	M_err("GetScrollBarVal", "%s not a scrollbar", ob ? ob->label : "null");
	return -1000;
    }

    return fl_get_slider_value(spec->slider);
}

void
fl_set_scrollbar_value(FL_OBJECT * ob, double val)
{
    SPEC *spec = ob->spec;;

    if (NOTSCROLLBAR(ob))
    {
	M_err("SetScrollBarVal", "%s not a scrollbar", ob ? ob->label : "null");
	return;
    }

    fl_set_slider_value(spec->slider, val);
}

void
fl_set_scrollbar_size(FL_OBJECT * ob, double val)
{
    SPEC *spec = ob->spec;;

    fl_set_slider_size(spec->slider, val);

}

void
fl_set_scrollbar_increment(FL_OBJECT * ob, double l, double r)
{
    SPEC *spec = ob->spec;;
    fl_set_slider_increment(spec->slider, l, r);
    spec->increment = r;
}

void
fl_get_scrollbar_increment(FL_OBJECT * ob, float *a, float *b)
{
    SPEC *sp = ob->spec;
    fl_get_slider_increment(sp->slider, a, b);
}

void
fl_set_scrollbar_bounds(FL_OBJECT * ob, double b1, double b2)
{
    SPEC *sp = ob->spec;

    if (NOTSCROLLBAR(ob))
    {
	M_err("SetScrollBarBounds", "%s not a scrollbar", ob ? ob->label : "null");
	return;
    }

    fl_set_slider_bounds(sp->slider, b1, b2);
}

void
fl_get_scrollbar_bounds(FL_OBJECT * ob, float *b1, float *b2)
{
    SPEC *sp = ob->spec;
    double d1, d2;

    fl_get_slider_bounds(sp->slider, &d1, &d2);
    *b1 = d1;
    *b2 = d2;
}

void
fl_set_scrollbar_return(FL_OBJECT * ob, int ret)
{
    SPEC *sp = ob->spec;
    fl_set_slider_return(sp->slider, ret);
}


void
fl_set_scrollbar_step(FL_OBJECT * ob, double step)
{
    SPEC *sp = ob->spec;

    fl_set_slider_step(sp->slider, step);
}
