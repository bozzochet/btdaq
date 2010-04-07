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
 * $Id: positioner.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *
 */
#if defined(F_ID) || defined(DEBUG)
char *fl_id_posi = "$Id: positioner.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include <sys/types.h>
#include <stdlib.h>
#include "forms.h"
#include "ppositioner.h"

/* The special information for positioners.
 * ymin is the value at the bottom and ymax is the value at the top
 */

#define SPEC FL_POSITIONER_SPEC

/* performs linear interpolation */
static float
flinear(float val, float smin, float smax, float gmin, float gmax)
{
    if (smin == smax)
	return gmax;
    else
	return gmin + (gmax - gmin) * (val - smin) / (smax - smin);
}

/* Draws a positioner */
static void
draw_positioner(FL_OBJECT * ob)
{
    SPEC *sp = ((SPEC *) (ob->spec));
    FL_Coord absbw = FL_abs(ob->bw);
    FL_Coord x1 = ob->x + absbw + 1, y1 = ob->y + absbw + 1;
    FL_Coord w1 = ob->w - 2 * absbw - 2, h1 = ob->h - 2 * absbw - 2;
    FL_Coord xx, yy;
    int oldmode = fl_get_drawmode();
    int newmode = (ob->type == FL_OVERLAY_POSITIONER ? FL_XOR : FL_COPY);


    if (!sp->partial)
    {
	if (ob->type != FL_OVERLAY_POSITIONER)
	    fl_drw_box(ob->boxtype, ob->x, ob->y, ob->w, ob->h, ob->col1, ob->bw);
	fl_draw_object_label_outside(ob);

    }
    else
    {
	long col = (ob->type == FL_OVERLAY_POSITIONER ? ob->col2 : ob->col1);

	xx = flinear(sp->lxval, sp->xmin, sp->xmax, x1, x1 + w1 - 1.0);
	yy = flinear(sp->lyval, sp->ymin, sp->ymax, y1 + h1 - 1.0, y1);

	if (oldmode != newmode)
	    fl_drawmode(newmode);
	fl_diagline(x1, yy, w1, 1, col);
	fl_diagline(xx, y1, 1, h1, col);
    }

    xx = flinear(sp->xval, sp->xmin, sp->xmax, x1, x1 + w1 - 1.0);
    yy = flinear(sp->yval, sp->ymin, sp->ymax, y1 + h1 - 1.0, y1);

    if (oldmode != newmode)
	fl_drawmode(newmode);

    fl_diagline(x1, yy, w1, 1, ob->col2);
    fl_diagline(xx, y1, 1, h1, ob->col2);

    if (oldmode != newmode)
	fl_drawmode(oldmode);
}

/* Handle a mouse position change */
static int
handle_mouse(FL_OBJECT * ob, FL_Coord mx, FL_Coord my)
{
    SPEC *sp = ((SPEC *) (ob->spec));
    FL_Coord absbw = FL_abs(ob->bw);
    FL_Coord x1 = ob->x + absbw + 1, y1 = ob->y + absbw + 1;
    FL_Coord w1 = ob->w - 2 * absbw - 2, h1 = ob->h - 2 * absbw - 2;
    float oldx = sp->xval, oldy = sp->yval;

    sp->xval = flinear(mx, x1, x1 + w1 - 1.0, sp->xmin, sp->xmax);
    sp->yval = flinear(my, y1 + h1 - 1.0, y1, sp->ymin, sp->ymax);

    if (sp->xstep != 0.0)
	sp->xval = ((int) (sp->xval / sp->xstep + 0.5)) * sp->xstep;
    if (sp->ystep != 0.0)
	sp->yval = ((int) (sp->yval / sp->ystep + 0.5)) * sp->ystep;

    /* make sure the position is in bounds */
    sp->xval = fl_clamp(sp->xval, sp->xmin, sp->xmax);
    sp->yval = fl_clamp(sp->yval, sp->ymin, sp->ymax);

    if (sp->xval != oldx || sp->yval != oldy)
    {
	sp->partial = 1;
	sp->lxval = oldx;
	sp->lyval = oldy;
	fl_redraw_object(ob);
	return 1;
    }
    return 0;
}

/* Handles an event */
static int
handle_it(FL_OBJECT * ob, int event, FL_Coord mx, FL_Coord my,
	  int key, void *ev)
{
    SPEC *sp = (SPEC *) ob->spec;

#if (FL_DEBUG >= ML_DEBUG)
    M_info("HandlePositioner", fl_event_name(event));
#endif

    switch (event)
    {
    case FL_DRAW:
	draw_positioner(ob);
	sp->partial = 0;
	break;
    case FL_DRAWLABEL:
	fl_draw_object_label_outside(ob);
	break;
    case FL_PUSH:
	sp->changed = 0;
    case FL_MOUSE:
	if (handle_mouse(ob, mx, my))
	    sp->changed = 1;
	if (sp->how_return == FL_RETURN_CHANGED && sp->changed)
	{
	    sp->changed = 0;
	    return 1;
	}
	else if (sp->how_return == FL_RETURN_ALWAYS)
	    return 1;
	break;
    case FL_RELEASE:
	return ((sp->how_return == FL_RETURN_END) ||
		(sp->how_return == FL_RETURN_END_CHANGED && sp->changed));
    case FL_FREEMEM:
	fl_free(ob->spec);
	break;
    }
    return 0;
}

