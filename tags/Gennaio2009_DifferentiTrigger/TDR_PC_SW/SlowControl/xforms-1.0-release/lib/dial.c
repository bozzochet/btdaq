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
 * $Id: dial.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 * dial.c
 *
 *  Default 0 is at 6 oclock and clock-wise is positive.
 */

#if defined(F_ID) || defined(DEBUG)
char *fl_id_dial = "$Id: dial.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#define SIX_OCLOCK 1

#include <math.h>
#include <sys/types.h>
#include <stdlib.h>
#include "forms.h"
#include "pdial.h"

#define SPEC FL_DIAL_SPEC

#ifndef M_PI
#define M_PI 3.1415926f
#endif


static float xo, yo;

#define ROTx(x,y,a) (xo + ((x)-xo)*cosa + ((y)-yo)*sina)
#define ROTy(x,y,a) (yo - ((x)-xo)*sina + ((y)-yo)*cosa)

static void
rotate_it(FL_POINT * xp, float x, float y, float a)
{
    float sina = (float)sin(a);
    float cosa = (float)cos(a);

    xp->x = (short)ROTx(x, y, a);
    xp->y = (short)ROTy(x, y, a);
}

/* Draws a dial */
static void
draw_dial(FL_OBJECT * ob)
{
    FL_Coord x, y, w, h, radius;
    float dangle;
    SPEC *sp = ob->spec;
    FL_POINT xp[6];
    int boxtype, iradius;

    dangle = ((sp->val - sp->b) / sp->a);

    /* since rotate_it always does the rotation in the math way, i.e., 0 at
       three o'clock and CCW, need to translate the current theta into that
       coordiante system */

    if (sp->direction == FL_DIAL_CW)
	dangle = sp->origin - dangle;
    else
	dangle += sp->origin;

    if (dangle < 0)
	dangle += 360;
    else if (dangle > 360)
	dangle -= 360;

    w = ob->w - 3;
    h = ob->h - 3;

    x = xo = ob->x + ob->w / 2;
    y = yo = ob->y + ob->h / 2;

    if (FL_IS_UPBOX(ob->boxtype))
	boxtype = FL_OVAL3D_UPBOX;
    else if (FL_IS_DOWNBOX(ob->boxtype))
	boxtype = FL_OVAL3D_DOWNBOX;
    else if (ob->boxtype == FL_FRAME_BOX)
	boxtype = FL_OVAL3D_FRAMEBOX;
    else if (ob->boxtype == FL_EMBOSSED_BOX)
	boxtype = FL_OVAL3D_EMBOSSEDBOX;
    else
	boxtype = FL_OVAL_BOX;

    /* the dial itself */
    radius = (int) (0.5f * FL_min(w, h));
    iradius = radius - 1;	/* internal radius */

    fl_drw_box(boxtype, x - radius, y - radius, 2 * radius, 2 * radius,
	       ob->col1, ob->bw);

    dangle *= M_PI / 180.0f;

    /* the "hand" */
    if (ob->type == FL_NORMAL_DIAL)
    {
	FL_Coord r;

	r = 0.5f * iradius;
	r = FL_min(15, r);
	rotate_it(xp, x + iradius - 1, y - 2, dangle);
	rotate_it(xp + 1, x + iradius - 1 - r, y - 2, dangle);
	rotate_it(xp + 2, x + iradius - 1 - r, y + 2, dangle);
	rotate_it(xp + 3, x + iradius - 1, y + 2, dangle);
	fl_polygon(1, xp, 4, ob->col2);
    }
    else if (ob->type == FL_LINE_DIAL)
    {
	float dx = 0.1f + (0.08f * iradius), dy = 0.1f + (0.08f * iradius);

	rotate_it(xp, x, y, dangle);
	rotate_it(xp + 1, x + dx, y - dy, dangle);
	rotate_it(xp + 2, x + iradius - 2, y, dangle);
	rotate_it(xp + 3, x + dx, y + dy, dangle);

	fl_polygon(1, xp, 4, ob->col2);
	fl_polygon(0, xp, 4, FL_BLACK);
    }
    else if (ob->type == FL_FILL_DIAL)
    {
	float ti, delta;

	delta = (sp->val - sp->b) / sp->a;
	delta = FL_abs(sp->thetai - delta);
	delta = (sp->direction == FL_DIAL_CW) ? -delta : delta;

	iradius -= boxtype != FL_OVAL_BOX;

	if (sp->direction == FL_DIAL_CCW)
	    ti = sp->thetai + sp->origin;
	else
	    ti = sp->origin - sp->thetai;

	if (ti < 0)
	    ti += 360.0f;
	else if (ti > 360.0f)
	    ti -= 360.0f;

	fl_ovalarc(1, xo - iradius, yo - iradius, 2 * iradius, 2 * iradius,
		   ti * 10, delta * 10, ob->col2);

	rotate_it(xp, xo + iradius - 1, yo, dangle);
	rotate_it(xp + 1, xo + iradius - 1, yo, ti * M_PI / 180.0f);
	fl_simple_line(xo, yo, xp[0].x, xp[0].y, FL_BLACK);
	fl_simple_line(xo, yo, xp[1].x, xp[1].y, FL_BLACK);

	if (boxtype == FL_OVAL_BOX)
	    fl_circ(x, y, iradius, FL_BLACK);
    }

    else
	M_err("DrawDial", "Bad type");

    fl_drw_text_beside(ob->align, ob->x, ob->y, ob->w, ob->h,
		       ob->lcol, ob->lstyle, ob->lsize, ob->label);
}


