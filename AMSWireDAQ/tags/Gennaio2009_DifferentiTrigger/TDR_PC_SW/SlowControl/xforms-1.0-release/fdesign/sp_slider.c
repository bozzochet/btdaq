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
 * $Id: sp_slider.c,v 1.0 2002/05/08 05:27:39 zhao Release $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 * Settting slider class specific attributes.
 *
 */

#include "forms.h"
#include "fd_main.h"
#include "fd_spec.h"
#include "slider_spec.h"

extern FD_sliderattrib *create_form_sliderattrib(void);

static FD_sliderattrib *sl_attrib;
static SuperSPEC *slider_spec;
static void show_spec(SuperSPEC *);

void *
get_slider_spec_fdform(void)
{
    if (!sl_attrib)
    {
	sl_attrib = create_form_sliderattrib();
	fl_clear_choice(sl_attrib->returnsetting);
	fl_addto_choice(sl_attrib->returnsetting, get_how_return_str());
    }
    return sl_attrib;
}


void
slider_spec_restore(FL_OBJECT * ob, long data)
{
    FL_OBJECT *edit_obj;

    edit_obj = sl_attrib->vdata;
    superspec_to_spec(edit_obj);
    redraw_the_form(0);
}

static void
show_spec(SuperSPEC * spec)
{
    set_finput_value(sl_attrib->minval, spec->min, spec->prec);
    set_finput_value(sl_attrib->maxval, spec->max, spec->prec);
    set_finput_value(sl_attrib->initial_val, spec->val, spec->prec);
    set_finput_value(sl_attrib->slsize, spec->slsize, 2);
    set_finput_value(sl_attrib->step, spec->step, 3);
    set_finput_value(sl_attrib->ldelta, spec->ldelta, -1);
    set_finput_value(sl_attrib->rdelta, spec->rdelta, -1);

    fl_set_counter_value(sl_attrib->prec, spec->prec);
    /* fl_call_object_callback(sl_attrib->prec); */

    fl_set_choice_text(sl_attrib->returnsetting,
		       get_how_return_str_name(spec->how_return));
}

#define is_vert(t)     (t==FL_VERT_SLIDER || t==FL_VERT_NICE_SLIDER ||\
                        t==FL_VERT_BROWSER_SLIDER||t==FL_VERT_FILL_SLIDER)

int
set_slider_attrib(FL_OBJECT * ob)
{
    sl_attrib->vdata = ob;
    slider_spec = get_superspec(ob);

    fl_freeze_form(sl_attrib->sliderattrib);

    if (ob->objclass == FL_VALSLIDER)
	fl_show_object(sl_attrib->prec);
    else
	fl_hide_object(sl_attrib->prec);

    fl_set_counter_step(sl_attrib->prec, 1, 2);
    fl_set_counter_precision(sl_attrib->prec, 0);
    fl_set_counter_bounds(sl_attrib->prec, 0, 6);
    fl_redraw_object(sl_attrib->prec);	/* need this to draw counter fully */

    if (is_vert(ob->type))
    {
	fl_set_object_label(sl_attrib->minval, "Value at top");
	fl_set_object_label(sl_attrib->maxval, "Value at bottom");
    }
    else
    {
	fl_set_object_label(sl_attrib->minval, "Value at left");
	fl_set_object_label(sl_attrib->maxval, "Value at right");
    }

    show_spec(slider_spec);

    fl_unfreeze_form(sl_attrib->sliderattrib);

    return 0;
}

void
emit_slider_code(FILE * fp, FL_OBJECT * ob)
{
    FL_OBJECT *defobj;
    SuperSPEC *sp, *defsp;

    if (ob->objclass != FL_SLIDER && ob->objclass != FL_VALSLIDER)
	return;

    /* create a default object */
    defobj = (ob->objclass == FL_SLIDER ?
	 fl_create_slider : fl_create_valslider) (ob->type, 0, 0, 0, 0, "");

    defsp = get_superspec(defobj);
    sp = get_superspec(ob);

    if (sp->prec != defsp->prec)
	fprintf(fp, "    fl_set_slider_precision(obj, %d);\n", sp->prec);

    if (sp->min != defsp->min || sp->max != defsp->max)
    {
	fprintf(fp, "    fl_set_slider_bounds(obj, %g, %g);\n",
		sp->min, sp->max);
    }

    if (sp->val != defsp->val)
	fprintf(fp, "    fl_set_slider_value(obj, %g);\n", sp->val);

    if (sp->slsize != defsp->slsize)
	fprintf(fp, "    fl_set_slider_size(obj, %.2f);\n", sp->slsize);

    if (sp->step != defsp->step)
	fprintf(fp, "    fl_set_slider_step(obj, %g);\n", sp->step);

    if (sp->ldelta != defsp->ldelta || sp->rdelta != defsp->rdelta)
	fprintf(fp, "    fl_set_slider_increment(obj, %g, %g);\n",
		sp->ldelta, sp->rdelta);

    if (sp->how_return != defsp->how_return)
	fprintf(fp, "     fl_set_slider_return(obj, %s);\n",
		get_how_return_name(sp->how_return));

}

