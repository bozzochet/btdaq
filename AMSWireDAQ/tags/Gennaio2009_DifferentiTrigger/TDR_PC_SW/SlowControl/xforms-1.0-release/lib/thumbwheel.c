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
 * $Id: thumbwheel.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *  This file is part of the XForms library package.
 *  Copyright (c) 1998-2002  T.C. Zhao 
 *  All rights reserved.
 *
 *  The thumbwheel
 *
 * TODO:   cursor keys
 *         home key
 *         do we need increments a la slider?
 */
#include "forms.h"
#include <math.h>
#include "ptwheel.h"
#define SPEC   FL_THUMBWHEEL_SPEC

#ifndef M_PI
#define M_PI    3.14159265
#endif

static const float arc = (M_PI * 0.48f);	/* about 90 degrees    */
#define GRID       0.190f	/* angular grid for ridges */
#define NEAR       600.0f	/* near plane distance     */
#define DEFSTEP    0.005

static void
draw(FL_OBJECT * ob)
{
    int h2 = ob->h / 2, w2 = ob->w / 2;
    int absbw = FL_abs(ob->bw);
    float delta = GRID - ((h2 + w2) > 300 ? 0.02f : 0.0f);
    SPEC *sp = ob->spec;
    float yo = ob->y + h2;
    float x0 = ob->x + w2;
    int x, y, w, h;
    float theta;
    float dx, dy;
    float offset = sp->val / (sp->step == 0.0 ? DEFSTEP : sp->step);
    double junk;
    FL_COLOR c1, c2;

    if (sp->draw_type == COMPLETE_DRAW)
	fl_drw_box(ob->boxtype, ob->x, ob->y, ob->w, ob->h, ob->col1, ob->bw);

    x = ob->x + absbw;
    y = ob->y + absbw;
    w = ob->w - 2 * absbw;
    h = ob->h - 2 * absbw;

    if (ob->type == FL_VERT_THUMBWHEEL)
    {
	float extra = h2 * (NEAR / (NEAR - h2) - 0.96f);

	/* (fake) depth-cue */
	fl_rectf(x, y + 1, w, h - 2, FL_DARKER_COL1);
	fl_rectf(x, yo - ob->h / 4, w, ob->h / 2, FL_COL1);
	fl_rectf(x, yo - ob->h / 10, w, ob->h / 5, FL_LIGHTER_COL1);

	for (theta = arc - modf(offset / delta, &junk);
	     theta > -arc; theta -= delta)
	{
	    dy = (h2 + extra) * sin(theta);
	    dx = ob->h - (h2 + extra) * cos(theta);
	    y = yo + FL_nint(dy * NEAR / (NEAR + dx));

	    if (y > ob->y + 3 && y < ob->y + ob->h - 3)
	    {
		int d = 1;
		if (y < yo + h2 / 4 && y > yo - h2 / 4)
		{
		    c1 = FL_LEFT_BCOL;
		    c2 = FL_RIGHT_BCOL;
		}
		else if (y < ob->y + h2 / 5 || y > yo + h2 - h2 / 5)
		{
		    c1 = FL_LIGHTER_COL1;
		    c2 = FL_BOTTOM_BCOL;
		    d = 0;
		}
		else
		{
		    c1 = FL_MCOL;
		    c2 = FL_BOTTOM_BCOL;
		}
		fl_line(x + 1, y - 1, x + w - 2, y - 1, c1);
		fl_line(x + 1, y + d, x + w - 2, y + d, c2);
	    }
	}

	/* bottom */
	y = ob->y + absbw;
	fl_rectf(x - 1, ob->y + ob->h - 6, w, 3, FL_RIGHT_BCOL);

	/* top */
	fl_rectf(x - 1, y, w, 3, FL_RIGHT_BCOL);

	/* left */
	fl_line(x - 1, y, x - 1, y + h - 1, FL_BLACK);

	/* right */
	fl_rectf(x + w - 1, y, 2, h, FL_RIGHT_BCOL);

	/* highlight */
	fl_line(x + 1, yo - h2 + 10, x + 1, yo + h2 - 10, FL_LEFT_BCOL);
    }
    else
    {
	float extra = w2 * (NEAR / (NEAR - w2) - 0.96);

	fl_rectf(x, y, w, h, FL_DARKER_COL1);
	fl_rectf(x0 - w / 4, y, w / 2, h, FL_COL1);
	dx = ob->w / 10;
	fl_rectf(x0 - dx, y, 2 * dx, h, FL_LIGHTER_COL1);

	for (theta = arc + modf(offset / delta, &junk);
	     theta > -arc; theta -= delta)
	{
	    dx = (w2 + extra) * sin(theta);
	    dy = ob->w - (w2 + extra) * cos(theta);
	    x = x0 + FL_nint(dx * NEAR / (NEAR + dy));

	    if (x > ob->x + absbw + 1 && x < x0 + w2 - absbw - 2)
	    {
		int d = 1;

		if (x < x0 + w2 / 4 && x > x0 - w2 / 4)
		{
		    c1 = FL_LEFT_BCOL;
		    c2 = FL_RIGHT_BCOL;
		}
		else if (x < ob->x + (w2 / 4) || x > x0 + w2 - (w2 / 4))
		{
		    c1 = FL_LIGHTER_COL1;
		    c2 = FL_BOTTOM_BCOL;
		}
		else
		{
		    c1 = FL_MCOL;
		    c2 = FL_BOTTOM_BCOL;
		}

		fl_line(x - 1, y + 2, x - 1, yo + h2 - 2 * absbw, c1);
		fl_line(x + d, y + 2, x + d, yo + h2 - 2 * absbw, c2);
	    }

	    x = ob->x + absbw;
	    y = ob->y + absbw;
	    w = ob->w - 2 * absbw;
	    h = ob->h - 2 * absbw;


	    /* top shadow */
	    fl_line(x, y - 1, x + w - 2, y - 1, FL_BLACK);
	    fl_line(x, y, x + w - 4, y, FL_BLACK);
	    /* bottom shadow */
	    fl_line(x + 5, y + h - 2, x + w - 4, y + h - 2, FL_BLACK);
	    fl_line(x, y + h - 1, x + w - 2, y + h - 1, FL_BLACK);
	    fl_line(x, y + h, x + w - 2, y + h, FL_BLACK);
	    /* left & right */
	    fl_rectf(x, y - 1, 3, h + 1, FL_RIGHT_BCOL);
	    fl_rectf(x + w - 4, y - 1, 3, h + 1, FL_RIGHT_BCOL);
	    /* high light */
	    fl_line(x0 - w2 + 11, y + 1, x0 + w2 - 11, y + 1, FL_TOP_BCOL);
	}
    }

    if (sp->draw_type == COMPLETE_DRAW)
	fl_draw_object_label_outside(ob);
}