/* Handle a mouse position change */
static int
handle_mouse(FL_OBJECT * ob, FL_Coord mousex, FL_Coord mousey)
{
    SPEC *sp = (SPEC *) ob->spec;
    double oldv, val, olda;
    double mx, my, angle, range = (sp->max - sp->min);

    oldv = sp->val;
    olda = (oldv - sp->b) / sp->a;

    /* convert to sane FL_coordinate system, i.e., +y up */
    mx = mousex - (ob->x + ob->w * 0.5f);
    my = -mousey + (ob->y + ob->h * 0.5f);

    /* skip clicks very close to center */
    if (fabs(mx) < 2 && fabs(my) < 2)
	return 0;

    /* get angle and normalize to (0,2PI) */
    angle = atan2(my, mx) * 180.0f / M_PI;

    if (sp->direction == FL_DIAL_CW)
	angle = sp->origin - angle;
    else
	angle -= sp->origin;

    while (angle < 0.0f)
	angle += 360.0;

    while (angle > 360.0f)
	angle -= 360.0f;

    val = fl_clamp(sp->a * angle + sp->b, sp->min, sp->max);

    /* check if crossed boundary. Fix it if it did. Fixing is necessary
       otherwise might be unable to reach thetaf(360) */

    if (!sp->cross_over && fabs(oldv - val) > (0.6f * range))
    {
	if (fabs(olda - sp->thetai) < (fabs(olda - sp->thetaf)))
	    angle = sp->thetai;
	else
	    angle = sp->thetaf;
	val = sp->a * angle + sp->b;
    }

    if (sp->step != 0.0)
	val = ((int) (val / sp->step + 0.5f)) * sp->step;

    /* allow a resolution about 0.2 degrees */
    if (fabs(val - oldv) > (range / 1800.0f))
    {
	sp->val = val;
	fl_redraw_object(ob);
	return 1;
    }

    return 0;
}

/* Handles an event */
static int
handle_dial(FL_OBJECT * ob, int event, FL_Coord mx, FL_Coord my,
	    int key, void *ev)
{
    SPEC *sp = (SPEC *) ob->spec;

#if (FL_DEBUG >= ML_DEBUG)
    M_info("HandleDial", fl_event_name(event));
#endif

    switch (event)
    {
    case FL_DRAW:
	draw_dial(ob);
	return 0;
    case FL_DRAWLABEL:
	fl_drw_text_beside(ob->align, ob->x, ob->y, ob->w, ob->h,
			   ob->lcol, ob->lstyle, ob->lsize, ob->label);
	break;
    case FL_PUSH:
	sp->changed = 0;
    case FL_MOUSE:
	if (handle_mouse(ob, mx, my))
	    sp->changed = 1;
	if (sp->changed && sp->how_return == FL_RETURN_CHANGED)
	{
	    sp->changed = 0;
	    return 1;
	}
	else if (sp->how_return == FL_RETURN_ALWAYS)
	    return 1;
	break;
    case FL_RELEASE:
	if (sp->how_return == FL_RETURN_END)
	    return 1;
	else
	    return (sp->changed && sp->how_return == FL_RETURN_END_CHANGED);
    case FL_FREEMEM:
	fl_free(ob->spec);
	return 0;
    }
    return 0;
}

