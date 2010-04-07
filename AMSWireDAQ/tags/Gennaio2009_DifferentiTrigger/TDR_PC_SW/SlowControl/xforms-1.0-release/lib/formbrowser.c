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
 * $Id: formbrowser.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *.
 *  This file is part of the XForms library package.
 *
 *  Copyright (c) 1997  By T.C. Zhao and Mark Overmars
 *  Copyright (c) 1998  By Steve Lamont of the National Center for
 *                      Microscopy and Imaging Research
 *  Copyright (c) 1999-2002  by T.C. Zhao and Steve Lamont
 *  All rights reserved.
 *.
 *
 * form browser.
 *
 */

#include "forms.h"
#include "pformbrowser.h"

void check_scrollbar(FL_OBJECT * ob);
#define SPEC     FL_FORMBROWSER_SPEC

static int canvas_cleanup(FL_OBJECT * ob);
static int canvas_handler(FL_OBJECT * ob, Window win, int w, int h,
			  XEvent * ev, void *data);
static void delete_form(SPEC *sp, int f);
static void display_forms(SPEC *sp);
static void form_callback(FL_OBJECT * ob, void *data);
static int handle(FL_OBJECT * ob, int event, FL_Coord mx, FL_Coord my,
		  int key, void *ev);
static void hcb(FL_OBJECT * ob, long data);
static void parentize_form(FL_FORM * form, FL_OBJECT * ob);
static void set_form_position(FL_FORM * form, int x, int y);
static void vcb(FL_OBJECT * ob, long data);


FL_OBJECT *
fl_create_formbrowser(int type, FL_Coord x, FL_Coord y,
		      FL_Coord w, FL_Coord h, const char *label)
{
    FL_OBJECT *ob;
    SPEC *sp;
    int absbw, oldu = fl_get_coordunit();
    int D;

    ob = fl_make_object(FL_FORMBROWSER, type, x, y, w, h, label, handle);
    fl_set_coordunit(FL_COORD_PIXEL);
    ob->boxtype = FL_FORMBROWSER_BOXTYPE;
    ob->align = FL_FORMBROWSER_ALIGN;
    ob->col1 = FL_FORMBROWSER_COL1;
    ob->col2 = FL_BLACK;

    absbw = FL_abs(ob->bw);

    ob->spec_size = sizeof(SPEC);
    sp = ob->spec = fl_calloc(1, ob->spec_size);
    sp->form = (FL_FORM **) fl_malloc(1);

    sp->parent = ob;

    sp->scroll = FL_SMOOTH_SCROLL;

    sp->vw_def = sp->hh_def = D = fl_get_default_scrollbarsize(ob);
    sp->canvas = fl_create_canvas(FL_SCROLLED_CANVAS,
				  ob->x + absbw, ob->y + absbw,
				  ob->w - 2 * absbw - sp->vw_def,
				  ob->h - 2 * absbw - sp->hh_def,
				  label ? label : "formbrowser");

    sp->canvas->u_vdata = sp;

    fl_modify_canvas_prop(sp->canvas, NULL, NULL, canvas_cleanup);

    fl_set_object_color(sp->canvas, ob->col1, ob->col2);
    fl_set_object_bw(sp->canvas, ob->bw);

    fl_set_canvas_decoration(sp->canvas, fl_boxtype2frametype(ob->boxtype));
    fl_add_canvas_handler(sp->canvas, Expose, canvas_handler, NULL);

    sp->v_pref = sp->h_pref = FL_AUTO;

    sp->hsl = fl_create_scrollbar(FL_HOR_THIN_SCROLLBAR, ob->x,
				  y + h - D, w - D, D, "");
    fl_set_scrollbar_value(sp->hsl, 0.0);
/*   fl_set_object_boxtype(sp->hsl, ob->boxtype); */
    sp->hsl->visible = sp->h_pref == FL_ON;
    sp->hsl->resize = FL_RESIZE_X;
    fl_set_object_callback(sp->hsl, hcb, 0);

    sp->vsl = fl_create_scrollbar(FL_VERT_THIN_SCROLLBAR,
				  x + w - D, y, D, h - D, "");
    fl_set_object_boxtype(sp->vsl, ob->boxtype);
    sp->vsl->visible = sp->v_pref == FL_ON;
    fl_set_scrollbar_value(sp->vsl, 0.0);
    sp->vsl->resize = FL_RESIZE_Y;
    fl_set_object_callback(sp->vsl, vcb, 0);

    fl_set_coordunit(oldu);
    return ob;
}