static int
handle(FL_OBJECT * ob, int ev, int mx, int my, int key, void *xev)
{
    SPEC *sp = ob->spec;
    int cur_pos, old_pos;
    float value;
    double step = sp->step != 0.0 ? sp->step : DEFSTEP;

    switch (ev)
    {
    case FL_DRAW:
	draw(ob);
	sp->draw_type = COMPLETE_DRAW;
	break;
    case FL_DRAWLABEL:
	if (sp->draw_type == COMPLETE_DRAW)
	    fl_draw_object_label_outside(ob);
	break;
    case FL_PUSH:
	sp->oldmx = mx;
	sp->oldmy = my;
	sp->start_val = sp->val;
	/* fall thruough */
    case FL_DRAG:
	cur_pos = ob->type == FL_VERT_THUMBWHEEL ? sp->oldmy : mx;
	old_pos = ob->type == FL_VERT_THUMBWHEEL ? my : sp->oldmx;
	value = sp->val + step * (cur_pos - old_pos);
	sp->oldmx = mx;
	sp->oldmy = my;
	return fl_valuator_handle_drag(ob, value);
    case FL_KEYBOARD:
	value = sp->val;
	if (IsHome(key))
	    value = 0.5f * (sp->min + sp->max);
	else if (IsUp(key) && ob->type == FL_VERT_THUMBWHEEL)
	    value = sp->val + step;
	else if (IsDown(key) && ob->type == FL_VERT_THUMBWHEEL)
	    value = sp->val - step;
	else if (IsRight(key) && ob->type == FL_HOR_THUMBWHEEL)
	    value = sp->val + step;
	else if (IsLeft(key) && ob->type == FL_HOR_THUMBWHEEL)
	    value = sp->val - step;
	return fl_valuator_handle_release(ob, value);
    case FL_RELEASE:
	if (ob->type == FL_VERT_THUMBWHEEL && (key == 4 || key == 5))
	    value = sp->val + (key == 4 ? step : -step);
	else
	    value = sp->val;
	return fl_valuator_handle_release(ob, value);
    }

    return 0;
}


