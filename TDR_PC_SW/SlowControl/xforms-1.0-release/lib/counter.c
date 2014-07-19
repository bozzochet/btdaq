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
 * $Id: counter.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *  Counter class
 *   only FL_RETURN_END_CHANGED and FL_RETURN_CHANED are meaningful
 */
#if defined(F_ID) || defined(DEBUG)
char *fl_id_cntr = "$Id: counter.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include <string.h>
#include <stdlib.h>
#include "forms.h"
#include "pcounter.h"

#define SPEC FL_COUNTER_SPEC

/* given each component a name. parts are numbered 01 4 23 */
enum
{
    NONE, OB0 = 1, OB1 = 2, OB2 = 4, OB3 = 8, OB4 = 16, ALL = 31
};

static int timdel;		/* Delay since last value change */

/* Draws a counter */
static void
draw_counter(FL_OBJECT * ob)
{
    char str[64];
    int i, btype[5];
    SPEC *sp = (SPEC *) ob->spec;

    /* Compute boxtypes if pushed */
    for (i = 0; i < 5; i++)
	if (ob->pushed && FL_IS_UPBOX(ob->boxtype) &&
	    (sp->mouseobj & (1 << i)))
	    btype[i] = FL_TO_DOWNBOX(ob->boxtype);
	else
	    btype[i] = ob->boxtype;

    if (btype[4] == FL_UP_BOX)
	btype[4] = FL_DOWN_BOX;

    /* Compute sizes. Must not leave any gaps otherwise double buffering will
       not work correctly */
    if (ob->type == FL_NORMAL_COUNTER)
    {
	/* button is numbered 01 4 23 */
	sp->ww[0] = sp->ww[1] = sp->ww[2] = sp->ww[3] = (int)FL_min(0.18f * ob->w, ob->h);
	sp->ww[4] = ob->w - 4 * sp->ww[0];	/* must calculate this way */
	sp->xx[0] = ob->x;
	sp->xx[1] = sp->xx[0] + sp->ww[0];
	sp->xx[4] = sp->xx[1] + sp->ww[1];
	sp->xx[2] = sp->xx[4] + sp->ww[4];
	sp->xx[3] = sp->xx[2] + sp->ww[2];
    }
    else
    {
	/* 1  4  2 */
	sp->ww[1] = sp->ww[2] = (int)FL_min(0.20f * ob->w, ob->h);
	sp->ww[4] = ob->w - 2 * sp->ww[1];
	sp->xx[1] = ob->x;
	sp->xx[4] = ob->x + sp->ww[1];
	sp->xx[2] = sp->xx[4] + sp->ww[4];
    }

    if (sp->filter)
	strcpy(str, sp->filter(ob, sp->val, sp->prec));
    else
	sprintf(str, "%.*f", sp->prec, sp->val);

    /* Only draw the parts that need to be drawn  */
    if (ob->type == FL_NORMAL_COUNTER && (sp->draw_type & OB0))
    {
	fl_drw_box(btype[0], sp->xx[0], ob->y, sp->ww[0], ob->h,
		   ob->col1, ob->bw);
	fl_drw_text(FL_ALIGN_CENTER, sp->xx[0], ob->y, sp->ww[0], ob->h,
		    ob->col2, 0, 0, "@#<<");
    }

    if ((sp->draw_type & OB1))
    {
	fl_drw_box(btype[1], sp->xx[1], ob->y, sp->ww[1], ob->h, ob->col1,
		   ob->bw);
	fl_drw_text(FL_ALIGN_CENTER, sp->xx[1], ob->y, sp->ww[1], ob->h,
		    ob->col2, 0, 0, "@#<");
    }

    if ((sp->draw_type & OB4))
    {
	fl_drw_box(btype[4], sp->xx[4], ob->y, sp->ww[4], ob->h,
		   ob->col1, ob->bw);
	fl_drw_text(FL_ALIGN_CENTER, sp->xx[4], ob->y, sp->ww[4], ob->h,
		    ob->lcol, ob->lstyle, ob->lsize, str);
    }

    if ((sp->draw_type & OB2))
    {
	fl_drw_box(btype[2], sp->xx[2], ob->y, sp->ww[2], ob->h,
		   ob->col1, ob->bw);
	fl_drw_text(FL_ALIGN_CENTER, sp->xx[2], ob->y, sp->ww[2], ob->h,
		    ob->col2, 0, 0, "@#>");
    }

    if (ob->type == FL_NORMAL_COUNTER && (sp->draw_type & OB3))
    {
	fl_drw_box(btype[3], sp->xx[3], ob->y, sp->ww[3], ob->h,
		   ob->col1, ob->bw);
	fl_drw_text(FL_ALIGN_CENTER, sp->xx[3], ob->y, sp->ww[3], ob->h,
		    ob->col2, 0, 0, "@#>>");
    }

    if (sp->draw_type == ALL)
	fl_draw_object_label_outside(ob);

    sp->draw_type = ALL;
}

