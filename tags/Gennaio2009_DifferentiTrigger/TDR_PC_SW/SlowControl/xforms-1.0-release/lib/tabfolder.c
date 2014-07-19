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
 * $Id: tabfolder.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1997-2002  By T.C. Zhao 
 *  All rights reserved.
 *.
 *
 * tabbed folder
 *
 */

#include "forms.h"

#define TBDEBUG    0

typedef struct
{
    FL_OBJECT *canvas;
    FL_OBJECT *parent;		/* the tabfolder object         */
    FL_FORM **forms;		/* the folders                  */
    FL_OBJECT **title;		/* the associted tab            */
    int nforms;			/* number of folders            */
    int active_folder;		/* current active folder        */
    int last_active;		/* the previous active folder   */
    int non_interactive;
    int x, y;
    int max_h;
    int h_pad, v_pad;
    int processing_destroy;
    int auto_fit;
    int offset;
    int num_visible;
}
SPEC;

static void compute_position(FL_OBJECT *);
static void switch_folder(FL_OBJECT *, long);
static void program_switch(FL_OBJECT *, int);
static void get_tabsize(FL_OBJECT *, const char *, int *, int *, int);
static void shift_tabs(FL_OBJECT * ob, int left);
#define IsFolderClass(ob) ((ob) && (ob)->objclass==FL_TABFOLDER)

#if TBDEBUG
static void fl_print_tabfolder(FL_OBJECT * ob, const char *s);
#endif


