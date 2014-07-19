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
 * $Id: slider.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 * slider.c
 *
 */
#if defined(F_ID) || defined(DEBUG)
char *fl_id_slid = "$Id: slider.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include <sys/types.h>
#include <stdlib.h>
#include "forms.h"
#include "pslider.h"

typedef FL_SLIDER_SPEC SPEC;

#define is_normal(t)  (t==FL_HOR_SLIDER || t==FL_VERT_SLIDER)
#define is_fill(t)    (t==FL_VERT_FILL_SLIDER || t==FL_HOR_FILL_SLIDER)
#define is_nice(t)    (t==FL_VERT_NICE_SLIDER || t==FL_HOR_NICE_SLIDER)

#define is_browser(t) (t==FL_HOR_BROWSER_SLIDER  ||  \
                       t==FL_VERT_BROWSER_SLIDER ||  \
                       t==FL_VERT_BROWSER_SLIDER2||  \
                       t==FL_HOR_BROWSER_SLIDER2 ||  \
                       t==FL_VERT_THIN_SLIDER    ||  \
                       t==FL_HOR_THIN_SLIDER    )

#define is_focus(t)   is_browser(t)

enum
{
    COMPLETE = 0, FOCUS = 1, SLIDER_MOTION = 2, SLIDER_JUMP = 4
};


#define VAL_BOXW   FL_max(35, 0.18 * ob->w)	/* reporting boxsize */
#define VAL_BOXH   25		/* vertical RBW      */

static FL_SCROLL_BAR osb;
static FL_SCROLL_BAR slb;

/* due to reporting box, need to compute bounds */
static void
compute_bounds(FL_OBJECT * ob, FL_Coord * x, FL_Coord * y,
	       FL_Coord * w, FL_Coord * h)
{
    FL_Coord delta;

    *x = ob->x;
    *y = ob->y;
    *w = ob->w;
    *h = ob->h;

    if (ob->objclass == FL_VALSLIDER)
    {
	if (is_vslider(ob->type))
	{
	    delta = VAL_BOXH;
	    *y += delta;
	    *h -= delta;
	}
	else if (is_hslider(ob->type))
	{
	    delta = VAL_BOXW;
	    *x += delta;
	    *w -= delta;
	}
    }
}


#define IS_FLATBOX(b) (b==FL_BORDER_BOX||b==FL_FRAME_BOX||\
                       b==FL_EMBOSSED_BOX||b==FL_ROUNDED_BOX)

/* reduce flicker by not painting the location the slider is going to be
 */
static void
draw_motion(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    XRectangle xrec[2];
    int abbw = FL_abs(ob->bw);
    FL_COLOR col;

    if (ob->type != FL_VERT_BROWSER_SLIDER2 &&
	ob->type != FL_HOR_BROWSER_SLIDER2 &&
	ob->type != FL_HOR_THIN_SLIDER &&
	ob->type != FL_VERT_THIN_SLIDER)
    {
	fl_calc_slider_size(ob->boxtype, sp->x, sp->y, sp->w, sp->h,
		       ob->type, sp->slsize, sp->norm_val, &slb, 0, ob->bw);

	if (is_hslider(ob->type))
	{
	    xrec[0].x = sp->x;
	    xrec[0].y = sp->y;
	    xrec[0].width = slb.x - sp->x + 1;
	    xrec[0].height = sp->h;

	    xrec[1].x = slb.x + slb.w - 1;
	    xrec[1].y = sp->y;
	    xrec[1].width = sp->x + sp->w - 1;
	    xrec[1].height = sp->mh;
	}
	else
	{
	    xrec[0].x = sp->x;
	    xrec[0].y = sp->y;
	    xrec[0].width = sp->w;
	    xrec[0].height = slb.y - sp->y;

	    xrec[1].x = sp->x;
	    xrec[1].y = slb.y + slb.h - 1;
	    xrec[1].width = sp->w;
	    xrec[1].height = sp->y + sp->h - 1;

	}

	fl_set_clippings(xrec, 2);
	fl_drw_box(FL_FLAT_BOX, sp->x + abbw, sp->y + abbw,
		   sp->w - 2 * abbw, sp->h - 2 * abbw, ob->col1, 0);
    }
    else if (ob->type == FL_HOR_THIN_SLIDER ||
	     ob->type == FL_VERT_THIN_SLIDER ||
	     ob->type == FL_HOR_BASIC_SLIDER ||
	     ob->type == FL_VERT_BASIC_SLIDER)
    {
	fl_drw_box(FL_FLAT_BOX, sp->x, sp->y, sp->w, sp->h, ob->col1, 1);
    }
    else if (ob->type == FL_HOR_BROWSER_SLIDER2 ||
	     ob->type == FL_VERT_BROWSER_SLIDER2)
    {
	fl_drw_box(ob->boxtype, sp->x, sp->y,
		   sp->w, sp->h, ob->col1, ob->bw > 0 ? 1 : -1);
    }
    else
	fl_drw_box(FL_UP_BOX, sp->x, sp->y,
		   sp->w, sp->h, ob->col1, ob->bw > 0 ? 1 : -1);

    /* for slider jumps, osb is NOT initialized */
    if (is_focus(ob->type) && !(sp->draw_type & SLIDER_JUMP))
    {
	int knob_depth = IS_FLATBOX(ob->boxtype) ? 1 : FL_max(abbw - 1, 1);
	fl_drw_box(FL_DOWN_BOX, osb.x + 1, osb.y + 1, osb.w - 2, osb.h - 2,
		   FL_INACTIVE, knob_depth > 2 ? 2 : knob_depth);
    }

    fl_unset_clipping();

    col = (is_focus(ob->type) && sp->mouse == FL_SLIDER_KNOB) ? FL_MCOL : ob->col2;
    fl_drw_slider(ob->boxtype, sp->x, sp->y, sp->w, sp->h, ob->col1,
		  col, ob->type, sp->slsize, sp->norm_val, "",
		  FL_SLIDER_KNOB, sp->min > sp->max, ob->bw);
}