/*------------------------------*/

/* creates an object */
FL_OBJECT *
fl_create_positioner(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		     const char *label)
{
    FL_OBJECT *ob;
    SPEC *sp;

    ob = fl_make_object(FL_POSITIONER, type, x, y, w, h, label, handle_it);
    ob->boxtype = FL_POSITIONER_BOXTYPE;
    ob->col1 = FL_POSITIONER_COL1;
    ob->col2 = FL_POSITIONER_COL2;
    ob->align = FL_POSITIONER_ALIGN;
    ob->lcol = FL_POSITIONER_LCOL;

    if (ob->type == FL_OVERLAY_POSITIONER)
    {
	ob->bw = 0;
	ob->boxtype = FL_NO_BOX;
    }

    ob->spec_size = sizeof(SPEC);
    sp = ob->spec = fl_calloc(1, ob->spec_size);

    sp->xmin = 0.0;
    sp->ymin = 0.0;
    sp->xmax = 1.0;
    sp->ymax = 1.0;
    sp->xval = sp->lxval = 0.5;
    sp->yval = sp->lyval = 0.5;

    sp->how_return = FL_RETURN_CHANGED;

    return ob;
}

/* Adds an object */
FL_OBJECT *
fl_add_positioner(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		  const char *label)
{
    FL_OBJECT *ob;

    ob = fl_create_positioner(type, x, y, w, h, label);
    fl_add_object(fl_current_form, ob);
    return ob;
}

void
fl_set_positioner_xvalue(FL_OBJECT * ob, double val)
{
    SPEC *sp = ob->spec;

    val = fl_clamp(val, sp->xmin, sp->xmax);

    if (sp->xval != val)
    {
	sp->lxval = sp->xval;
	sp->xval = val;
	fl_redraw_object(ob);
    }
}

void
fl_set_positioner_yvalue(FL_OBJECT * ob, double val)
{
    SPEC *sp = ob->spec;

    val = fl_clamp(val, sp->ymin, sp->ymax);

    if (sp->yval != val)
    {
	sp->lyval = sp->yval;
	sp->yval = val;
	fl_redraw_object(ob);
    }
}

void
fl_set_positioner_xbounds(FL_OBJECT * ob, double min, double max)
{
    SPEC *sp = ob->spec;

    if (sp->xmin != min || sp->xmax != max)
    {
	sp->xmin = min;
	sp->xmax = max;
	sp->xval = fl_clamp(sp->xval, sp->xmin, sp->xmax);
	fl_redraw_object(ob);
    }
}

void
fl_set_positioner_ybounds(FL_OBJECT * ob, double min, double max)
{
    SPEC *sp = ob->spec;

    if (sp->ymin != min || sp->ymax != max)
    {
	sp->ymin = min;
	sp->ymax = max;
	sp->yval = fl_clamp(sp->yval, sp->ymin, sp->ymax);
	fl_redraw_object(ob);
    }
}

double
fl_get_positioner_xvalue(FL_OBJECT * ob)
{
    return ((SPEC *) (ob->spec))->xval;
}

double
fl_get_positioner_yvalue(FL_OBJECT * ob)
{
    return ((SPEC *) (ob->spec))->yval;
}

void
fl_get_positioner_xbounds(FL_OBJECT * ob, double *min, double *max)
{
    *min = ((SPEC *) (ob->spec))->xmin;
    *max = ((SPEC *) (ob->spec))->xmax;
}

void
fl_get_positioner_ybounds(FL_OBJECT * ob, double *min, double *max)
{
    *min = ((SPEC *) (ob->spec))->ymin;
    *max = ((SPEC *) (ob->spec))->ymax;
}

/* Sets the step size to which values are rounded.  */
void
fl_set_positioner_xstep(FL_OBJECT * ob, double value)
{
    ((SPEC *) (ob->spec))->xstep = value;
}

/* Sets the step size to which values are rounded. */
void
fl_set_positioner_ystep(FL_OBJECT * ob, double value)
{
    ((SPEC *) (ob->spec))->ystep = value;
}

/* Sets whether to return value all the time */
void
fl_set_positioner_return(FL_OBJECT * ob, int value)
{
    ((SPEC *) (ob->spec))->how_return = value;
}