static void
get_mapping(SPEC *sp)
{
    sp->a = (double) (sp->max - sp->min) / (sp->thetaf - sp->thetai);
    sp->b = sp->max - (double) sp->a * sp->thetaf;
}

/* creates an object */
FL_OBJECT *
fl_create_dial(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
	       const char *label)
{
    FL_OBJECT *ob;
    SPEC *sp;

    ob = fl_make_object(FL_DIAL, type, x, y, w, h, label, handle_dial);
    ob->col1 = FL_DIAL_COL1;
    ob->col2 = FL_DIAL_COL2;
    ob->align = FL_DIAL_ALIGN;
    ob->lcol = FL_DIAL_LCOL;
    ob->boxtype = FL_DIAL_BOXTYPE;

    ob->spec_size = sizeof(SPEC);
    sp = ob->spec = calloc(1, sizeof(SPEC));
    sp->min = 0.0f;
    sp->max = 1.0f;
    sp->val = 0.5f;
    sp->step = 0.0f;
    sp->thetai = 0.0f;
    sp->thetaf = 360.0f;
    sp->origin = 270;
    sp->direction = FL_DIAL_CW;
    get_mapping(sp);
    sp->how_return = FL_RETURN_END_CHANGED;

    return ob;
}

/* Adds an object */
FL_OBJECT *
fl_add_dial(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
	    const char *label)
{
    FL_OBJECT *ob;

    ob = fl_create_dial(type, x, y, w, h, label);
    fl_add_object(fl_current_form, ob);
    fl_set_object_dblbuffer(ob, 1);
    return ob;
}

void
fl_set_dial_value(FL_OBJECT * ob, double val)
{
    if (((SPEC *) (ob->spec))->val != val)
    {
	((SPEC *) (ob->spec))->val = val;
	fl_redraw_object(ob);
    }
}

void
fl_set_dial_bounds(FL_OBJECT * ob, double min, double max)
{
    SPEC *sp = ob->spec;

    if (sp->min != min || sp->max != max)
    {
	sp->min = min;
	sp->max = max;
	get_mapping(sp);
	sp->val = fl_clamp(sp->val, sp->min, sp->max);
	fl_redraw_object(ob);
    }
}

void
fl_set_dial_angles(FL_OBJECT * ob, double amin, double amax)
{
    SPEC *sp = ob->spec;

    if (amin < 0)
	amin += 360.0;
    else if (amin > 360)
	amin -= 360.0;

    if (amax < 0)
	amax += 360.0;
    else if (amax > 360)
	amax += 360.0;

    if (sp->thetaf != amax || sp->thetai != amin)
    {
	sp->thetaf = amax;
	sp->thetai = amin;
	get_mapping(sp);
	fl_redraw_object(ob);
    }
}

void
fl_set_dial_cross(FL_OBJECT * ob, int flag)
{
    ((SPEC *) (ob->spec))->cross_over = flag;
}

double
fl_get_dial_value(FL_OBJECT * ob)
{
    return ((SPEC *) (ob->spec))->val;
}


void
fl_get_dial_bounds(FL_OBJECT * ob, double *min, double *max)
{
    *min = ((SPEC *) (ob->spec))->min;
    *max = ((SPEC *) (ob->spec))->max;
}


/* Sets whether to return value all the time */
void
fl_set_dial_return(FL_OBJECT * ob, int value)
{
    ((SPEC *) (ob->spec))->how_return = value;
}

/* Sets the step size to which values are rounded. */
void
fl_set_dial_step(FL_OBJECT * ob, double value)
{
    ((SPEC *) (ob->spec))->step = value;
}

void
fl_set_dial_direction(FL_OBJECT * ob, int dir)
{
    SPEC *sp = ob->spec;

    if (sp->direction != dir)
    {
	sp->direction = dir;
	get_mapping(sp);
	fl_redraw_object(ob);
    }
}
