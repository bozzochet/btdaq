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
 * $Id: sp_scrollbar.c,v 1.0 2002/05/08 05:27:39 zhao Release $
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
#include "scrollbar_spec.h"

extern FD_scrollbarattrib *create_form_scrollbarattrib(void);

static FD_scrollbarattrib *scb_attrib;
static SuperSPEC *scb_spec;
static void show_spec(SuperSPEC *);

void *
get_scrollbar_spec_fdform(void)
{
    if (!scb_attrib)
    {
	scb_attrib = create_form_scrollbarattrib();
	fl_addto_choice(scb_attrib->returnsetting, get_how_return_str());
    }

    return scb_attrib;
}

void
scrollbar_spec_restore(FL_OBJECT * ob, long data)
{
    FL_OBJECT *edited = scb_attrib->vdata;

    superspec_to_spec(edited);
    show_spec(get_superspec(edited));
    redraw_the_form(0);
}

static void
show_spec(SuperSPEC * spec)
{
    set_finput_value(scb_attrib->minval, spec->min, -1);
    set_finput_value(scb_attrib->maxval, spec->max, -1);
    set_finput_value(scb_attrib->initial_val, spec->val, -1);
    set_finput_value(scb_attrib->slsize, spec->slsize, 2);
    set_finput_value(scb_attrib->step, spec->step, 3);
    set_finput_value(scb_attrib->ldelta, spec->ldelta, -1);
    set_finput_value(scb_attrib->rdelta, spec->rdelta, -1);

    fl_set_choice_text(scb_attrib->returnsetting,
		       get_how_return_str_name(spec->how_return));
}

#define is_vert(t)     (t==FL_VERT_SCROLLBAR || t==FL_VERT_NICE_SCROLLBAR ||\
                        t==FL_VERT_THIN_SCROLLBAR)

/*
 * Entry point
 */
int
set_scrollbar_attrib(FL_OBJECT * ob)
{
    scb_attrib->vdata = ob;
    scb_spec = get_superspec(ob);

    if (is_vert(ob->type))
    {
	fl_set_object_label(scb_attrib->minval, "Value at top");
	fl_set_object_label(scb_attrib->maxval, "Value at bottom");
    }
    else
    {
	fl_set_object_label(scb_attrib->minval, "Value at left");
	fl_set_object_label(scb_attrib->maxval, "Value at right");
    }

    show_spec(scb_spec);

    return 0;
}

void
emit_scrollbar_code(FILE * fp, FL_OBJECT * ob)
{
    FL_OBJECT *defobj;
    SuperSPEC *spec, *defspec;

    if (ob->objclass != FL_SCROLLBAR)
	return;

    /* create a default object */
    defobj = fl_create_scrollbar(ob->type, 0, 0, 30, 30, "");

    defspec = get_superspec(defobj);
    spec = get_superspec(ob);

    if (spec->min != defspec->min || spec->max != defspec->max)
    {
	fprintf(fp, "    fl_set_scrollbar_bounds(obj, %g, %g);\n",
		spec->min, spec->max);
    }

    if (spec->val != defspec->val)
	fprintf(fp, "    fl_set_scrollbar_value(obj, %g);\n", spec->val);

    if (spec->slsize != defspec->slsize)
	fprintf(fp, "    fl_set_scrollbar_size(obj, %.2f);\n", spec->slsize);

    if (spec->step != defspec->step)
	fprintf(fp, "    fl_set_scrollbar_step(obj, %g);\n", spec->step);

    if (spec->ldelta != defspec->ldelta || spec->rdelta != defspec->rdelta)
	fprintf(fp, "    fl_set_scrollbar_increment(obj, %g, %g);\n",
		spec->ldelta, spec->rdelta);

    if (spec->how_return != defspec->how_return)
	fprintf(fp, "     fl_set_scrollbar_return(obj, %s);\n",
		get_how_return_name(spec->how_return));

}

void
save_scrollbar_attrib(FILE * fp, FL_OBJECT * ob)
{
    FL_OBJECT *defobj;
    SuperSPEC *defsp, *sp;

    if (ob->objclass != FL_SCROLLBAR)
	return;

    /* create a default object */
    defobj = fl_create_scrollbar(ob->type, 0, 0, 50, 50, "");

    defsp = get_superspec(defobj);
    sp = get_superspec(ob);

    if (sp->min != defsp->min || sp->max != defsp->max)
	fprintf(fp, "\tbounds: %g %g\n", sp->min, sp->max);
    if (sp->val != defsp->val)
	fprintf(fp, "\tvalue: %g\n", sp->val);
    if (sp->slsize != defsp->slsize)
	fprintf(fp, "\tslsize: %.2f\n", sp->slsize);
    if (sp->step != defsp->step)
	fprintf(fp, "\tstep: %.3f\n", sp->step);
    if (sp->rdelta != defsp->rdelta || sp->ldelta != defsp->ldelta)
	fprintf(fp, "\tincrement: %g %g\n", sp->ldelta, sp->rdelta);
    if (sp->how_return != defsp->how_return)
	fprintf(fp, "\treturn: %s\n", get_how_return_name(sp->how_return));
}


void
scb_minmax_change(FL_OBJECT * ob, long data)
{
    double min = get_finput_value(scb_attrib->minval, 3);
    double max = get_finput_value(scb_attrib->maxval, 3);

    fl_set_scrollbar_bounds(scb_attrib->vdata, min, max);

    if (auto_apply)
	redraw_the_form(0);
}

void
scb_delta_change(FL_OBJECT * ob, long data)
{
    double ldelta = get_finput_value(scb_attrib->ldelta, -1);
    double rdelta = get_finput_value(scb_attrib->rdelta, -1);

    fl_set_scrollbar_increment(scb_attrib->vdata, ldelta, rdelta);

    if (auto_apply)
	redraw_the_form(0);
}

void
scb_slsize_change(FL_OBJECT * ob, long data)
{
    double slsize = get_finput_value(scb_attrib->slsize, 2);

    fl_set_scrollbar_size(scb_attrib->vdata, slsize);

    if (auto_apply)
	redraw_the_form(0);
}

void
scb_step_change(FL_OBJECT * ob, long data)
{
    double step = get_finput_value(scb_attrib->step, 4);

    fl_set_scrollbar_step(scb_attrib->vdata, step);

    if (auto_apply)
	redraw_the_form(0);
}

void
scb_initialvalue_change(FL_OBJECT * ob, long data)
{
    double val = get_finput_value(scb_attrib->initial_val, 3);

    fl_set_scrollbar_value(scb_attrib->vdata, val);
    set_finput_value(scb_attrib->initial_val, scb_spec->val, -1);

    if (auto_apply)
	redraw_the_form(0);
}

void
scb_returnsetting_change(FL_OBJECT * ob, long data)
{
    const char *s = fl_get_choice_text(scb_attrib->returnsetting);
    fl_set_scrollbar_return(scb_attrib->vdata, get_how_return_str_value(s));
}


#include "scrollbar_spec.c"