/* buttons are numbered as 01 4 23 */
static void
calc_mouse_obj(FL_OBJECT * ob, FL_Coord mx, FL_Coord my)
{
    SPEC *sp = (SPEC *) ob->spec;

    sp->mouseobj = NONE;

    if (my < ob->y || my > ob->y + ob->h || mx < ob->x)
	return;

    /* 01 4 23 */
    if (ob->type == FL_NORMAL_COUNTER)
    {
	if (mx < ob->x + sp->ww[0])
	    sp->mouseobj = OB0;
	else if (mx < sp->ww[1] + sp->xx[1])
	    sp->mouseobj = OB1;
	else if (mx < sp->ww[4] + sp->xx[4])
	    sp->mouseobj = OB4;
	else if (mx < sp->ww[2] + sp->xx[2])
	    sp->mouseobj = OB2;
	else if (mx < sp->ww[3] + sp->xx[3])
	    sp->mouseobj = OB3;
    }
    else
    {
	/* 1  4  2 */
	if (mx < ob->x + sp->ww[1])
	    sp->mouseobj = OB1;
	else if (mx < sp->xx[4] + sp->ww[4])
	    sp->mouseobj = OB4;
	else if (mx < sp->xx[2] + sp->ww[2])
	    sp->mouseobj = OB2;
    }

}

/* handles an event on ob */
static int
handle_mouse(FL_OBJECT * ob, int event, FL_Coord mx, FL_Coord my)
{
    SPEC *sp = (SPEC *) ob->spec;
    int changeval = 0;

    if (event == FL_RELEASE)
    {
	sp->mouseobj = NONE;
	fl_redraw_object(ob);
	return 0;
    }
    else if (event == FL_PUSH)
    {
	calc_mouse_obj(ob, mx, my);
	if (sp->mouseobj != NONE)
	    changeval = 1;
	timdel = 1;
    }
    else if (event == FL_MOUSE && sp->mouseobj != NONE)
	changeval = (timdel++ > 10 && (timdel & 1) == 0);

    if (changeval)
    {
	double oval = sp->val;

	if (sp->mouseobj == OB0)
	    sp->val -= sp->lstep;
	if (sp->mouseobj == OB1)
	    sp->val -= sp->sstep;
	if (sp->mouseobj == OB2)
	    sp->val += sp->sstep;
	if (sp->mouseobj == OB3)
	    sp->val += sp->lstep;

	sp->val = fl_clamp(sp->val, sp->min, sp->max);

	if (sp->val != oval)
	{
	    sp->draw_type = sp->mouseobj | OB4;
	    fl_redraw_object(ob);
	}
    }
    return changeval;
}

/* show which button is active by highlighting the button */
static void
show_focus_obj(FL_OBJECT * ob, FL_Coord mx, FL_Coord my)
{
    SPEC *sp = ob->spec;
    unsigned int oldobj = sp->mouseobj;

    calc_mouse_obj(ob, mx, my);

    /* if same object, do nothing */
    if (sp->mouseobj == oldobj)
	return;

    if (sp->mouseobj && sp->mouseobj != OB4 && sp->mouseobj != oldobj)
    {
	FL_COLOR old = ob->col1;
	sp->draw_type = sp->mouseobj;
	ob->col1 = FL_MCOL;
	fl_redraw_object(ob);
	sp->draw_type = oldobj;
	ob->col1 = old;
	fl_redraw_object(ob);
    }
    else if ((sp->mouseobj == NONE || sp->mouseobj == OB4) && oldobj)
    {
	sp->draw_type = oldobj;
	fl_redraw_object(ob);
    }
}


/* Handles an event */
static int
handle_counter(FL_OBJECT * ob, int event, FL_Coord mx, FL_Coord my,
	       int key, void *ev)
{
    SPEC *sp = (SPEC *) ob->spec;

#if (FL_DEBUG >= ML_DEBUG)
    M_info2("HandleCounter", fl_event_name(event));
#endif

    switch (event)
    {
    case FL_DRAW:
	draw_counter(ob);
	break;
    case FL_DRAWLABEL:
	fl_draw_object_label_outside(ob);
	break;
    case FL_PUSH:
	sp->changed = handle_mouse(ob, event, mx, my);

	if (sp->how_return == FL_RETURN_CHANGED && sp->changed)
	{
	    sp->changed = 0;
	    return 1;
	}
	break;
    case FL_MOUSE:
	if (handle_mouse(ob, event, mx, my))
	    sp->changed = 1;

	if (sp->how_return == FL_RETURN_CHANGED && sp->changed)
	{
	    sp->changed = 0;
	    return 1;
	}
	break;
    case FL_RELEASE:
	if (handle_mouse(ob, event, mx, my))
	    sp->changed = 1;
	else if (sp->how_return == FL_RETURN_END_CHANGED && sp->changed)
	    return 1;
	break;
    case FL_ENTER:
    case FL_MOTION:
    case FL_LEAVE:
	show_focus_obj(ob, mx, my);
	break;
    case FL_FREEMEM:
	fl_free(ob->spec);
	break;
    }
    return 0;
}

