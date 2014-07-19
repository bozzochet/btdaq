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
 * $Id: sp_positioner.c,v 1.0 2002/05/08 05:27:39 zhao Release $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 * Settting counter class specific attributes.
 *
 */

#include "forms.h"
#include "fd_main.h"
#include "fd_spec.h"
#include "ppositioner.h"
#include "spec/positioner_spec.h"

extern FD_posattrib *create_form_posattrib(void);

static FD_posattrib *pos_attrib;
static SuperSPEC *pos_spec;
static void init_spec(SuperSPEC *);

void *
get_pos_spec_fdform(void)
{
    if (!pos_attrib)
    {
	pos_attrib = create_form_posattrib();
	fl_addto_choice(pos_attrib->returnsetting, get_how_return_str());
    }
    return pos_attrib;
}


void
pos_spec_restore(FL_OBJECT * ob, long data)
{
    superspec_to_spec(pos_attrib->vdata);
    init_spec(get_superspec(pos_attrib->vdata));
    redraw_the_form(0);
}


static void
init_spec(SuperSPEC * spec)
{
    set_finput_value(pos_attrib->xminval, spec->xmin, -1);
    set_finput_value(pos_attrib->xmaxval, spec->xmax, -1);
    set_finput_value(pos_attrib->yminval, spec->ymin, -1);
    set_finput_value(pos_attrib->ymaxval, spec->ymax, -1);

    set_finput_value(pos_attrib->initialxval, spec->xval, -1);
    set_finput_value(pos_attrib->initialyval, spec->yval, -1);

    set_finput_value(pos_attrib->xstep, spec->xstep, -1);
    set_finput_value(pos_attrib->ystep, spec->ystep, -1);

    fl_set_choice_text(pos_attrib->returnsetting,
		       get_how_return_str_name(spec->how_return));
}

int
set_pos_attrib(FL_OBJECT * ob)
{
    pos_attrib->vdata = ob;
    pos_spec = get_superspec(ob);
    init_spec(pos_spec);
    return 0;
}

void
emit_pos_code(FILE * fp, FL_OBJECT * ob)
{
    FL_OBJECT *defobj;
    SuperSPEC *spec, *defspec;

    if (ob->objclass != FL_POSITIONER)
	return;

    /* create a default object */
    defobj = fl_create_positioner(ob->type, 0, 0, 0, 0, "");

    defspec = get_superspec(defobj);
    spec = get_superspec(ob);

    if (spec->xmin != defspec->xmin || spec->xmax != defspec->xmax)
    {
	fprintf(fp, "    fl_set_positioner_xbounds(obj, %g, %g);\n",
		spec->xmin, spec->xmax);
    }

    if (spec->ymin != defspec->ymin || spec->ymax != defspec->ymax)
    {
	fprintf(fp, "    fl_set_positioner_ybounds(obj, %g, %g);\n",
		spec->ymin, spec->ymax);
    }

    if (spec->xval != defspec->xval)
	fprintf(fp, "    fl_set_positioner_xvalue(obj, %g);\n", spec->xval);
    if (spec->yval != defspec->yval)
	fprintf(fp, "    fl_set_positioner_yvalue(obj, %g);\n", spec->yval);

    if (spec->xstep != defspec->xstep)
	fprintf(fp, "    fl_set_positioner_xstep(obj, %g);\n", spec->xstep);
    if (spec->ystep != defspec->ystep)
	fprintf(fp, "    fl_set_positioner_ystep(obj, %g);\n", spec->ystep);

    if (spec->how_return != defspec->how_return)
	fprintf(fp, "    fl_set_positioner_return(obj, %s);\n",
		get_how_return_name(spec->how_return));


    fl_free_object(defobj);
}

void
save_pos_attrib(FILE * fp, FL_OBJECT * ob)
{
    FL_OBJECT *defobj;
    SuperSPEC *defspec, *spec;


    if (ob->objclass != FL_POSITIONER)
	return;

    /* create a default object */
    defobj = fl_create_positioner(ob->type, 0, 0, 0, 0, "");

    defspec = get_superspec(defobj);
    spec = get_superspec(ob);

    if (spec->xmin != defspec->xmin || spec->xmax != defspec->xmax)
	fprintf(fp, "  xbounds: %g %g\n", spec->xmin, spec->xmax);
    if (spec->ymin != defspec->ymin || spec->ymax != defspec->ymax)
	fprintf(fp, "  ybounds: %g %g\n", spec->ymin, spec->ymax);

    if (spec->xval != defspec->xval)
	fprintf(fp, "  xvalue: %g\n", spec->xval);
    if (spec->yval != defspec->yval)
	fprintf(fp, "  yvalue: %g\n", spec->yval);

    if (spec->xstep != defspec->xstep)
	fprintf(fp, "  xstep: %g\n", spec->xstep);
    if (spec->ystep != defspec->ystep)
	fprintf(fp, "  ystep: %g\n", spec->ystep);

    if (spec->how_return != defspec->how_return)
	fprintf(fp, "  return: %s\n", get_how_return_name(spec->how_return));

    fl_free_object(defobj);
}


void
pos_xminmax_change(FL_OBJECT * ob, long data)
{
    double min = get_finput_value(pos_attrib->xminval, -1);
    double max = get_finput_value(pos_attrib->xmaxval, -1);

    fl_set_positioner_xbounds(pos_attrib->vdata, min, max);
    if (auto_apply)
	redraw_the_form(0);
}

void
pos_yminmax_change(FL_OBJECT * ob, long data)
{
    double min = get_finput_value(pos_attrib->yminval, -1);
    double max = get_finput_value(pos_attrib->ymaxval, -1);

    fl_set_positioner_ybounds(pos_attrib->vdata, min, max);
    if (auto_apply)
	redraw_the_form(0);
}

void
pos_xstepchange_cb(FL_OBJECT * ob, long data)
{
    float s = get_finput_value(pos_attrib->xstep, -1);
    fl_set_positioner_xstep(pos_attrib->vdata, s);
    if (auto_apply)
	redraw_the_form(0);
}

void
pos_ystepchange_cb(FL_OBJECT * ob, long data)
{
    float s = get_finput_value(pos_attrib->ystep, -1);
    fl_set_positioner_ystep(pos_attrib->vdata, s);
    if (auto_apply)
	redraw_the_form(0);
}

void
pos_initialxvalue_change(FL_OBJECT * ob, long data)
{
    double val = get_finput_value(pos_attrib->initialxval, -1);
    fl_set_positioner_xvalue(pos_attrib->vdata, val);
    if (auto_apply)
	redraw_the_form(0);
}

void
pos_initialyvalue_change(FL_OBJECT * ob, long data)
{
    double val = get_finput_value(pos_attrib->initialyval, -1);
    fl_set_positioner_yvalue(pos_attrib->vdata, val);
    if (auto_apply)
	redraw_the_form(0);
}

void
pos_returnsetting_change(FL_OBJECT * ob, long data)
{
    const char *s = fl_get_choice_text(pos_attrib->returnsetting);
    fl_set_positioner_return(pos_attrib->vdata, get_how_return_str_value(s));
}

#include "spec/positioner_spec.c"