double
fl_get_thumbwheel_value(FL_OBJECT * ob)
{
#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_THUMBWHEEL))
    {
	Bark("GetThumbWheelValue", "%s is not a thumbwheel",
	     ob ? ob->label : "null");
	return 1.0;
    }
#endif

    return ((SPEC *) ob->spec)->val;
}

double
fl_set_thumbwheel_value(FL_OBJECT * ob, double value)
{
    SPEC *sp;
    double oldval;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_THUMBWHEEL))
    {
	Bark("SetThumbWheelValue", "%s is not a thumbwheel",
	     ob ? ob->label : "null");
	return 1.0;
    }
#endif

    sp = ob->spec;
    oldval = sp->val;

    value = FL_clamp(value, sp->min, sp->max);
    if (sp->val != value)
    {
	sp->val = value;
	fl_redraw_object(ob);
    }

    return oldval;
}

void
fl_set_thumbwheel_bounds(FL_OBJECT * ob, double min, double max)
{
    SPEC *sp;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_THUMBWHEEL))
    {
	Bark("SetThumbWheelBounds", "%s is not a thumbwheel",
	     ob ? ob->label : "null");
	return;
    }
#endif

    sp = ob->spec;
    if (sp->min != min || sp->max != max)
    {
	sp->min = min;
	sp->max = max;
	sp->val = FL_clamp(sp->val, min, max);
	fl_redraw_object(ob);
    }

}

void
fl_get_thumbwheel_bounds(FL_OBJECT * ob, double *min, double *max)
{
    *min = ((SPEC *) ob->spec)->min;
    *max = ((SPEC *) ob->spec)->max;
}

double
fl_get_thumbwheel_step(FL_OBJECT * ob)
{
    return ((SPEC *) ob->spec)->step;
}


double
fl_set_thumbwheel_step(FL_OBJECT * ob, double step)
{
    SPEC *sp = ob->spec;
    double old = sp->step;

    if (sp->step != step)
	sp->step = step;

    return old;
}

int fl_set_thumbwheel_crossover(FL_OBJECT *ob, int flag)
{
     SPEC *sp;
     int old;

     if(!ob || !( ob->objclass != FL_THUMBWHEEL) )
         return -1;

     sp = ob->spec;
     old = sp->cross_over;
     sp->cross_over = flag;
     return old;
}

int
fl_set_thumbwheel_return(FL_OBJECT * ob, int how)
{
    return fl_set_valuator_return(ob, how);
}


/* creates an object */
FL_OBJECT *
fl_create_thumbwheel(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		     const char *label)
{
    FL_OBJECT *ob;
    SPEC *sp;

    ob = fl_make_object(FL_THUMBWHEEL, type, x, y, w, h, label, handle);
    ob->col1 = FL_THUMBWHEEL_COL1;
    ob->col2 = FL_THUMBWHEEL_COL2;
    ob->lcol = FL_THUMBWHEEL_LCOL;
    ob->align = FL_THUMBWHEEL_ALIGN;
    ob->boxtype = FL_THUMBWHEEL_BOXTYPE;
    ob->wantkey = FL_KEY_SPECIAL;
    fl_set_object_dblbuffer(ob, 1);

    sp = fl_init_valuator(ob);
    sp->how_return = FL_RETURN_CHANGED;
    sp->step = DEFSTEP;

    return ob;
}

/* Adds an object */
FL_OBJECT *
fl_add_thumbwheel(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		  const char *label)
{
    FL_OBJECT *ob;

    ob = fl_create_thumbwheel(type, x, y, w, h, label);
    fl_add_object(fl_current_form, ob);
    return ob;
}
