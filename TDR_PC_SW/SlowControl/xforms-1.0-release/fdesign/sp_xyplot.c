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
 * $Id: sp_xyplot.c,v 1.0 2002/05/08 05:27:39 zhao Release $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 * Settting xyplot class specific attributes.
 *
 */

#include "forms.h"
#include "fd_main.h"
#include "fd_spec.h"
#include "pxyplot.h"
#include "spec/xyplot_spec.h"

extern FD_xyplotattrib *create_form_xyplotattrib(void);

static FD_xyplotattrib *xyplot_attrib;
static SuperSPEC *xyplot_spec;
static void show_spec(SuperSPEC *);

void *
get_xyplot_spec_fdform(void)
{
    if (!xyplot_attrib)
    {
	xyplot_attrib = create_form_xyplotattrib();
	fl_addto_choice(xyplot_attrib->xscale, get_scale_string());
	fl_addto_choice(xyplot_attrib->yscale, get_scale_string());
	fl_addto_choice(xyplot_attrib->xgrid, get_grid_string());
	fl_addto_choice(xyplot_attrib->ygrid, get_grid_string());
	fl_addto_choice(xyplot_attrib->gridstyle, get_linestyle_string());
	fl_addto_choice(xyplot_attrib->how_return, get_how_return_str());
    }
    return xyplot_attrib;
}

void
xyplot_spec_restore(FL_OBJECT * ob, long data)
{
    superspec_to_spec(xyplot_attrib->vdata);
    show_spec(get_superspec(xyplot_attrib->vdata));
    redraw_the_form(0);
}

static void
show_spec(SuperSPEC * spec)
{
    fl_set_counter_value(xyplot_attrib->xmajor, spec->xmajor);
    fl_set_counter_value(xyplot_attrib->xminor, spec->xminor);
    fl_set_counter_value(xyplot_attrib->ymajor, spec->ymajor);
    fl_set_counter_value(xyplot_attrib->yminor, spec->yminor);

    fl_set_choice(xyplot_attrib->xgrid, spec->xgrid + 1);
    fl_set_choice(xyplot_attrib->ygrid, spec->ygrid + 1);
    fl_set_choice(xyplot_attrib->xscale, spec->xscale + 1);
    fl_set_choice(xyplot_attrib->yscale, spec->yscale + 1);

    fl_set_choice(xyplot_attrib->gridstyle, spec->grid_linestyle + 1);

    set_finput_value(xyplot_attrib->xbase, spec->xbase, -1);
    set_finput_value(xyplot_attrib->ybase, spec->ybase, -1);

    fl_set_choice_text(xyplot_attrib->how_return,
		       get_how_return_str_name(spec->how_return));

    fl_set_button(xyplot_attrib->mark_active, spec->mark_active);
}

int
set_xyplot_attrib(FL_OBJECT * ob)
{
    xyplot_attrib->vdata = ob;
    xyplot_spec = get_superspec(ob);

    if (ob->type != FL_ACTIVE_XYPLOT)
    {
	fl_hide_object(xyplot_attrib->how_return);
	fl_hide_object(xyplot_attrib->mark_active);
    }
    else
    {
	fl_show_object(xyplot_attrib->how_return);
	fl_show_object(xyplot_attrib->mark_active);
    }

    if (xyplot_spec->xscale != FL_LOG)
	fl_hide_object(xyplot_attrib->xbase);
    else
	fl_show_object(xyplot_attrib->xbase);

    if (xyplot_spec->yscale != FL_LOG)
	fl_hide_object(xyplot_attrib->ybase);
    else
	fl_show_object(xyplot_attrib->ybase);

    if (xyplot_spec->xgrid || xyplot_spec->ygrid)
	fl_show_object(xyplot_attrib->gridstyle);
    else
	fl_hide_object(xyplot_attrib->gridstyle);

    show_spec(xyplot_spec);

    return 0;
}