static int
handle(FL_OBJECT * ob, int event, FL_Coord mx, FL_Coord my,
       int key, void *ev)
{
    SPEC *sp = ob->spec;

    switch (event)
    {
    case FL_ATTRIB:
    case FL_DRAW:
	fl_set_canvas_decoration(sp->canvas, fl_boxtype2frametype(ob->boxtype));
	sp->processing_destroy = 0;
#if 0
	/* for size testing */
	fl_drw_box(FL_FLAT_BOX, ob->x, ob->y, ob->w, ob->h, FL_RED, 0);
#endif
	compute_position(ob);
	break;
    case FL_FREEMEM:
	fl_addto_freelist(ob->spec);
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

    /* sp->nforms can be zero */
    if ((sp->nforms == 0) || (sp->active_folder >= sp->nforms))
	return 0;

    if (sp->active_folder >= 0)	/* regular exposure, not first time */
	program_switch(sp->title[sp->active_folder], sp->active_folder);
    else if (sp->last_active >= 0 && sp->last_active < sp->nforms)
	program_switch(sp->title[sp->last_active], sp->last_active);

    return 0;
}


/* Before canvas is destroyed, this routine will be called.
   we need to close the form that is attached to this canvas
 */
static int
canvas_cleanup(FL_OBJECT * ob)
{
    SPEC *sp = ob->u_vdata;

    if (sp->active_folder >= 0 && sp->active_folder < sp->nforms)
    {
	sp->processing_destroy = 1;
	if (sp->forms[sp->active_folder]->visible)
	    fl_hide_form(sp->forms[sp->active_folder]);

	sp->last_active = sp->active_folder;

	if (sp->active_folder >= 0)
	    fl_set_object_boxtype(sp->title[sp->active_folder],
		 ob->parent->type != FL_BOTTOM_TABFOLDER ? FL_TOPTAB_UPBOX :
				  FL_BOTTOMTAB_UPBOX);
	sp->active_folder = -1;
    }

    return 0;
}

/* for all the folders, we set a dummy form callback to prevent
 * the object on the form from leaking thru fl_do_forms
 */
static void
form_callback(FL_OBJECT * ob, void *data)
{
    return;
}

FL_OBJECT *
fl_create_tabfolder(int type, FL_Coord x, FL_Coord y, FL_Coord w,
		    FL_Coord h, const char *label)
{
    FL_OBJECT *ob;
    SPEC *sp;
    int absbw, oldu = fl_get_coordunit();;

    ob = fl_make_object(FL_TABFOLDER, type, x, y, w, h, label, handle);
    fl_set_coordunit(FL_COORD_PIXEL);

    ob->boxtype = FL_UP_BOX;
    absbw = FL_abs(ob->bw);

    ob->spec_size = sizeof(SPEC);
    sp = ob->spec = fl_calloc(1, ob->spec_size);
    sp->parent = ob;
    sp->forms = fl_malloc(sizeof(**sp->forms));
    sp->title = fl_malloc(sizeof(**sp->title));

    sp->x = ob->x + absbw;
    sp->y = ob->y + absbw;

    sp->h_pad = 12;
    sp->v_pad = 5;
    sp->auto_fit = FL_NO;

    sp->canvas = fl_create_canvas(FL_SCROLLED_CANVAS, sp->x, sp->y,
				  ob->w - 2 * absbw,
				  ob->h - 2 * absbw, label ? label : "tab");

    sp->canvas->u_vdata = sp;
    fl_modify_canvas_prop(sp->canvas, 0, 0, canvas_cleanup);
    fl_set_canvas_decoration(sp->canvas, fl_boxtype2frametype(ob->boxtype));
    fl_add_canvas_handler(sp->canvas, Expose, canvas_handler, 0);

    fl_set_object_color(sp->canvas, ob->col1, ob->col2);
    fl_set_object_bw(sp->canvas, ob->bw);
    fl_set_object_gravity(sp->canvas, ob->nwgravity, ob->segravity);
    fl_set_coordunit(oldu);

    return ob;
}

FL_OBJECT *
fl_add_tabfolder(int type, int x, int y, int w, int h, const char *l)
{
    FL_OBJECT *ob = fl_create_tabfolder(type, x, y, w, h, l);
    SPEC *sp = ob->spec;

    fl_add_child(ob, sp->canvas);
    fl_add_object(fl_current_form, ob);

    return ob;
}

int
fl_get_tabfolder_numfolders(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    return sp->nforms;
}

/* switched by the application. No need to invoke the callback */
static void
program_switch(FL_OBJECT * ob, int folder)
{
    SPEC *sp;

    if (folder >= 0)
    {
	sp = ob->u_vdata;
	sp->non_interactive = 1;
	switch_folder(ob, folder);
	sp->non_interactive = 0;
	/* this handles set_folder while hidden */
	if (!ob->visible || !ob->form->visible)
	    sp->last_active = folder;
    }
}

static void
switch_folder(FL_OBJECT * ob, long data)
{
    SPEC *sp = ob->u_vdata;
    FL_FORM *form = sp->forms[data];
    Window win;
    FL_OBJECT *bkob;
    int active = data;

#if TBDEBUG
    fl_print_tabfolder(sp->parent, "SwitchFolder");
    fprintf(stderr, "data=%ld active=%d lastactive=%d\n",
	    data, sp->active_folder, sp->last_active);
#endif


    if (active == sp->active_folder || sp->processing_destroy)
    {
	sp->processing_destroy = 0;
	return;
    }

    if (!ob->form->window || !FL_ObjWin(sp->canvas))
    {
	return;
    }

    if (sp->auto_fit != FL_NO)
    {
	if (sp->auto_fit == FL_FIT)
	    fl_set_form_size(form, sp->canvas->w, sp->canvas->h);
	else if (form->w < sp->canvas->w || form->h < sp->canvas->h)
	    fl_set_form_size(form, sp->canvas->w, sp->canvas->h);
    }

    /* we have more tabs than that can be shown */
    if (sp->num_visible < sp->nforms - 1 || sp->offset)
    {
	if ((active && active == sp->offset) || active > sp->num_visible)
	{
	    int last;
	    shift_tabs(ob, active == sp->offset ? -1 : 1);
	    sp->title[active]->boxtype &= ~FL_BROKEN_BOX;
	    sp->title[active]->align = FL_ALIGN_CENTER;
	    last = sp->num_visible + sp->offset + 1;
	    last = FL_clamp(last, 0, sp->nforms - 1);
	    sp->title[last]->boxtype |= FL_BROKEN_BOX;
	    sp->title[last]->align = FL_ALIGN_LEFT | FL_ALIGN_INSIDE;
	    fl_redraw_form(ob->form);
	}
    }

    win = fl_prepare_form_window(form, 0, 0, "Folder");

    /* win reparent eats the reparent event */
    fl_winreparent(win, FL_ObjWin(sp->canvas));
    form->parent_obj = ob;
    fl_show_form_window(form);

    /* need to redraw the last selected folder tab */
    if (sp->active_folder >= 0 && sp->forms[sp->active_folder]->visible)
    {
	FL_OBJECT *actobj;

	actobj = sp->title[sp->active_folder];
	actobj->col1 = sp->parent->col1;

	fl_set_object_boxtype(actobj,
		 ob->parent->type != FL_BOTTOM_TABFOLDER ? FL_TOPTAB_UPBOX :
			      FL_BOTTOMTAB_UPBOX);

	fl_drw_frame(FL_UP_FRAME, sp->canvas->x, sp->canvas->y, sp->canvas->w,
		     sp->canvas->h, sp->canvas->col1, sp->canvas->bw);
	fl_hide_form(sp->forms[sp->active_folder]);
	sp->forms[sp->active_folder]->parent_obj = 0;
	sp->last_active = sp->active_folder;
    }

    form->parent = ob->form;
    ob->form->child = form;

    /* find out the color of the new form */
    if ((bkob = form->first) && bkob->type == FL_NO_BOX)
	bkob = bkob->next;

    if (bkob)
	fl_set_object_color(ob, bkob->col1, ob->col2);

    fl_set_object_boxtype(ob, ob->parent->type != FL_BOTTOM_TABFOLDER ?
		    FL_SELECTED_TOPTAB_UPBOX : FL_SELECTED_BOTTOMTAB_UPBOX);

    sp->active_folder = active;

    if (!sp->non_interactive)
	fl_call_object_callback(ob->parent);
}


/*
 * add a new folder to the bunch
 */
FL_OBJECT *
fl_addto_tabfolder(FL_OBJECT * ob, const char *title, FL_FORM * form)
{
    SPEC *sp = ob->spec;
    FL_OBJECT *tab;
    static const char *func = "AddtoFolder";

    if (!IsFolderClass(ob))
    {
	M_err(func, "%s not a folder class", ob ? ob->label : "null");
	return 0;
    }

    if (!form || !title)
    {
	M_err(func, "Invalid argument(s)");
	return 0;
    }

    if (form->attached)
    {
	M_err(func, "Seems the form already attached");
	return 0;
    }

    if (form->visible == FL_VISIBLE)
	fl_hide_form(form);

    sp->forms = fl_realloc(sp->forms, sizeof(*sp->forms) * (sp->nforms + 1));

    /* plug the possible object leakage thru fl_do_forms */
    if (!form->form_callback)
	fl_set_form_callback(form, form_callback, 0);

    sp->forms[sp->nforms] = form;
    form->attached = 1;

    if (form->pre_attach)
	form->pre_attach(form);

    tab = sp->title[sp->nforms] = fl_create_button(FL_NORMAL_BUTTON,
						   0, 0, 10, 10, title);

    fl_set_object_boxtype(tab,
    ob->type != FL_BOTTOM_TABFOLDER ? FL_TOPTAB_UPBOX : FL_BOTTOMTAB_UPBOX);

    tab->u_vdata = sp;
    fl_set_object_callback(tab, switch_folder, sp->nforms);

    sp->nforms++;
    compute_position(ob);

    fl_add_child(ob, tab);
    fl_add_object(ob->form, tab);

    if (sp->nforms == 1)
    {
	sp->last_active = 0;
	sp->active_folder = -1;
	program_switch(sp->title[sp->last_active], sp->last_active);
    }

    /* if first time and the canvas is visible, refresh */
    if (sp->nforms == 1 && ob->visible)
	fl_redraw_form(ob->form);

    return tab;
}

static void
get_tabsize(FL_OBJECT * ob, const char *label, int *ww, int *hh, int fudge)
{
    int w, absbw = FL_abs(ob->bw), h;
    SPEC *sp = ob->spec;

    fl_get_string_dimension(ob->lstyle, ob->lsize, label, strlen(label),
			    &w, &h);
    w += sp->h_pad + 2 * absbw;
    h += sp->v_pad + 2 * absbw;

    *hh = h + fudge * absbw;
    *ww = w;

    return;
}

void
fl_delete_folder_byname(FL_OBJECT * ob, const char *name)
{
    SPEC *sp = ob->spec;
    int i, done;

    for (done = 0, i = 0; !done && i < sp->nforms; i++)
    {
	if (strcmp(sp->title[i]->label, name) == 0)
	    done = i + 1;
    }

    if (done)
	fl_delete_folder_bynumber(ob, done);

}

#if TBDEBUG
void 
fl_print_tabfolder(FL_OBJECT * ob, const char *s)
{
    SPEC *sp = ob->spec;
    int i;

    fprintf(stderr, "%s*************\n", s);
    for (i = 0; i < sp->nforms; i++)
    {
	fprintf(stderr, "folder%d: title=%s form=%p\n",
		i, sp->title[i]->label, sp->forms[i]);
    }
}
#endif

void
fl_delete_folder_bynumber(FL_OBJECT * ob, int num)
{
    int i = num - 1, j;
    SPEC *sp = ob->spec;
    FL_OBJECT *deleted = 0;
    FL_FORM *theform = 0;

#if TBDEBUG
    fl_print_tabfolder(ob, "before deletion");
#endif
    if (i >= 0 && i < sp->nforms)
    {
	deleted = sp->title[i];
	fl_detach_form(theform = sp->forms[i]);
	for (j = i + 1; j < sp->nforms; j++)
	{
	    sp->title[j - 1] = sp->title[j];
	    sp->title[j - 1]->argument = j - 1;
	    sp->forms[j - 1] = sp->forms[j];
	}
	sp->nforms--;
    }

#if TBDEBUG
    fl_print_tabfolder(ob, "after deletion");
#endif

    if (deleted)
    {
/*      fl_hide_object(deleted); */
	deleted->visible = 0;

	if (theform->form_callback == form_callback)
	    theform->form_callback = 0;

	if (theform->visible)
	    fl_hide_form(theform);

	/* change active folder if need to */
	sp->last_active = -1;

	if (i < sp->active_folder)
	{
	    sp->active_folder--;
	}
	else if (i == sp->active_folder)
	{
	    sp->active_folder = -1;
	    fl_set_folder_bynumber(ob, i);
	}

	fl_delete_object(deleted);
	fl_free_object(deleted);
	fl_redraw_form(ob->form);
    }
}

FL_FORM *
fl_get_tabfolder_folder_bynumber(FL_OBJECT * ob, int num)
{
    SPEC *sp = ob->spec;
    int i = num - 1;

    return (i >= 0 && i < sp->nforms) ? sp->forms[i] : 0;
}

FL_FORM *
fl_get_tabfolder_folder_byname(FL_OBJECT * ob, const char *name)
{
    int i;
    SPEC *sp = ob->spec;

    for (i = 0; i < sp->nforms; i++)
	if (strcmp(sp->title[i]->label, name) == 0)
	    return fl_get_tabfolder_folder_bynumber(ob, i + 1);
    return 0;
}

void
fl_delete_folder(FL_OBJECT * ob, FL_FORM * form)
{
    int i, done;
    SPEC *sp = ob->spec;

    for (done = i = 0; !done && i < sp->nforms; i++)
    {
	if (form == sp->forms[i])
	    done = i + 1;
    }

    if (done)
    {
	fl_delete_folder_bynumber(ob, done);
    }
}


void
fl_set_folder(FL_OBJECT * ob, FL_FORM * form)
{
    SPEC *sp;
    int i, done;

    if (!IsFolderClass(ob))
    {
	M_err("SetFolder", "%s is not tabfolder", ob ? ob->label : "null");
	return;
    }

    sp = ob->spec;
    for (done = i = 0; !done && i < sp->nforms; i++)
    {
	if (sp->forms[i] == form)
	{
	    program_switch(sp->title[i], i);
	    done = 1;
	}
    }
}

void
fl_set_folder_byname(FL_OBJECT * ob, const char *name)
{
    SPEC *sp;
    int i, done;

    if (!IsFolderClass(ob))
    {
	M_err("SetFolderName", "%s is not tabfolder", ob ? ob->label : "null");
	return;
    }

    sp = ob->spec;
    for (done = i = 0; !done && i < sp->nforms; i++)
    {
	if (strcmp(sp->title[i]->label, name) == 0)
	{
	    program_switch(sp->title[i], i);
	    done = 1;
	}
    }
}

void
fl_set_folder_bynumber(FL_OBJECT * ob, int num)
{
    SPEC *sp;
    int i = num - 1;

    if (!IsFolderClass(ob))
    {
	M_err("SetFolderNum", "%s is not tabfolder", ob ? ob->label : "null");
	return;
    }

    sp = ob->spec;
    if (i >= 0 && i < sp->nforms)
	program_switch(sp->title[i], i);
}

/* keep tab, but replace the folder content */
void
fl_replace_folder_bynumber(FL_OBJECT * ob, int num, FL_FORM * form)
{
    SPEC *sp = ob->spec;
    int i = num - 1;

    if (i >= 0 && i < sp->nforms && sp->forms[i] != form)
    {
	sp->forms[i] = form;

	if (i == sp->active_folder)
	{
	    sp->active_folder = -1;
	    program_switch(sp->title[i], i);
	}
    }
}


int
fl_get_folder_number(FL_OBJECT * ob)
{
    if (!IsFolderClass(ob))
    {
	M_err("GetFolderNum", "%s is not tabfolder", ob ? ob->label : "null");
	return 0;
    }

    return ((SPEC *) (ob->spec))->last_active + 1;
}

int
fl_get_active_folder_number(FL_OBJECT * ob)
{
    if (!IsFolderClass(ob))
    {
	M_err("GetFolderNum", "%s is not tabfolder", ob ? ob->label : "null");
	return 0;
    }

    return ((SPEC *) (ob->spec))->active_folder + 1;
}

FL_FORM *
fl_get_folder(FL_OBJECT * ob)
{
    SPEC *sp;

    if (!IsFolderClass(ob))
    {
	M_err("GetFolder", "%s is not tabfolder", ob ? ob->label : "null");
	return 0;
    }

    sp = ob->spec;

    return (sp->last_active >= 0 ? sp->forms[sp->last_active] : 0);
}

const char *
fl_get_folder_name(FL_OBJECT * ob)
{
    SPEC *sp;

    if (!IsFolderClass(ob))
    {
	M_err("GetFolderName", "%s is not tabfolder", ob ? ob->label : "null");
	return 0;
    }

    sp = ob->spec;
    return (sp->last_active >= 0 ? sp->title[sp->last_active]->label : 0);
}


FL_FORM *
fl_get_active_folder(FL_OBJECT * ob)
{
    SPEC *sp;

    if (!IsFolderClass(ob))
    {
	M_err("GetFolder", "%s is not tabfolder", ob ? ob->label : "null");
	return 0;
    }

    sp = ob->spec;

    return (sp->active_folder >= 0 ? sp->forms[sp->active_folder] : 0);
}

const char *
fl_get_active_folder_name(FL_OBJECT * ob)
{
    SPEC *sp;

    if (!IsFolderClass(ob))
    {
	M_err("GetActiveFolderName", "%s is not tabfolder", ob ? ob->label : "null");
	return 0;
    }

    sp = ob->spec;
    return (sp->active_folder >= 0 ? sp->title[sp->active_folder]->label : 0);
}


void
fl_get_folder_area(FL_OBJECT * ob, int *x, int *y, int *w, int *h)
{
    SPEC *sp = ob->spec;

    compute_position(ob);
    *x = sp->canvas->x;
    *y = sp->canvas->y;
    *w = sp->canvas->w;
    *h = sp->canvas->h;
}

int
fl_get_tabfolder_offset(FL_OBJECT * ob)
{
    return ((SPEC *) (ob->spec))->offset;
}

int
fl_set_tabfolder_offset(FL_OBJECT * ob, int offset)
{
    SPEC *sp = ob->spec;
    int old = sp->offset;

    if (offset < 0)
	offset = 0;
    else if ((offset + sp->num_visible + 1) > sp->nforms - 1)
	offset = sp->nforms - sp->num_visible;

    if (offset != sp->offset)
    {
	shift_tabs(ob, offset - sp->offset);
	fl_redraw_form(ob->form);
    }

    return old;
}

/* compute the position and propagate the parent attributes */
static void
compute_top_position(FL_OBJECT * ob)
{
    SPEC *sp ;
    FL_OBJECT *tab;
    int i, max_h = 4;

    sp = ob->objclass == FL_TABFOLDER ? ob->spec : ob->u_vdata;

    sp->y = ob->y + 1;
    sp->x = sp->canvas->x - FL_abs(sp->canvas->bw);

    for (i = 0; i < sp->offset; i++)
	sp->title[i]->x = 2000;

    /* this gets the fl_get_folder_area() right (singe line tab)  * even if
       empty folder */
    if (sp->nforms == 0)
    {
	int junk;
	get_tabsize(ob, "AjbY", &junk, &max_h, 1);
    }

    for (i = sp->offset; i < sp->nforms; i++)
    {
	tab = sp->title[i];
	get_tabsize(ob, tab->label, &tab->w, &tab->h, 1);
	if (tab->h > max_h)
	    max_h = tab->h;
	tab->x = sp->x;
	tab->y = sp->y;
	fl_inherit_attributes(ob, tab);
	sp->x += tab->w + (ob->bw > 0);
	if (sp->x < (sp->canvas->x + sp->canvas->w) - 2)
	{
	    sp->num_visible = i;
	    tab->boxtype &= ~FL_BROKEN_BOX;
	    tab->align = FL_ALIGN_CENTER;
	    tab->visible = 1;
	}
	else if ((tab->w -= (sp->x - sp->canvas->x - sp->canvas->w)) > 0)
	{
	    tab->boxtype |= FL_BROKEN_BOX;
	    tab->align = FL_ALIGN_LEFT | FL_ALIGN_INSIDE;
	    tab->visible = 1;
	}
	else
	{
	    tab->w = 20;
	    tab->visible = 0;
	}
    }

    for (i = 0; i < sp->nforms; i++)
	sp->title[i]->h = max_h;

    /* this will be the canvas location */
    sp->canvas->y = sp->y + max_h - (ob->bw < 0);
    sp->canvas->h = ob->h - max_h - FL_abs(ob->bw) - 1;
    sp->max_h = max_h;
    fl_set_object_color(sp->canvas, ob->col1, ob->col2);
}

static void
compute_bottom_position(FL_OBJECT * ob)
{
    SPEC *sp;
    FL_OBJECT *tab;
    int i, max_h = 4, absbw = FL_abs(ob->bw);

    sp = ob->objclass == FL_TABFOLDER ? ob->spec:ob->u_vdata;
    sp->x = ob->x;

    if (sp->nforms == 0)
    {
	int junk;
	get_tabsize(ob, "AjbY", &junk, &max_h, -1);
    }

    for (i = 0; i < sp->nforms; i++)
    {
	tab = sp->title[i];
	get_tabsize(ob, tab->label, &tab->w, &tab->h, -1);
	if (tab->h > max_h)
	    max_h = tab->h;
	tab->x = sp->x;
	fl_inherit_attributes(ob, tab);
	sp->x += tab->w + (ob->bw > 0);
    }

    sp->canvas->h = ob->h - 2 * absbw - max_h - 1;
    sp->y = sp->canvas->y + sp->canvas->h + absbw - (ob->bw < 0);

    for (i = 0; i < sp->nforms; i++)
    {
	sp->title[i]->h = max_h;
	sp->title[i]->y = sp->y;
    }

    sp->max_h = max_h;
    fl_set_object_color(sp->canvas, ob->col1, ob->col2);
}

static void
compute_position(FL_OBJECT * ob)
{
#if 0
    if (((SPEC *) (ob->spec))->nforms == 0)
	return;
#endif
#if TBDEBUG
    fl_print_tabfolder(ob, "computing position");
#endif

    if (ob->type == FL_BOTTOM_TABFOLDER)
	compute_bottom_position(ob);
    else
	compute_top_position(ob);

}

int
fl_set_tabfolder_autofit(FL_OBJECT * ob, int y)
{
    SPEC *sp = ob->spec;
    int old = sp->auto_fit;

    sp->auto_fit = y;
    return old;
}


static void
shift_tabs(FL_OBJECT * ob, int left)
{
    SPEC *sp = ob->u_vdata;
    int newp = sp->offset + left;

    if (newp < 0)
	newp = 0;

    if (newp == sp->offset)
	return;

    sp->offset = newp;

    compute_position(ob);
}

void
fl_detach_form(FL_FORM * form)
{
    form->attached = 0;
    if (form->parent && form->parent->child == form)
	form->parent->child = 0;
    form->parent = 0;
}
