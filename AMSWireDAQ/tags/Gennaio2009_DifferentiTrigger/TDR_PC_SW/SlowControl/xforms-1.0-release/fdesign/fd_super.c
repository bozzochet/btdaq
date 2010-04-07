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
 * $Id: fd_super.c,v 1.0 2002/05/08 05:27:39 zhao Release $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 * translation between superspec and spec
 */
#include "forms.h"
#include "fd_main.h"
#include "fd_spec.h"

#include "pslider.h"
#include "pbrowser.h"
#include "ppositioner.h"
#include "pcounter.h"
#include "pscrollbar.h"
#include "pdial.h"
#include "pxyplot.h"
#include "pchoice.h"
#include "pmenu.h"

SuperSPEC *
get_superspec(FL_OBJECT * ob)
{
    SuperSPEC *sp = ob->u_vdata;

    return sp ? sp : spec_to_superspec(ob);
}

SuperSPEC *
spec_to_superspec(FL_OBJECT * ob)
{
    SuperSPEC *spp;
    int i;

    if (!ob->u_vdata)
    {
	ob->u_vdata = spp = fl_calloc(1, sizeof(*spp));
	spp->content = fl_calloc(MAX_CONTENT + 1, sizeof(*spp->content));
	spp->shortcut = fl_calloc(MAX_CONTENT + 1, sizeof(*spp->shortcut));
	spp->mode = fl_calloc(MAX_CONTENT + 1, sizeof(*spp->mode));
	spp->new_menuapi = 1;
    }

    spp = ob->u_vdata;

    if (ob->objclass == FL_BROWSER)
    {
	FL_BROWSER_SPEC *sp = ob->spec;
	int n;

	spp->h_pref = sp->h_pref;
	spp->v_pref = sp->v_pref;

	n = fl_get_browser_maxline(ob);
	spp->nlines = n;
	for (i = 1; i <= n && i <= MAX_CONTENT; i++)
	{
	    if (spp->content[i])
		fl_free(spp->content[i]);
	    spp->content[i] = fl_strdup(fl_get_browser_line(ob, i));
	}

    }
    else if (ob->objclass == FL_CHOICE)
    {
	FL_CHOICE_SPEC *sp = ob->spec;

	spp->nlines = sp->numitems;
	spp->align = sp->align;
	spp->int_val = sp->val;

	for (i = 1; i <= spp->nlines; i++)
	{
	    spp->mode[i] = sp->mode[i];

	    if (spp->shortcut[i])
		fl_free(spp->shortcut[i]);

	    if (sp->shortcut[i])
		spp->shortcut[i] = fl_strdup(sp->shortcut[i]);

	    if (spp->content[i])
		fl_free(spp->content[i]);

	    spp->content[i] = fl_strdup(fl_get_choice_item_text(ob, i));
	}
    }
    else if (ob->objclass == FL_MENU)
    {
	FL_MENU_SPEC *sp = ob->spec;

	spp->nlines = sp->numitems;

	for (i = 1; i <= spp->nlines; i++)
	{
	    spp->mode[i] = sp->mode[i];

	    if (spp->shortcut[i])
		fl_free(spp->shortcut[i]);

	    if (sp->shortcut[i])
		spp->shortcut[i] = fl_strdup(sp->shortcut[i]);

	    if (spp->content[i])
		fl_free(spp->content[i]);

	    spp->content[i] = fl_strdup(fl_get_menu_item_text(ob, i));
	}
    }
    else if (ob->objclass == FL_SLIDER ||
	     ob->objclass == FL_VALSLIDER ||
	     ob->objclass == FL_THUMBWHEEL)
    {
	FL_SLIDER_SPEC *sp = ob->spec;

	spp->val = sp->val;
	spp->min = sp->min;
	spp->max = sp->max;
	spp->step = sp->step;
	spp->prec = sp->prec;
	spp->ldelta = sp->ldelta;
	spp->rdelta = sp->rdelta;
	spp->slsize = sp->slsize;
	spp->how_return = sp->how_return;
    }
    else if (ISBUTTON(ob->objclass) ||
	     ob->objclass == FL_PIXMAP || ob->objclass == FL_BITMAP)
    {
	FL_BUTTON_SPEC *sp = ob->spec;
	IconInfo *info;

	spp->int_val = sp->val;
	if (!spp->cspecv)
	{
	    info = spp->cspecv = fl_calloc(1, sizeof(IconInfo));
	    info->show_focus = 1;
	    info->dx = info->dy = 3;
	    info->align = FL_ALIGN_CENTER;
	    info->fullpath = 1;
	}

	info = spp->cspecv;

	if (ob->objclass == FL_PIXMAPBUTTON ||
	    ob->objclass == FL_BITMAPBUTTON ||
	    ob->objclass == FL_PIXMAP ||
	    ob->objclass == FL_BITMAP)
	{
	    spp->align = info->align;
	    spp->dx = info->dx;
	    spp->dy = info->dy;
	    spp->show_focus = info->show_focus;
	    spp->use_data = info->use_data;
	    spp->fullpath = info->fullpath;
	    strcpy(spp->filename, info->filename);
	    strcpy(spp->data, info->data);
	    strcpy(spp->focus_filename, info->focus_filename);
	    strcpy(spp->helper, info->helper);
	    strcpy(spp->focus_data, info->focus_data);
	    strcpy(spp->width, info->width);
	    strcpy(spp->height, info->height);
	}

    }
    else if (ob->objclass == FL_POSITIONER)
    {
	FL_POSITIONER_SPEC *sp = ob->spec;
	spp->xstep = sp->xstep;
	spp->ystep = sp->ystep;
	spp->xmin = sp->xmin;
	spp->xmax = sp->xmax;
	spp->xval = sp->xval;
	spp->ymin = sp->ymin;
	spp->ymax = sp->ymax;
	spp->yval = sp->yval;
	spp->how_return = sp->how_return;
    }
    else if (ob->objclass == FL_COUNTER)
    {
	FL_COUNTER_SPEC *sp = ob->spec;

	spp->val = sp->val;
	spp->lstep = sp->lstep;
	spp->sstep = sp->sstep;
	spp->min = sp->min;
	spp->max = sp->max;
	spp->prec = sp->prec;
	spp->how_return = sp->how_return;
    }
    else if (ob->objclass == FL_DIAL)
    {
	FL_DIAL_SPEC *sp = ob->spec;

	spp->min = sp->min;
	spp->max = sp->max;
	spp->val = sp->val;
	spp->step = sp->step;
	spp->thetai = sp->thetai;
	spp->thetaf = sp->thetaf;
	spp->direction = sp->direction;
	spp->how_return = sp->how_return;
    }
    else if (ob->objclass == FL_XYPLOT)
    {
	FL_XYPLOT_SPEC *sp = ob->spec;

	spp->xmajor = sp->xmajor;
	spp->xminor = sp->xminor;
	spp->ymajor = sp->ymajor;
	spp->yminor = sp->yminor;
	spp->xscale = sp->xscale;
	spp->yscale = sp->yscale;
	spp->xgrid = sp->xgrid;
	spp->ygrid = sp->ygrid;
	spp->grid_linestyle = sp->grid_linestyle;
	spp->xbase = sp->xbase;
	spp->ybase = sp->ybase;
	spp->mark_active = sp->mark_active;
    }
    else if (ob->objclass == FL_SCROLLBAR)
    {
	FL_SCROLLBAR_SPEC *scbsp = ob->spec;
	FL_SLIDER_SPEC *sp = scbsp->slider->spec;

	spp->val = sp->val;
	spp->min = sp->min;
	spp->max = sp->max;
	spp->prec = sp->prec;
	spp->step = sp->step;
	spp->slsize = sp->slsize;
	spp->ldelta = sp->ldelta;
	spp->rdelta = sp->rdelta;
	spp->how_return = sp->how_return;
    }
    return spp;
}

