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
 * $Id: valuator.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *  This file is part of the XForms library package.
 *  Copyright (c) 1998-2002  T.C. Zhao
 *  All rights reserved.
 *
 *
 *  Handle some common valuator tasks. 
 *   TODO: move slider and counter handler here
 */

#include "forms.h"
#include "pvaluator.h"

void *
fl_init_valuator(FL_OBJECT * ob)
{
    FL_VALUATOR_SPEC *sp = ob->spec;

    if (!sp)
	sp = ob->spec = fl_calloc(1, ob->spec_size = sizeof(*sp));

    sp->min = 0.0;
    sp->max = 1.0;
    sp->val = 0.5;
    sp->prec = 2;
    sp->step = 0.01;
    sp->draw_type = COMPLETE_DRAW;

    return sp;
}

#define CROSS_OVER(v,vmin,vmax)  ((v)<(vmin)?(vmax):((v)>(vmax)?(vmin):(v)))


double
fl_valuator_round_and_clamp(FL_OBJECT * ob, double value)
{
    FL_VALUATOR_SPEC *sp = ob->spec;
    double vmin, vmax;

    if (sp->step != 0.0)
    {
	float f = value / sp->step;
	value = sp->step * (int) (f > 0 ? (f + 0.4) : (f - 0.4));
    }

    vmin = FL_min(sp->min, sp->max);
    vmax = FL_max(sp->min, sp->max);

    if(!sp->cross_over)
       return FL_clamp(value, vmin, vmax);
    else
       return CROSS_OVER(value,vmin,vmax);
}

int
fl_valuator_handle_drag(FL_OBJECT * ob, double value)
{
    FL_VALUATOR_SPEC *sp = ob->spec;

    value = fl_valuator_round_and_clamp(ob, value);

    if (value != sp->val)
    {
	sp->val = value;
	sp->draw_type = VALUE_DRAW;
	fl_redraw_object(ob);
	return (sp->how_return == FL_RETURN_CHANGED ||
		sp->how_return == FL_RETURN_ALWAYS);
    }

    return (sp->how_return == FL_RETURN_ALWAYS);
}

int
fl_valuator_handle_release(FL_OBJECT * ob, double value)
{
    FL_VALUATOR_SPEC *sp = ob->spec;

    value = fl_valuator_round_and_clamp(ob, value);

    if (value != sp->val)
    {
	sp->val = value;
	sp->draw_type = VALUE_DRAW;
	fl_redraw_object(ob);
	if (sp->how_return == FL_RETURN_CHANGED)
	    return 1;
    }

    if (sp->start_val != sp->val && (sp->how_return == FL_RETURN_END_CHANGED))
	return 1;

    return (sp->how_return == FL_RETURN_ALWAYS ||
	    sp->how_return == FL_RETURN_END);
}

int
fl_set_valuator_return(FL_OBJECT * ob, int n)
{
    FL_VALUATOR_SPEC *sp = ob->spec;
    int old = sp->how_return;

    sp->how_return = n;

    return old;
}

double
fl_clamp(double val, double min, double max)
{
    double vmin = FL_min(min, max), vmax = FL_max(min, max);
    return FL_clamp(val, vmin, vmax);
}