/* Draws a slider */
static void
draw_slider(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    char valstr[64];
    float val;
    FL_Coord bx = ob->x, by = ob->y, bw = ob->w, bh = ob->h;	/* value box */
    int inv = sp->min > sp->max;

    /* Draw the value box */
    if (ob->objclass == FL_VALSLIDER)
    {
	if (is_vslider(ob->type))
	    bh = VAL_BOXH;
	else
	    bw = VAL_BOXW;

	if (sp->filter)
	    strcpy(valstr, sp->filter(ob, sp->val, sp->prec));
	else
	    sprintf(valstr, "%.*f", sp->prec, sp->val);

	fl_drw_box(ob->boxtype, bx, by, bw, bh, ob->col1, ob->bw);
	fl_drw_text_beside(FL_ALIGN_CENTER, bx, by, bw, bh,
			   ob->lcol, ob->lstyle, ob->lsize, valstr);
    }

    if ((sp->draw_type == SLIDER_MOTION || sp->draw_type == SLIDER_JUMP) &&
	(is_focus(ob->type) || is_normal(ob->type) || is_nice(ob->type)))
    {
	draw_motion(ob);
	return;
    }

    /* Draw the slider */
    if (sp->min == sp->max)
	val = 0.5f;
    else
	val = (sp->val - sp->min) / (sp->max - sp->min);

    if (ob->align == FL_ALIGN_CENTER)
    {
	fl_drw_slider(ob->boxtype, sp->x, sp->y, sp->w, sp->h,
		   ob->col1, ob->col2, ob->type, sp->slsize, val, 
                    is_fill(ob->type) ? "":ob->label,
		      FL_SLIDER_ALL & ~sp->mouse, inv, ob->bw);

        /* added 10/21/00 TCZ: need this to get the inside label right*/
        /* otherwise fl_drw_slider draw lable centered on the filled part!*/
        if(is_fill(ob->type))
            fl_draw_object_label(ob);
    }
    else
    {
	fl_drw_slider(ob->boxtype, sp->x, sp->y, sp->w, sp->h, ob->col1,
		      ob->col2, ob->type, sp->slsize, val, "",
		      FL_SLIDER_ALL & ~sp->mouse, inv, ob->bw);
	fl_draw_object_label_outside(ob);
    }

    if (sp->mouse != FL_SLIDER_NONE)
    {
	fl_drw_slider(ob->boxtype, sp->x, sp->y, sp->w, sp->h, ob->col1,
		      sp->mouse ? FL_MCOL : ob->col2,
		      ob->type, sp->slsize, val, "",
		      sp->mouse, inv, ob->bw);
    }
}