void
save_slider_attrib(FILE * fp, FL_OBJECT * ob)
{
    FL_OBJECT *defobj;
    SuperSPEC *defsp, *sp;

    if (ob->objclass != FL_SLIDER && ob->objclass != FL_VALSLIDER)
	return;

    /* create a default object */
    defobj = (ob->objclass == FL_SLIDER ?
	 fl_create_slider : fl_create_valslider) (ob->type, 0, 0, 0, 0, "");

    defsp = get_superspec(defobj);
    sp = get_superspec(ob);

    if (sp->min != defsp->min || sp->max != defsp->max)
	fprintf(fp, "\tbounds: %g %g\n", sp->min, sp->max);
    if (sp->prec != defsp->prec)
	fprintf(fp, "\tprecision: %d\n", sp->prec);
    if (sp->val != defsp->val)
	fprintf(fp, "\tvalue: %g\n", sp->val);
    if (sp->ldelta != defsp->ldelta || sp->rdelta != defsp->rdelta)
	fprintf(fp, "\tincrement: %g %g\n", sp->ldelta, sp->rdelta);
    if (sp->slsize != defsp->slsize)
	fprintf(fp, "\tslsize: %.2f\n", sp->slsize);
    if (sp->step != defsp->step)
	fprintf(fp, "\tstep: %g\n", sp->step);
    if (sp->how_return != defsp->how_return)
	fprintf(fp, "\treturn: %s\n", get_how_return_name(sp->how_return));
}

void
adjust_precision(FL_OBJECT * ob, long data)
{
    double p = fl_get_counter_value(ob);

    fl_set_slider_precision(sl_attrib->vdata, p);
    if (auto_apply)
	redraw_the_form(0);
}

void
minmax_change(FL_OBJECT * ob, long data)
{
    double min = get_finput_value(sl_attrib->minval, 3);
    double max = get_finput_value(sl_attrib->maxval, 3);

    fl_set_slider_bounds(sl_attrib->vdata, min, max);
    if (auto_apply)
	redraw_the_form(0);
}

void
slsize_change(FL_OBJECT * ob, long data)
{
    double slsize = get_finput_value(sl_attrib->slsize, 3);

    fl_set_slider_size(sl_attrib->vdata, slsize);
    if (auto_apply)
	redraw_the_form(0);
}

void
step_change(FL_OBJECT * ob, long data)
{
    double step = get_finput_value(sl_attrib->step, 5);

    fl_set_slider_step(sl_attrib->vdata, step);
    if (auto_apply)
	redraw_the_form(0);
}

void
increment_change(FL_OBJECT * ob, long data)
{
    double l = get_finput_value(sl_attrib->ldelta, -1);
    double r = get_finput_value(sl_attrib->rdelta, -1);

    fl_set_slider_increment(sl_attrib->vdata, l, r);
}

void
initialvalue_change(FL_OBJECT * ob, long data)
{
    double val = get_finput_value(sl_attrib->initial_val, 3);

    fl_set_slider_value(sl_attrib->vdata, val);
    slider_spec->val = fl_get_slider_value(sl_attrib->vdata);
    set_finput_value(sl_attrib->initial_val, slider_spec->val,
		     slider_spec->prec);
    if (auto_apply)
	redraw_the_form(0);
}

void
returnsetting_change(FL_OBJECT * ob, long data)
{
    const char *s = fl_get_choice_text(sl_attrib->returnsetting);
    fl_set_slider_return(sl_attrib->vdata, get_how_return_str_value(s));
}


#include "slider_spec.c"