FL_OBJECT *
fl_add_formbrowser(int type, int x, int y, int w, int h, const char *label)
{

    FL_OBJECT *ob = fl_create_formbrowser(type, x, y, w, h, label);
    SPEC *sp = OBSPEC(ob);

    fl_add_child(ob, sp->canvas);
    fl_add_child(ob, sp->hsl);
    fl_add_child(ob, sp->vsl);

    fl_add_object(fl_current_form, ob);

    return ob;
}

FL_FORM *
fl_get_formbrowser_topform(FL_OBJECT * ob)
{
    int topline = 0;

    if (!IsFormBrowserClass(ob))
	M_err("GetFormBrowserTop",
	      "%s not a formbrowser class", ob ? ob->label : "null");
    else
	topline = OBSPEC(ob)->top_form + 1;

    return topline ? OBSPEC(ob)->form[topline-1] : 0;
}

int
fl_set_formbrowser_topform(FL_OBJECT * ob, FL_FORM * form)
{
    int n = fl_find_formbrowser_form_number(ob, form);
    SPEC *sp;

    if (n > 0)
    {
	sp = ob->spec;
	sp->top_form = n - 1;
	display_forms(sp);
	fl_set_scrollbar_value(sp->vsl, (n - 0.5f) / (sp->nforms - 1));
    }
    return n;
}

FL_FORM *
fl_set_formbrowser_topform_bynumber(FL_OBJECT * ob, int n)
{
    SPEC *sp = ob->spec;
    FL_FORM *form = 0;

    if (n > 0 && n <= sp->nforms)
    {
	sp->top_form = n - 1;
	form = sp->form[sp->top_form];
	display_forms(sp);
	fl_set_scrollbar_value(sp->vsl, (n - 0.5f) / (sp->nforms - 1));
    }
    return form;
}

int
fl_addto_formbrowser(FL_OBJECT * ob, FL_FORM * form)
{
    int return_value = 0;
    SPEC *sp = OBSPEC(ob);

    if (!IsFormBrowserClass(ob))
	M_err("AddtoFormBrowser",
	      "%s not a formbrowser class", ob ? ob->label : "null");
    else if (!form)
	M_err("AddtoFormBrowser", "Invalid argument");
    else
    {
	if (form->attached)
	{
	    M_err("AddtoFormBrowser", "Already attached ?");
	}

	if (form->visible == FL_VISIBLE)
	    fl_hide_form(form);

	if (!form->form_callback)
	    fl_set_form_callback(form, form_callback, 0);

	parentize_form(form, ob);
	sp->form = fl_realloc(sp->form, sizeof(FL_FORM *) * (sp->nforms + 1));
	sp->form[sp->nforms] = form;
	form->attached = 1;
	if (form->pre_attach)
	    form->pre_attach(form);

	if (sp->max_width < form->w)
	    sp->max_width = form->w;

	return_value = ++(sp->nforms);
	sp->max_height += form->h;
	display_forms(sp);
    }
    return return_value;
}

int
fl_find_formbrowser_form_number(FL_OBJECT * ob, FL_FORM * candidate_form)
{
    int num = 0;

    if (!IsFormBrowserClass(ob))
	M_err("FindFormBrowserIdent",
	      "%s not a formbrowser class", ob ? ob->label : "null");
    else if (!candidate_form)
	M_err("FindFormBrowserIdent", "Invalid argument");
    else
    {
	SPEC *sp = OBSPEC(ob);
	int f;
	FL_FORM **form = sp->form;
	int forms = sp->nforms;

	for (f = 0; f < forms; f++)
	{
	    if (form[f] == candidate_form)
	    {
		num = f + 1;
		break;
	    }
	}
    }
    return num;

}