void *
superspec_to_spec(FL_OBJECT * ob)
{
    void *v = ob->spec;
    SuperSPEC *spp = ob->u_vdata;
    int i = 0;

    if (!spp)
	return v;

    if (ob->objclass == FL_BROWSER)
    {
	FL_BROWSER_SPEC *sp = ob->spec;

	sp->h_pref = spp->h_pref;
	sp->v_pref = spp->v_pref;

	fl_clear_browser(ob);
	if (spp->content[1])
	{
	    for (i = 1; i <= spp->nlines; i++)
		fl_addto_browser(ob, spp->content[i]);
	}
    }
    else if (ob->objclass == FL_CHOICE)
    {
	FL_CHOICE_SPEC *sp = ob->spec;

	sp->numitems = spp->nlines;
	sp->align = spp->align;

	fl_clear_choice(ob);
	if (spp->content[1])
	{
	    for (i = 1; i <= spp->nlines; i++)
	    {
		fl_addto_choice(ob, spp->content[i]);
		fl_set_choice_item_mode(ob, i, spp->mode[i]);
		if (spp->shortcut[i])
		    fl_set_choice_item_shortcut(ob, i, spp->shortcut[i]);
	    }

	    fl_set_choice(ob, spp->int_val);
	}
    }
    else if (ob->objclass == FL_MENU)
    {
	FL_MENU_SPEC *sp = ob->spec;

	sp->numitems = spp->nlines;

	if (spp->content[1])
	{
	    fl_clear_menu(ob);
	    for (i = 1; i <= spp->nlines; i++)
	    {
		fl_addto_menu(ob, spp->content[i]);
		fl_set_menu_item_mode(ob, i, spp->mode[i]);
		if (spp->shortcut[i])
		    fl_set_menu_item_shortcut(ob, i, spp->shortcut[i]);
	    }
	}
    }
    else if (ob->objclass == FL_SLIDER ||
	     ob->objclass == FL_VALSLIDER ||
	     ob->objclass == FL_THUMBWHEEL)
    {
	FL_SLIDER_SPEC *sp = ob->spec;

	sp->val = spp->val;
	sp->min = spp->min;
	sp->max = spp->max;
	sp->step = spp->step;
	sp->prec = spp->prec;
	sp->ldelta = spp->ldelta;
	sp->rdelta = spp->rdelta;
	sp->slsize = spp->slsize;
	sp->how_return = spp->how_return;
    }
    else if (ISBUTTON(ob->objclass) ||
	     ob->objclass == FL_PIXMAP || ob->objclass == FL_BITMAP)
    {
	FL_BUTTON_SPEC *sp = ob->spec;
	IconInfo *info = spp->cspecv;

	sp->val = spp->int_val;
	if (ISBUTTON(ob->objclass))
	    fl_set_button(ob, sp->val);

	if (ob->objclass == FL_PIXMAPBUTTON ||
	    ob->objclass == FL_BITMAPBUTTON ||
	    ob->objclass == FL_PIXMAP ||
	    ob->objclass == FL_BITMAP)
	{
	    info->align = spp->align;
	    info->dx = spp->dx;
	    info->dy = spp->dy;
	    info->show_focus = spp->show_focus;
	    info->use_data = spp->use_data;
	    info->fullpath = spp->fullpath;
	    strcpy(info->filename, spp->filename);
	    strcpy(info->data, spp->data);
	    strcpy(info->focus_filename, spp->focus_filename);
	    strcpy(info->helper, spp->helper);
	    strcpy(info->focus_data, spp->focus_data);
	    strcpy(info->width, spp->width);
	    strcpy(info->height, spp->height);

	    if (ob->objclass == FL_PIXMAPBUTTON || ob->objclass == FL_PIXMAP)
	    {
		fl_set_pixmap_align(ob, info->align | FL_ALIGN_INSIDE,
				    info->dx, info->dy);
		fl_set_pixmapbutton_focus_outline(ob, info->show_focus);
	    }

	    if (info->filename[0])
	    {
		if (ob->objclass == FL_PIXMAPBUTTON || ob->objclass == FL_PIXMAP)
		{
		    fl_set_pixmapbutton_file(ob, info->filename);
		    if (info->focus_filename[0])
			fl_set_pixmapbutton_focus_file(ob, info->focus_filename);
		}
		else
		    fl_set_bitmapbutton_file(ob, info->filename);
	    }
	}
	if (info->helper[0])
	    fl_set_object_helper(ob, get_helper(info->helper));
    }
    else if (ob->objclass == FL_POSITIONER)
    {
	FL_POSITIONER_SPEC *sp = ob->spec;
	sp->xstep = spp->xstep;
	sp->ystep = spp->ystep;
	sp->xmin = spp->xmin;
	sp->xmax = spp->xmax;
	sp->xval = spp->xval;
	sp->ymin = spp->ymin;
	sp->ymax = spp->ymax;
	sp->yval = spp->yval;
	sp->how_return = spp->how_return;
    }
    else if (ob->objclass == FL_COUNTER)
    {
	FL_COUNTER_SPEC *sp = ob->spec;

	sp->val = spp->val;
	sp->sstep = spp->sstep;
	sp->lstep = spp->lstep;
	sp->min = spp->min;
	sp->max = spp->max;
	sp->prec = spp->prec;
	sp->how_return = spp->how_return;
    }
    else if (ob->objclass == FL_DIAL)
    {
	FL_DIAL_SPEC *sp = ob->spec;

	sp->min = spp->min;
	sp->max = spp->max;
	sp->val = spp->val;
	sp->step = spp->step;
	sp->thetai = spp->thetai;
	sp->thetaf = spp->thetaf;
	sp->direction = spp->direction;
	sp->how_return = spp->how_return;
    }
    else if (ob->objclass == FL_XYPLOT)
    {
	FL_XYPLOT_SPEC *sp = ob->spec;

	sp->xmajor = spp->xmajor;
	sp->xminor = spp->xminor;
	sp->ymajor = spp->ymajor;
	sp->yminor = spp->yminor;
	sp->xscale = spp->xscale;
	sp->yscale = spp->yscale;
	sp->xgrid = spp->xgrid;
	sp->ygrid = spp->ygrid;
	sp->xbase = spp->xbase;
	sp->ybase = spp->ybase;
	sp->grid_linestyle = spp->grid_linestyle;
	sp->mark_active = spp->mark_active;
    }
    else if (ob->objclass == FL_SCROLLBAR)
    {
	FL_SCROLLBAR_SPEC *scbsp = ob->spec;
	FL_SLIDER_SPEC *sp = scbsp->slider->spec;

	sp->val = spp->val;
	sp->min = spp->min;
	sp->max = spp->max;
	sp->prec = spp->prec;
	sp->step = spp->step;
	sp->slsize = spp->slsize;
	sp->ldelta = spp->ldelta;
	sp->rdelta = spp->rdelta;
	sp->how_return = spp->how_return;
    }
    return v;
}

static int keep_content = 1;

void
copy_superspec(FL_OBJECT * target, FL_OBJECT * src)
{
    SuperSPEC *t, *s;
    void *tmp;
    int i;

    t = get_superspec(target);
    s = get_superspec(src);

    if (!t || !s)
    {
	M_err("CopySuperSPEC", "null spec");
	return;
    }

    tmp = t->cspecv;
    *t = *s;
    t->cspecv = tmp;

    if (!keep_content)
    {
	for (i = 1; i <= t->nlines; i++)
	{
	    t->content[i] = 0;
	    t->mode[i] = 0;
	    t->shortcut[i] = 0;
	}
	t->nlines = 0;
    }
    else
    {
	for (i = 1; i <= t->nlines; i++)
	{
	    t->mode[i] = s->mode[i];
	    t->content[i] = fl_strdup(s->content[i]);
	    if (s->shortcut[i])
		t->shortcut[i] = fl_strdup(s->shortcut[i]);
	}

	if (t->cspecv && s->cspecv && t->cspecv_size)
	    memcpy(t->cspecv, s->cspecv, t->cspecv_size);
    }

    superspec_to_spec(target);
}