void
emit_xyplot_code(FILE * fp, FL_OBJECT * ob)
{
    FL_OBJECT *defobj;
    SuperSPEC *sp, *defsp;

    if (ob->objclass != FL_XYPLOT)
    {
	M_err("EmitXYPlotCode", "not xyplot class!");
	return;
    }

    /* create a default object */
    defobj = fl_create_xyplot(ob->type, 0, 0, 0, 0, "");

    defsp = get_superspec(defobj);
    sp = get_superspec(ob);

    if (sp->xmajor != defsp->xmajor || sp->xminor != defsp->xminor)
	fprintf(fp, "    fl_set_xyplot_xtics(obj, %d, %d);\n",
		sp->xmajor, sp->xminor);

    if (sp->ymajor != defsp->ymajor || sp->yminor != defsp->yminor)
	fprintf(fp, "    fl_set_xyplot_ytics(obj, %d, %d);\n",
		sp->ymajor, sp->yminor);

    if (sp->xscale != defsp->xscale ||
	(sp->xscale == FL_LOG && (sp->xbase != defsp->xbase)))
	fprintf(fp, "    fl_set_xyplot_xscale(obj, %s, %g);\n",
		get_scale_name(sp->xscale), sp->xbase);

    if (sp->yscale != defsp->yscale ||
	(sp->yscale == FL_LOG && (sp->ybase != defsp->ybase)))
	fprintf(fp, "    fl_set_xyplot_yscale(obj, %s, %g);\n",
		get_scale_name(sp->yscale), sp->ybase);

    if (sp->xgrid != defsp->xgrid)
	fprintf(fp, "    fl_set_xyplot_xgrid(obj, %s);\n",
		get_grid_name(sp->xgrid));

    if (sp->ygrid != defsp->ygrid)
	fprintf(fp, "    fl_set_xyplot_ygrid(obj, %s);\n",
		get_grid_name(sp->ygrid));

    if (sp->grid_linestyle != defsp->grid_linestyle)
	fprintf(fp, "    fl_set_xyplot_grid_linestyle(obj, %s);\n",
		get_linestyle_name(sp->grid_linestyle));

    if (ob->type == FL_ACTIVE_XYPLOT)
    {
	if (sp->mark_active != defsp->mark_active)
	    fprintf(fp, "    fl_set_xyplot_mark_active(obj, %d);\n",
		    sp->mark_active);
	if (sp->how_return != defsp->how_return)
	    fprintf(fp, "    fl_set_xyplot_return(obj, %s);\n",
		    get_how_return_name(sp->how_return));
    }


    fl_free_object(defobj);
}

void
save_xyplot_attrib(FILE * fp, FL_OBJECT * ob)
{
    FL_OBJECT *defobj;
    SuperSPEC *defsp, *sp;

    if (ob->objclass != FL_XYPLOT)
	return;

    /* create a default object */
    defobj = fl_create_xyplot(ob->type, 0, 0, 0, 0, "");

    defsp = get_superspec(defobj);
    sp = get_superspec(ob);

    if (sp->xmajor != defsp->xmajor || sp->xminor != defsp->xminor)
	fprintf(fp, "  xtics: %d %d\n", sp->xmajor, sp->xminor);

    if (sp->ymajor != defsp->ymajor || sp->yminor != defsp->yminor)
	fprintf(fp, "  ytics: %d %d\n", sp->ymajor, sp->yminor);

    if (sp->xgrid != defsp->xgrid || sp->ygrid != defsp->ygrid)
	fprintf(fp, "   grid: %s %s\n",
		get_grid_name(sp->xgrid), get_grid_name(sp->ygrid));

    if (sp->grid_linestyle != defsp->grid_linestyle)
	fprintf(fp, "   gridstyle: %s\n",
		get_linestyle_name(sp->grid_linestyle));

    if (sp->xscale != defsp->xscale || sp->xbase != defsp->xbase)
	fprintf(fp, "  xscale: %s %g\n", get_scale_name(sp->xscale), sp->xbase);
    if (sp->yscale != defsp->yscale || sp->ybase != defsp->ybase)
	fprintf(fp, "  yscale: %s %g\n", get_scale_name(sp->yscale), sp->ybase);

    if (ob->type == FL_ACTIVE_XYPLOT)
    {
	if (sp->how_return != defsp->how_return)
	    fprintf(fp, "  return: %s\n",
		    get_how_return_str_name(sp->how_return));
	if (sp->mark_active != defsp->mark_active)
	    fprintf(fp, "  markactive: %d\n", sp->mark_active);
    }

    fl_free_object(defobj);
}