int
fl_delete_formbrowser(FL_OBJECT * ob, FL_FORM * candidate_form)
{
    SPEC *sp = 0;

    if (!IsFormBrowserClass(ob))
	M_err("DeleteFormBrowser",
	      "%s not a formbrowser class", ob ? ob->label : "null");
    else if (!candidate_form)
	M_err("DeleteFormBrowser", "Invalid argument");
    else
    {
	int f = fl_find_formbrowser_form_number(ob, candidate_form);
	sp = OBSPEC(ob);
	if (f)
	    delete_form(sp, f - 1);
    }
    return sp ? sp->nforms : -1;
}

#if 0
FL_FORM *
fl_get_formbrowser_parent_form(FL_OBJECT * ob)
{
    return ob->form->parent;
}
#endif

FL_FORM *
fl_delete_formbrowser_bynumber(FL_OBJECT * ob, int num)
{
    FL_FORM *form = NULL;

    if (!IsFormBrowserClass(ob))
	M_err("DeleteFormByNumber",
	      "%s not a formbrowser class", ob ? ob->label : "null");
    else if ((num > 0) && (num <= OBSPEC(ob)->nforms))
    {
	form = OBSPEC(ob)->form[--num];
	delete_form(OBSPEC(ob), num);

    }
    else
	M_err("DeleteFormByNumber",
	      "Invalid argument -- %d not between 1 and %d",
	      num, OBSPEC(ob)->nforms);

    return form;
}

FL_FORM *
fl_replace_formbrowser(FL_OBJECT * ob, int num, FL_FORM * form)
{
    FL_FORM *old_form = NULL;

    if (!IsFormBrowserClass(ob))
	M_err("ReplaceFormByNumber",
	      "%s not a formbrowser class", ob ? ob->label : "null");
    else if ((num > 0) && (num <= OBSPEC(ob)->nforms))
    {
	SPEC *sp = OBSPEC(ob);
	int i = num - 1;

	old_form = sp->form[i];
	fl_hide_form(old_form);
	sp->form[i] = form;
	display_forms(sp);
    }
    return old_form;
}

int
fl_get_formbrowser_area(FL_OBJECT * ob, int *x, int *y, int *w, int *h)
{

    int status = 0;

    if (!IsFormBrowserClass(ob))
	M_err("GetFormbrowserArea",
	      "%s not a formbrowser class", ob ? ob->label : "null");
    else
    {
	SPEC *sp = OBSPEC(ob);

	*x = sp->canvas->x;
	*y = sp->canvas->y;
	*w = sp->canvas->w;
	*h = sp->canvas->h;
	status = 1;

    }
    return status;
}

int
fl_insert_formbrowser(FL_OBJECT * ob, int line, FL_FORM * new_form)
{
    int status = -1;

    if (!IsFormBrowserClass(ob))
	M_err("InsertForm",
	      "%s not a formbrowser class", ob ? ob->label : "null");
    else
    {
	SPEC *sp = OBSPEC(ob);
	int forms = sp->nforms;

	if ((line > 0) && (line <= forms))
	{

	    FL_FORM **form = sp->form;
	    int n = line - 1;

	    form = fl_realloc(form, sizeof(FL_FORM *) * (forms + 1));

	    parentize_form(new_form, ob);

	    if (n != forms)
		memmove(form + n + 1, form + n, sizeof(FL_FORM *) * (forms - n));
	    form[n] = new_form;
	    status = sp->nforms = forms + 1;
	    sp->form = form;
	    status = sp->nforms;
	    display_forms(sp);
	}
    }
    return status;
}

void
fl_set_formbrowser_hscrollbar(FL_OBJECT * ob, int how)
{
    SPEC *sp = ob->spec;

    if (sp->h_pref != how)
    {
	sp->h_pref = how;
	fl_redraw_object(ob);
    }
}