static void
show_focus(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    int inv = (sp->min > sp->max);

    if (sp->lmouse == sp->mouse)
	return;

    if (sp->lmouse != FL_SLIDER_NONE)
	fl_drw_slider(ob->boxtype, sp->x, sp->y, sp->w, sp->h,
		      ob->col1, ob->col2, ob->type,
		      sp->slsize, sp->norm_val, "", sp->lmouse, inv, ob->bw);

    if (sp->mouse != FL_SLIDER_NONE)
	fl_drw_slider(ob->boxtype, sp->x, sp->y, sp->w, sp->h,
		      ob->col1, FL_MCOL, ob->type,
		      sp->slsize, sp->norm_val, "", sp->mouse, inv, ob->bw);
    sp->lmouse = sp->mouse;
}

/* get the value of the mouse position */
static float
get_newvalue(FL_OBJECT * ob, FL_Coord mx, FL_Coord my)
{
    SPEC *sp = (SPEC *) ob->spec;
    float oldval, newval;

    /* Calculate the value */
    if (sp->min == sp->max)
	oldval = 0.5f;
    else
	oldval = (sp->val - sp->min) / (sp->max - sp->min);

    /* get_pos always has newval between (0,1) with 0 on top */
    fl_get_pos_in_slider(sp->mx, sp->my, sp->mw, sp->mh, ob->type, sp->slsize,
			 mx - sp->offx, my - sp->offy, oldval, &newval);

    return (sp->min + newval * (sp->max - sp->min));
}

static int timdel;
static int mpos;		/* < 0 below knob, 0 on knob, > 0 above knob */

/* Handle a mouse position change */
static int
handle_mouse(FL_OBJECT * ob, FL_Coord mx, FL_Coord my, int key)
{
    SPEC *sp = (SPEC *) ob->spec;
    float newval;

    /* mouse on trough */
    if (mpos && (sp->rdelta + sp->ldelta) > 0.0f)
    {
	if (timdel++ == 0 || (timdel > 11 && (timdel & 1) == 0))
	{
	    if (key == FL_LEFT_MOUSE)
		newval = sp->val + mpos * sp->ldelta;
	    else
		newval = sp->val + mpos * sp->rdelta;
	}
	else
	    return 0;
    }
    else
	newval = get_newvalue(ob, mx, my);

    newval = fl_valuator_round_and_clamp(ob, newval);

    if (sp->val != newval)
    {
	sp->val = newval;
	sp->norm_val = (sp->min == sp->max) ? 0.5f :
	    (sp->val - sp->min) / (sp->max - sp->min);
	sp->draw_type = mpos ? SLIDER_JUMP : SLIDER_MOTION;
	fl_redraw_object(ob);
	return 1;
    }
    return 0;
}


static int
focusobj_change(FL_OBJECT * ob, FL_Coord mx, FL_Coord my)
{
    SPEC *sp = ob->spec;

    sp->lmouse = sp->mouse;
    sp->mouse = fl_slider_mouse_object(ob->boxtype, sp->mx, sp->my, sp->mw,
		sp->mh, ob->type, sp->slsize, sp->norm_val, mx, my, ob->bw);
    return sp->lmouse != sp->mouse;
}