/************ callback stuff  ************/

void
grid_change_cb(FL_OBJECT * ob, long data)
{
    int xg = fl_get_choice(xyplot_attrib->xgrid) - 1;
    int yg = fl_get_choice(xyplot_attrib->ygrid) - 1;

    if (xg || yg)
	fl_show_object(xyplot_attrib->gridstyle);
    else
	fl_hide_object(xyplot_attrib->gridstyle);

    fl_set_xyplot_xgrid(xyplot_attrib->vdata, xg);
    fl_set_xyplot_ygrid(xyplot_attrib->vdata, yg);

    if (auto_apply)
	redraw_the_form(0);
}

void
markactive_change_cb(FL_OBJECT * ob, long data)
{
    fl_set_xyplot_mark_active(xyplot_attrib->vdata,
			      fl_get_button(xyplot_attrib->mark_active));
    if (auto_apply)
	redraw_the_form(0);
}

void
gridstyle_change_cb(FL_OBJECT * ob, long data)
{
    int style = fl_get_choice(xyplot_attrib->gridstyle) - 1;

    fl_set_xyplot_grid_linestyle(xyplot_attrib->vdata, style);

    if (auto_apply)
	redraw_the_form(0);
}

void
xscale_change_cb(FL_OBJECT * ob, long data)
{
    int s = fl_get_choice(xyplot_attrib->xscale) - 1;
    float base = get_finput_value(xyplot_attrib->xbase, -1);

    if (s == FL_LOG)
	fl_show_object(xyplot_attrib->xbase);
    else
	fl_hide_object(xyplot_attrib->xbase);

    fl_set_xyplot_xscale(xyplot_attrib->vdata, s, base);
    if (auto_apply)
	redraw_the_form(0);

}

void
yscale_change_cb(FL_OBJECT * ob, long data)
{
    /* fill-in code for callback */
    int s = fl_get_choice(xyplot_attrib->yscale) - 1;
    float base = get_finput_value(xyplot_attrib->ybase, -1);

    if (s == FL_LOG)
	fl_show_object(xyplot_attrib->ybase);
    else
	fl_hide_object(xyplot_attrib->ybase);

    fl_set_xyplot_yscale(xyplot_attrib->vdata, s, base);

    if (auto_apply)
	redraw_the_form(0);
}

void
ymajorminor_change_cb(FL_OBJECT * ob, long data)
{
    /* fill-in code for callback */
    int ymajor = fl_get_counter_value(xyplot_attrib->ymajor);
    int yminor = fl_get_counter_value(xyplot_attrib->yminor);

    fl_set_xyplot_ytics(xyplot_attrib->vdata, ymajor, yminor);
    if (auto_apply)
	redraw_the_form(0);
}


void
xmajorminor_change_cb(FL_OBJECT * ob, long data)
{
    int xmajor = fl_get_counter_value(xyplot_attrib->xmajor);
    int xminor = fl_get_counter_value(xyplot_attrib->xminor);

    fl_set_xyplot_xtics(xyplot_attrib->vdata, xmajor, xminor);
    if (auto_apply)
	redraw_the_form(0);
}

void
xyplot_returnsetting_change(FL_OBJECT * ob, long data)
{
    const char *s = fl_get_choice_text(ob);
    fl_set_xyplot_return(xyplot_attrib->how_return,
			 get_how_return_str_value(s));
}


#include "spec/xyplot_spec.c"