/*------------------------------*/

/* creates an object */
FL_OBJECT *
fl_create_counter(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		  const char *label)
{
    FL_OBJECT *ob;
    SPEC *sp;

    ob = fl_make_object(FL_COUNTER, type, x, y, w, h, label, handle_counter);
    ob->boxtype = FL_COUNTER_BOXTYPE;
    ob->col1 = FL_COUNTER_COL1;
    ob->col2 = FL_COUNTER_COL2;
    ob->align = FL_COUNTER_ALIGN;
    ob->lcol = FL_COUNTER_LCOL;

    /* counter has a different default */
    if (ob->bw == FL_BOUND_WIDTH && ob->bw == 3)
	ob->bw = FL_COUNTER_BW;

    ob->spec_size = sizeof(SPEC);
    ob->spec = fl_calloc(1, sizeof(SPEC));
    sp = ((SPEC *) (ob->spec));
    sp->min = -1000000.0;
    sp->max = 1000000.0;
    sp->sstep = 0.1;
    sp->lstep = 1.0;
    sp->val = 0.0;
    sp->prec = 1;
    sp->mouseobj = NONE;
    sp->draw_type = ALL;
    sp->how_return = FL_RETURN_END_CHANGED;
    return ob;
}

/* Adds an object */
FL_OBJECT *
fl_add_counter(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
	       const char *label)
{
    FL_OBJECT *ob;

    ob = fl_create_counter(type, x, y, w, h, label);
    fl_add_object(fl_current_form, ob);
    return ob;
}

void
fl_set_counter_value(FL_OBJECT * ob, double val)
{
    SPEC *sp;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_COUNTER))
    {
	Bark("SetCounterValue", "%s not a counter", ob ? ob->label : "");
	return;
    }
#endif

    sp = ob->spec;
    val = fl_clamp(val, sp->min, sp->max);
    if (sp->val != val)
    {
	sp->val = val;
	sp->draw_type = (ob->visible && ob->form->visible) ? OB4 : ALL;
	fl_redraw_object(ob);
    }
}

void
fl_get_counter_bounds(FL_OBJECT * ob, double *min, double *max)
{
    *min = ((SPEC *) (ob->spec))->min;
    *max = ((SPEC *) (ob->spec))->max;
}

void
fl_set_counter_bounds(FL_OBJECT * ob, double min, double max)
{
    SPEC *sp;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_COUNTER))
    {
	Bark("CounterBounds", "%s not a counter", ob ? ob->label : "");
	return;
    }
#endif
    sp = ob->spec;
    if (sp->min != min || sp->max != max)
    {
	sp->min = min;
	sp->max = max;
	sp->val = fl_clamp(sp->val, sp->min, sp->max);
	fl_redraw_object(ob);
    }
}

void
fl_set_counter_step(FL_OBJECT * ob, double s, double l)
{
    SPEC *sp = ob->spec;

    if (sp->sstep != s || sp->lstep != l)
    {
	sp->sstep = s;
	sp->lstep = l;
	fl_redraw_object(ob);
    }
}

void
fl_get_counter_step(FL_OBJECT * ob, float *s, float *l)
{
    SPEC *sp = ob->spec;

    *s = (float)sp->sstep;
    *l = (float)sp->lstep;
}

void
fl_set_counter_precision(FL_OBJECT * ob, int prec)
{
    if (((SPEC *) (ob->spec))->prec != prec)
    {
	((SPEC *) (ob->spec))->prec = prec;
	fl_redraw_object(ob);
    }
}

double
fl_get_counter_value(FL_OBJECT * ob)
{
#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_COUNTER))
    {
	Bark("GetCounterValue", "%s not a counter", ob ? ob->label : "");
	return 0;
    }
#endif
    return ((SPEC *) (ob->spec))->val;
}

/* Sets whether to return value all the time */
void
fl_set_counter_return(FL_OBJECT * ob, int how)
{
    SPEC *sp = ob->spec;

    sp->how_return = how;
    if (sp->how_return == FL_RETURN_END)
	sp->how_return = FL_RETURN_END_CHANGED;
    else if (sp->how_return == FL_RETURN_ALWAYS)
	sp->how_return = FL_RETURN_CHANGED;
}

void
fl_set_counter_filter(FL_OBJECT * ob, FL_VAL_FILTER filter)
{
    ((SPEC *) (ob->spec))->filter = filter;
}