/* Handles an event */
static int
handle_it(FL_OBJECT * ob, int event, FL_Coord mx, FL_Coord my,
	  int key, void *ev)
{
    SPEC *sp = (SPEC *) ob->spec;
    int mobj;
    static int lmx, lmy;

#if (FL_DEBUG >= ML_DEBUG)
    M_info2("HandleSlider", fl_event_name(event));
#endif

    switch (event)
    {
    case FL_DRAW:
	compute_bounds(ob, &sp->x, &sp->y, &sp->w, &sp->h);
	ob->align &= ~FL_ALIGN_INSIDE;
	(sp->draw_type == FOCUS ? show_focus : draw_slider) (ob);
	sp->draw_type = COMPLETE;
	break;
    case FL_DRAWLABEL:
	fl_draw_object_label_outside(ob);
	break;
    case FL_LEAVE:
	if (is_focus(ob->type) && sp->mouse == FL_SLIDER_KNOB)
	{
	    sp->mouse = FL_SLIDER_NONE;
	    sp->draw_type = FOCUS;
	    fl_redraw_object(ob);
	    lmx = mx;
	    lmy = my;
	}
	break;
    case FL_ENTER:
	compute_bounds(ob, &sp->mx, &sp->my, &sp->mw, &sp->mh);
    case FL_MOTION:
	if (is_focus(ob->type) && focusobj_change(ob, mx, my))
	{
	    sp->draw_type = FOCUS;
	    fl_redraw_object(ob);
	    lmx = mx;
	    lmy = my;
	}
	break;
    case FL_PUSH:
	timdel = mpos = 0;
	sp->start_val = sp->val;
	sp->offx = sp->offy = 0;
	lmx = lmy = -1;
	mobj = FL_SLIDER_ALL;
	if (!is_fill(ob->type))
	    mobj = fl_slider_mouse_object(ob->boxtype, sp->mx, sp->my, sp->mw,
					  sp->mh, ob->type, sp->slsize,
					  sp->norm_val, mx, my, ob->bw);

	if (mobj == FL_SLIDER_KNOB)
	{
	    fl_calc_slider_size(ob->boxtype, sp->mx, sp->my, sp->mw, sp->mh,
		       ob->type, sp->slsize, sp->norm_val, &osb, 0, ob->bw);
	    sp->offx = mx - (osb.x + osb.w / 2);
	    sp->offy = my - (osb.y + osb.h / 2);

	    if (sp->mx != sp->x)
		fl_calc_slider_size(ob->boxtype, sp->x, sp->y, sp->w, sp->h,
		       ob->type, sp->slsize, sp->norm_val, &osb, 0, ob->bw);
	    lmx = mx;
	    lmy = my;
	}
	else
	{
	    float newval = get_newvalue(ob, mx, my);
	    mpos = (newval > sp->val) ? 1 : -1;
	}
    case FL_MOUSE:
	/* need to fix get_pos before removing this */
	if (mx == lmx && my == lmy && mpos == 0)
	    break;

	handle_mouse(ob, mx, my, key);

	if (sp->how_return == FL_RETURN_CHANGED && sp->start_val != sp->val)
	{
	    sp->start_val = sp->val;
	    return 1;
	}
	else if (sp->how_return == FL_RETURN_ALWAYS)
	    return 1;
	break;
    case FL_RELEASE:
	if (is_focus(ob->type))
	{
	    fl_redraw_object(ob);
	    osb = slb;
	}

	if (sp->how_return == FL_RETURN_END ||
	    sp->how_return == FL_RETURN_ALWAYS)
	    return 1;
	else if (sp->how_return == FL_RETURN_END_CHANGED &&
		 sp->start_val != sp->val)
	{
	    sp->start_val = sp->val;
	    return 1;
	}
	break;
    case FL_FREEMEM:
	fl_free(ob->spec);
	break;
    }

    return 0;
}

/* creates an object */

static FL_OBJECT *
create_it(int objclass, int type, FL_Coord x, FL_Coord y,
	  FL_Coord w, FL_Coord h, const char *label)
{
    FL_OBJECT *ob;
    SPEC *sp;

    ob = fl_make_object(objclass, type, x, y, w, h, label, handle_it);
    ob->boxtype = FL_SLIDER_BOXTYPE;
    ob->col1 = FL_SLIDER_COL1;
    ob->col2 = FL_SLIDER_COL2;
    ob->align = FL_SLIDER_ALIGN;
    ob->lcol = FL_SLIDER_LCOL;
    ob->lsize = FL_TINY_SIZE;

    ob->spec_size = sizeof(SPEC);
    sp = ob->spec = fl_calloc(1, sizeof(SPEC));
    sp->max = 1.0;
    sp->val = sp->norm_val = 0.5;
    sp->start_val = sp->val;
    sp->filter = 0;
    sp->slsize = FL_SLIDER_WIDTH;
    if (is_focus(ob->type))
	sp->slsize *= 1.5;
    sp->prec = 2;
    sp->how_return = FL_RETURN_CHANGED;

    fl_set_object_dblbuffer(ob, 1	/* is_fill(ob->type) ||
					   is_nice(ob->type) */ );

    return ob;
}


/* Adds an object */
static FL_OBJECT *
add_it(int objclass, int type, FL_Coord x, FL_Coord y,
       FL_Coord w, FL_Coord h, const char *label)
{
    FL_OBJECT *ob;

    ob = create_it(objclass, type, x, y, w, h, label);
    fl_add_object(fl_current_form, ob);
    return ob;
}

FL_OBJECT *
fl_create_slider(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		 const char *label)
{
    return create_it(FL_SLIDER, type, x, y, w, h, label);
}

FL_OBJECT *
fl_add_slider(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
	      const char *label)
{
    return add_it(FL_SLIDER, type, x, y, w, h, label);
}

FL_OBJECT *
fl_create_valslider(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		    const char *label)
{
    return create_it(FL_VALSLIDER, type, x, y, w, h, label);
}