void
fl_set_formbrowser_vscrollbar(FL_OBJECT * ob, int how)
{
    SPEC *sp = ob->spec;

    if (sp->v_pref != how)
    {
	sp->v_pref = how;
	fl_redraw_object(ob);
    }
}

void
fl_set_formbrowser_scroll(FL_OBJECT * ob, int how)
{
    SPEC *sp = ob->spec;

    if (sp->scroll != how)
    {
	if ((sp->scroll = how) == FL_JUMP_SCROLL)
	    sp->top_edge = 0;
	fl_redraw_object(ob);
    }
}

int
fl_set_formbrowser_xoffset(FL_OBJECT * ob, int offset)
{
    SPEC *sp;
    int current;
    float val;

    if (!IsFormBrowserClass(ob))
	M_err("FormBrowserXOffset", "%s not a formbrowser class",
	      ob ? ob->label : "null");

    sp = ob->spec;
    current = sp->left_edge;

    if (offset > 0 && (current != offset) && sp->max_width > sp->canvas->w)
    {
	sp->left_edge = offset;
	val = sp->left_edge / (float) (sp->max_width - sp->canvas->w);
	fl_set_scrollbar_value(sp->hsl, val);
	fl_redraw_object(sp->hsl);
	/* fl_call_object_callback(sp->hsl); */
    }
    return current;
}

int
fl_get_formbrowser_xoffset(FL_OBJECT * ob)
{
    if (!IsFormBrowserClass(ob))
	M_err("FormBrowserXOffset", "%s not a formbrowser class",
	      ob ? ob->label : "null");

    return ((SPEC *) ob->spec)->left_edge;
}

int
fl_set_formbrowser_yoffset(FL_OBJECT * ob, int offset)
{
    SPEC *sp;
    int current;
    float val;

    if (!IsFormBrowserClass(ob))
	M_err("FormBrowserYOffset", "%s not a formbrowser class",
	      ob ? ob->label : "null");

    sp = ob->spec;
    current = sp->top_edge;

    if (offset > 0 || (current != offset) || sp->max_height > sp->canvas->h)
    {
	sp->top_edge = offset;
	val = sp->top_edge / ((float) sp->max_height - sp->canvas->h);
	fl_set_scrollbar_value(sp->vsl, val);
	fl_call_object_callback(sp->vsl);
    }
    return current;
}

int
fl_get_formbrowser_yoffset(FL_OBJECT * ob)
{
    if (!IsFormBrowserClass(ob))
	M_err("FormBrowserYOffset", "%s not a formbrowser class",
	      ob ? ob->label : "null");
    return ((SPEC *) ob->spec)->top_edge;
}

int
fl_get_formbrowser_numforms(FL_OBJECT * ob)
{
    int nforms = -1;

    if (!IsFormBrowserClass(ob))
        M_err("GetFormbrowserForms",
              "%s not a formbrowser class", ob ? ob->label : "null");
    else
    {
        SPEC *sp = OBSPEC(ob);
        nforms = sp->nforms;
    }
    return nforms;
}

FL_FORM *
fl_get_formbrowser_form(FL_OBJECT * ob, int n)
{
    FL_FORM *form = NULL;

    if (!IsFormBrowserClass(ob))
        M_err("GetFormbrowserForm",
              "%s not a formbrowser class", ob ? ob->label : "null");
    else
    {
        SPEC *sp = OBSPEC(ob);
        int nforms = sp->nforms;

        if ((1 <= n) && (n <= nforms))
            form = sp->form[n - 1];
        else
            M_err("GetFormbrowserForm",
                  "%d is not an allowable form number", n);
    }
    return form;
}


