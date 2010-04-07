/*
 *
 * This file is part of XForms.
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
 * $Id: sp_counter.c,v 1.0 2002/05/08 05:27:39 zhao Release $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 * Settting counter class specific attributes.
 *
 */

#include "forms.h"
#include "fd_main.h"
#include "fd_spec.h"
#include "pcounter.h"
#include "spec/counter_spec.h"

extern FD_counterattrib *create_form_counterattrib(void);

static FD_counterattrib *cnt_attrib;
static SuperSPEC *counter_spec;
static void init_spec(SuperSPEC *);

void *
get_counter_spec_fdform(void)
{
    if (!cnt_attrib)
    {
	cnt_attrib = create_form_counterattrib();
	fl_addto_choice(cnt_attrib->returnsetting, get_how_return_str());
	fl_set_choice_item_mode(cnt_attrib->returnsetting, 3, FL_PUP_GRAY);
	fl_set_choice_item_mode(cnt_attrib->returnsetting, 4, FL_PUP_GRAY);
    }
    return cnt_attrib;
}

void
counter_spec_restore(FL_OBJECT * ob, long data)
{
    superspec_to_spec(cnt_attrib->vdata);
    init_spec(get_superspec(cnt_attrib->vdata));
    redraw_the_form(0);
}


static void
init_spec(SuperSPEC * spec)
{
    fl_set_counter_value(cnt_attrib->prec, spec->prec);

    set_finput_value(cnt_attrib->minval, spec->min, spec->prec);
    set_finput_value(cnt_attrib->maxval, spec->max, spec->prec);
    set_finput_value(cnt_attrib->initialval, spec->val, spec->prec);
    set_finput_value(cnt_attrib->step1, spec->sstep, spec->prec);
    set_finput_value(cnt_attrib->step2, spec->lstep, spec->prec);

    fl_set_choice(cnt_attrib->returnsetting, spec->how_return + 1);
}

int
set_counter_attrib(FL_OBJECT * ob)
{
    cnt_attrib->vdata = ob;
    counter_spec = get_superspec(ob);

    fl_set_counter_precision(cnt_attrib->prec, 0);
    fl_set_counter_step(cnt_attrib->prec, 1, 2);
    fl_set_counter_bounds(cnt_attrib->prec, 0, 6);

    if (ob->type == FL_SIMPLE_COUNTER)
	fl_hide_object(cnt_attrib->step2);

    init_spec(counter_spec);

    return 0;
}


void
emit_counter_code(FILE * fp, FL_OBJECT * ob)
{
    FL_OBJECT *defobj;
    SuperSPEC *spec, *defspec;

    if (ob->objclass != FL_COUNTER)
	return;

    /* create a default object */
    defobj = fl_create_counter(ob->type, 0, 0, 0, 0, "");

    defspec = get_superspec(defobj);
    spec = get_superspec(ob);

    if (spec->prec != defspec->prec)
	fprintf(fp, "    fl_set_counter_precision(obj, %d);\n", spec->prec);

    if (spec->min != defspec->min || spec->max != defspec->max)
	fprintf(fp, "    fl_set_counter_bounds(obj, %.*f, %.*f);\n",
		spec->prec, spec->min, spec->prec, spec->max);

    if (spec->val != defspec->val)
	fprintf(fp, "    fl_set_counter_value(obj, %.*f);\n",
		spec->prec, spec->val);

    if (spec->sstep != defspec->sstep || spec->lstep != defspec->lstep)
	fprintf(fp, "    fl_set_counter_step(obj, %.*f, %.*f);\n",
		spec->prec, spec->sstep, spec->prec, spec->lstep);

    if (spec->how_return != defspec->how_return)
	fprintf(fp, "    fl_set_counter_return(obj, %s);\n",
		get_how_return_name(spec->how_return));


    fl_free_object(defobj);
}

void
save_counter_attrib(FILE * fp, FL_OBJECT * ob)
{
    FL_OBJECT *defobj;
    SuperSPEC *defspec, *spec;


    if (ob->objclass != FL_COUNTER)
	return;

    /* create a default object */
    defobj = fl_create_counter(ob->type, 0, 0, 0, 0, "");

    defspec = get_superspec(defobj);
    spec = get_superspec(ob);

    if (spec->min != defspec->min ||
	spec->max != defspec->max)
    {
	fprintf(fp, "  bounds: %.*f %.*f\n",
		spec->prec, spec->min, spec->prec, spec->max);
    }

    if (spec->prec != defspec->prec)
	fprintf(fp, "  precision: %d\n", spec->prec);
    if (spec->val != defspec->val)
	fprintf(fp, "  value: %.*f\n", spec->prec, spec->val);
    if (spec->how_return != defspec->how_return)
	fprintf(fp, "  return: %s\n", get_how_return_name(spec->how_return));
    if (spec->sstep != defspec->sstep)
	fprintf(fp, "  sstep: %.*f\n", spec->prec, spec->sstep);
    if (spec->lstep != defspec->lstep)
	fprintf(fp, "  lstep: %.*f\n", spec->prec, spec->lstep);

    fl_free_object(defobj);
}


void
cnt_precision_cb(FL_OBJECT * ob, long data)
{
    double p = fl_get_counter_value(ob);

    fl_set_counter_precision(cnt_attrib->vdata, p);
    if (auto_apply)
	redraw_the_form(0);
}

void
cnt_minmax_change(FL_OBJECT * ob, long data)
{
    double min = get_finput_value(cnt_attrib->minval, 3);
    double max = get_finput_value(cnt_attrib->maxval, 3);

    fl_set_counter_bounds(cnt_attrib->vdata, min, max);
    if (auto_apply)
	redraw_the_form(0);
}

void
cnt_stepchange_cb(FL_OBJECT * ob, long data)
{
    float s1 = get_finput_value(cnt_attrib->step1, 3);
    float s2 = get_finput_value(cnt_attrib->step2, 3);

    fl_set_counter_step(cnt_attrib->vdata, s1, s2);

    if (auto_apply)
	redraw_the_form(0);
}

void
cnt_initialvalue_change(FL_OBJECT * ob, long data)
{
    double val = get_finput_value(cnt_attrib->initialval, 2);

    fl_set_counter_value(cnt_attrib->vdata, val);
    if (val != fl_get_counter_value(cnt_attrib->vdata))
    {
	counter_spec->val = fl_get_counter_value(cnt_attrib->vdata);
	set_finput_value(cnt_attrib->initialval, counter_spec->val,
			 counter_spec->prec);
    }
    if (auto_apply)
	redraw_the_form(0);
}

void
cnt_returnsetting_change(FL_OBJECT * ob, long data)
{
#if 1
    const char *s = fl_get_choice_text(cnt_attrib->returnsetting);
    fprintf(stderr, "changing how return to %s (%d)\n", s,
	    get_how_return_str_value(s));
    fl_set_counter_return(cnt_attrib->vdata, get_how_return_str_value(s));
#else
    int r = fl_get_choice(cnt_attrib->returnsetting) - 1;
    fl_set_counter_return(cnt_attrib->vdata, r);
#endif
}

#include "spec/counter_spec.c"