FL_OBJECT *
fl_add_valslider(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		 const char *label)
{
    return add_it(FL_VALSLIDER, type, x, y, w, h, label);
}

/*-------------------------------------------*/

void
fl_set_slider_value(FL_OBJECT * ob, double val)
{
    SPEC *sp;
    float smin, smax;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_SLIDER) && !IsValidClass(ob, FL_VALSLIDER))
    {
	Bark("SetSliderValue", "%s is not a slider", ob ? ob->label : "");
	return;
    }
#endif

    sp = ob->spec;
    smin = FL_min(sp->min, sp->max);
    smax = FL_max(sp->min, sp->max);
    val = FL_clamp(val, smin, smax);

    if (sp->val != val)
    {
	sp->val = val;
	sp->norm_val = (sp->min == sp->max) ? 0.5f :
	    (sp->val - sp->min) / (sp->max - sp->min);
	fl_redraw_object(ob);
    }
}

void
fl_set_slider_bounds(FL_OBJECT * ob, double min, double max)
{
    SPEC *sp;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_SLIDER) && !IsValidClass(ob, FL_VALSLIDER))
    {
	Bark("SetSliderBounds", "%s is not a slider", ob ? ob->label : "");
	return;
    }
#endif

    sp = ob->spec;
    if (sp->min != min || sp->max != max)
    {
	sp->min = min;
	sp->max = max;
	if (sp->val < sp->min && sp->val < sp->max)
	    sp->val = FL_min(sp->min, sp->max);
	if (sp->val > sp->min && sp->val > sp->max)
	    sp->val = FL_max(sp->min, sp->max);

	sp->norm_val = (sp->min == sp->max) ? 0.5 :
	    (sp->val - sp->min) / (sp->max - sp->min);

	fl_redraw_object(ob);
    }
}

/* Returns value of the slider */
double
fl_get_slider_value(FL_OBJECT * ob)
{
#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_SLIDER) && !IsValidClass(ob, FL_VALSLIDER))
    {
	Bark("GetSliderValue", "%s is not a slider", ob ? ob->label : "");
	return 0;
    }
#endif
    return ((SPEC *) (ob->spec))->val;
}

/* Returns the slider bounds. */
void
fl_get_slider_bounds(FL_OBJECT * ob, double *min, double *max)
{
    *min = ((SPEC *) (ob->spec))->min;
    *max = ((SPEC *) (ob->spec))->max;
}

/* Sets whether to return value all the time */
void
fl_set_slider_return(FL_OBJECT * ob, int value)
{
    ((SPEC *) (ob->spec))->how_return = value;
}

/* Sets the step size to which values are rounded. */
void
fl_set_slider_step(FL_OBJECT * ob, double value)
{
    ((SPEC *) (ob->spec))->step = value;
}

void
fl_set_slider_increment(FL_OBJECT * ob, double l, double r)
{
    ((SPEC *) (ob->spec))->ldelta = l;
    ((SPEC *) (ob->spec))->rdelta = r;
}

void
fl_get_slider_increment(FL_OBJECT * ob, float *l, float *r)
{
    *l = ((SPEC *) (ob->spec))->ldelta;
    *r = ((SPEC *) (ob->spec))->rdelta;
}

/* Sets the portion of the slider box covered by the slider */
void
fl_set_slider_size(FL_OBJECT * ob, double size)
{
    SPEC *sp = ob->spec;

    if (size <= 0.0)
	size = 0.005;
    else if (size >= 1.0)
	size = 1.0;

    /* impose min knob size limit for scrollbar slider */
    if (is_browser(ob->type))
    {
	float psize, dim;
	dim = is_vslider(ob->type) ? ob->h : ob->w;
	dim -= 2 * FL_abs(ob->bw);
	psize = dim * size;
	if (psize < FL_MINKNOB_SB && dim > 0.0f)
	    size = FL_MINKNOB_SB / dim;
    }

    if (size != sp->slsize)
    {
	sp->slsize = size;
	fl_redraw_object(ob);
    }
}

/* Only for value sliders. */
void
fl_set_slider_precision(FL_OBJECT * ob, int prec)
{
    if (((SPEC *) (ob->spec))->prec != prec)
    {
	if (prec > 10)
	    prec = 10;
	((SPEC *) (ob->spec))->prec = prec;
	fl_redraw_object(ob);
    }
}

void
fl_set_slider_filter(FL_OBJECT * ob, FL_VAL_FILTER filter)
{
    ((SPEC *) (ob->spec))->filter = filter;
}