/* Internals */
static void
display_forms(SPEC *sp)
{
    int f;
    int y_pos;
    FL_OBJECT *canvas = sp->canvas;
    FL_FORM **form = sp->form;
    int forms = sp->nforms;
    int top_form = sp->top_form;
    int left_edge = -sp->left_edge;
    int height = canvas->h;	/* - (2 * absbw); */

    if (!FL_ObjWin(sp->canvas))
	return;

    fl_inherit_attributes(sp->parent, sp->canvas);

    for (f = 0; f < top_form; f++)
	if (form[f]->visible)
	    fl_hide_form(form[f]);

    y_pos = 0;
    fl_inherit_attributes(sp->parent, sp->vsl);
    fl_inherit_attributes(sp->parent, sp->hsl);

    /* like to keep scrollbar no resizable */
    sp->vsl->resize = sp->hsl->resize = FL_RESIZE_NONE;

    y_pos = sp->scroll == FL_JUMP_SCROLL ? 0 : -sp->top_edge;
    for (f = top_form; (y_pos < height) && (f < forms); f++)
    {
	if (form[f]->visible)
	    set_form_position(form[f], left_edge, y_pos);
	else
	{
	    fl_prepare_form_window(form[f], 0, FL_NOBORDER, "Formbrowser");
	    form[f]->parent_obj = sp->parent;
	    XReparentWindow(fl_get_display(),
			    (Window) form[f]->window,
			    FL_ObjWin(sp->canvas),
			    left_edge, y_pos);
	    fl_show_form_window(form[f]);
	}
	y_pos += form[f]->h;
    }


    for (; f < forms; f++)
	if (form[f]->visible)
	    fl_hide_form(form[f]);

}


static int
handle(FL_OBJECT * ob, int event, FL_Coord mx, FL_Coord my, int key, void *ev)
{
    SPEC *sp = OBSPEC(ob);

    switch (event)
    {
    case FL_ATTRIB:
	check_scrollbar(ob);
    case FL_DRAW:
	fl_set_canvas_decoration(sp->canvas, fl_boxtype2frametype(ob->boxtype));
	sp->processing_destroy = 0;
#if 0
	fl_drw_box(ob->boxtype, ob->x, ob->y, ob->w, ob->h, ob->col1, ob->bw);
#endif
	check_scrollbar(ob);
	if (!sp->in_draw && FL_ObjWin(sp->canvas))
	{
	    sp->in_draw = 1;
	    display_forms(OBSPEC(ob));
	    sp->in_draw = 0;
	}
	break;
    case FL_FREEMEM:
	fl_addto_freelist(OBSPEC(ob));
	break;
    }
    return 0;
}

/* canvas expose handler. */
static int
canvas_handler(FL_OBJECT * ob, Window win, int w, int h,
	       XEvent * ev, void *data)
{
    SPEC *sp = ob->u_vdata;

    display_forms(sp);
    return 0;
}

/* 
   Before canvas is destroyed, this routine will be called.
   we need to close the form that is attached to this canvas
 */

static int
canvas_cleanup(FL_OBJECT * ob)
{

    SPEC *sp = ob->u_vdata;
    int i;

    sp->processing_destroy = 1;

    sp->h_on = FL_OFF;
    sp->v_on = FL_OFF;

    for (i = 0; i < sp->nforms; i++)
	if (sp->form[i]->visible)
	    fl_hide_form(sp->form[i]);
    return 0;
}

/* Dummy. */
static void
form_callback(FL_OBJECT * ob, void *data)
{
}

static void
hcb(FL_OBJECT * ob, long data)
{
    SPEC *sp = ob->parent->spec;
    float val = (float)fl_get_scrollbar_value(sp->hsl);

    sp->left_edge = (int)((sp->max_width - sp->canvas->w) * val);
    fl_freeze_form(ob->form);
    display_forms(sp);
    fl_unfreeze_form(ob->form);
}

static void
vcb(FL_OBJECT * ob, long data)
{
    SPEC *sp = ob->parent->spec;
    float val = (float)fl_get_scrollbar_value(sp->vsl);

    if (sp->scroll == FL_JUMP_SCROLL)
    {
	sp->top_form = (int)(((sp->nforms - 1) * val) + 0.5f);
    }
    else
    {
	/* do pixel based scrolling */
	int pos = (int)((sp->max_height - sp->canvas->h + 1) * val + 0.4f);
	int h = 0, f;
	for (f = 0; h <= pos && f < sp->nforms; f++)
	    h += sp->form[f]->h;
	sp->top_form = f ? (f - 1) : f;
	sp->top_edge = sp->form[sp->top_form]->h - (h - pos);
    }

    fl_freeze_form(ob->form);
    display_forms(sp);
    fl_unfreeze_form(ob->form);
}

static void
set_form_position(FL_FORM * form, int x, int y)
{
    XMoveWindow(fl_get_display(), form->window, x, y);
}

static void
delete_form(SPEC *sp, int f)
{
    fl_hide_form(sp->form[f]);
    sp->form[f]->attached = 0;
    --(sp->nforms);
    sp->max_height -= sp->form[f]->h;
    for (; f < sp->nforms; f++)
	sp->form[f] = sp->form[f + 1];
    sp->form = (FL_FORM **) fl_realloc(sp->form, sizeof(FL_FORM *) * sp->nforms);
    display_forms(sp);

}

static void
parentize_form(FL_FORM * form, FL_OBJECT * ob)
{
    form->parent = ob->form;	/* This is probably the wrong way to do it. */
}

void
check_scrollbar(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    int absbw = FL_abs(ob->bw);
    int h_on = sp->h_on, v_on = sp->v_on;

    /* inheric the boxtype of parent */
    sp->hsl->boxtype = ob->boxtype;
    sp->vsl->boxtype = ob->boxtype;

    /* gravity/resize may screwup the ratios. Recompute */
    sp->canvas->x = ob->x + absbw;
    sp->canvas->y = ob->y + absbw;
    sp->canvas->w = ob->w - 2 * absbw;
    sp->canvas->h = ob->h - 2 * absbw;


    sp->v_on = sp->v_pref == FL_ON ||
	(sp->v_pref != FL_OFF && sp->canvas->h < sp->max_height);

    if (sp->v_on)
    {
	sp->vw = sp->vw_def;
	sp->vsl->x = ob->x + ob->w - sp->vsl->w;
	sp->vsl->y = ob->y;
	sp->vsl->w = sp->vw;
	sp->vsl->visible = 1;
    }
    else
    {
	sp->vsl->visible = 0;
	sp->vw = 0;
    }

    sp->canvas->w = ob->w - 2 * absbw - sp->vw;

    sp->h_on = sp->h_pref == FL_ON ||
	(sp->h_pref != FL_OFF && sp->canvas->w < sp->max_width);

    if (sp->h_on)
    {
	sp->hh = sp->hh_def;
	sp->hsl->x = ob->x;
	sp->hsl->y = ob->y + ob->h - sp->hh;
	sp->hsl->h = sp->hh;
	sp->hsl->visible = 1;
    }
    else
    {
	sp->hh = 0;
	sp->hsl->visible = 0;
    }

    /* make adjustment to the canvas size */
    sp->canvas->h = ob->h - 2 * absbw - sp->hh;

    /* recheck vertical */
    if (!sp->v_on &&
	(sp->v_pref != FL_OFF && sp->canvas->h < sp->max_height))
    {
	sp->v_on = 1;
	sp->vw = sp->vw_def;
	sp->canvas->w = ob->w - 2 * absbw - sp->vw;
    }

    sp->hsl->w = sp->canvas->w + 2 * absbw;
    sp->vsl->h = sp->canvas->h + 2 * absbw;


    /* if scrollbars are turned-off due to resize, adjust the offsets. */
    if (!sp->v_on && v_on)
    {
	sp->top_edge = 0;
	fl_set_scrollbar_value(sp->vsl, 0.0);
    }

    if (!sp->h_on && h_on)
    {
	sp->left_edge = 0;
	fl_set_scrollbar_value(sp->hsl, 0.0);
    }

    sp->vsl->visible = sp->v_on;
    sp->hsl->visible = sp->h_on;

    fl_winresize(FL_ObjWin(sp->canvas), sp->canvas->w, sp->canvas->h);
}
