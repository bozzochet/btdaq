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
 * $Id: child.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao 
 *  All rights reserved.
 *.
 *
 * Temprary hack to get composite (sort of) object support in
 *
 */
#include "forms.h"

void
fl_add_child(FL_OBJECT * parent, FL_OBJECT * child)
{
    FL_OBJECT *t;

    if (child->form)
	fl_delete_object(child);

    if (child->child || !child->parent)
	child->parent = parent;

    parent->parent = parent;
    child->is_child = 1;

    /* copy gravity attributes */
    child->nwgravity = parent->nwgravity;
    child->segravity = parent->segravity;
    child->resize = parent->resize;

    if (parent->child == 0)
	parent->child = child;
    else
    {
	for (t = parent->child; t && t->nc; t = t->nc)
	    ;
	t->nc = child;
    }

    child->nc = child->child;

}

void
fl_delete_child(FL_OBJECT * child)
{
    FL_OBJECT *obj;

    for (obj = child->parent->child; obj && obj->nc != child; obj = obj->nc)
	;
    if (obj)
    {
	obj->nc = child->nc;
	child->is_child = 0;
	child->nc = 0;
    }
}

void
fl_hide_composite(FL_OBJECT * ob)
{
    FL_OBJECT *tmp = ob->child;

    for (; tmp; tmp = tmp->nc)
    {
	if (tmp->objclass == FL_CANVAS)
	    fl_hide_canvas(tmp);
	tmp->visible = 0;
    }
}


void
fl_delete_composite(FL_OBJECT * ob)
{
    FL_OBJECT *tmp = ob->child;

    for (; tmp; tmp = tmp->nc)
    {
	if (tmp->form)
	    fl_delete_object(tmp);
    }
}

void
fl_show_composite(FL_OBJECT * ob)
{
    FL_OBJECT *tmp = ob->child;

    for (; tmp; tmp = tmp->nc)
	tmp->visible = 1;
}

void
fl_deactivate_composite(FL_OBJECT * ob)
{
    FL_OBJECT *tmp = ob->child;

    ob->parent->active = DEACTIVATED;
    for (; tmp; tmp = tmp->nc)
	tmp->active = DEACTIVATED;;
}

void
fl_activate_composite(FL_OBJECT * ob)
{
    FL_OBJECT *tmp = ob->child;

    ob->parent->active = 1;
    for (; tmp; tmp = tmp->nc)
	tmp->active = 1;
}

void
fl_set_composite_resize(FL_OBJECT * ob, unsigned resize)
{
    FL_OBJECT *tmp = ob->child;

    for (; tmp; tmp = tmp->nc)
	tmp->resize = resize;
}

void
fl_set_composite_gravity(FL_OBJECT * ob, unsigned nw, unsigned se)
{
    FL_OBJECT *tmp = ob->child;

    for (; tmp; tmp = tmp->nc)
    {
	tmp->nwgravity = nw;
	tmp->segravity = se;
    }
}

void
fl_insert_composite_after(FL_OBJECT * comp, FL_OBJECT * node)
{
    FL_OBJECT *next, *tmp, *prev;
    FL_FORM *form;

    if (!comp || !node)
    {
	M_err("InsertComp", "Bad argument");
	return;
    }

    if (!(form = node->form))
    {
	M_err("InsertComp", "null form");
	return;
    }

    comp->form = form;

    next = node->next;
    node->next = comp;
    comp->prev = node;
    comp->child->form = form;
    comp->next = comp->child;
    comp->next->prev = comp;

    prev = comp;

    for (tmp = comp->child; tmp && tmp->nc; prev = tmp, tmp = tmp->nc)
    {
	tmp->form = form;
	tmp->next = tmp->nc;
	tmp->prev = prev;
    }

    tmp->next = next;
    tmp->prev = prev;
    tmp->form = form;

    if (form->last == node)
	form->last = tmp;

}

/* change the parent of a composite. Need to take care for
 * composite of composite
 */
void
fl_change_composite_parent(FL_OBJECT * comp, FL_OBJECT * newparent)
{
    FL_OBJECT *tmp = comp->child;

    comp->parent = newparent;
    for (; tmp; tmp = tmp->nc)
	if (tmp->parent == comp)
	    tmp->parent = newparent;
}


void
fl_add_composite(FL_FORM * form, FL_OBJECT * ob)
{
    FL_OBJECT *tmp = ob->child, *tmp1 = ob;

    for (; tmp; tmp1 = tmp, tmp = tmp->nc)
	fl_add_object(form, tmp);

    if (form->last == ob)
	form->last = tmp1;
}


FL_OBJECT *
fl_get_object_component(FL_OBJECT * composite, int objclass, int type, int numb)
{
    FL_OBJECT *tmp;
    int n;

    for (n = 0, tmp = composite->child; tmp; tmp = tmp->nc)
    {
	if (tmp->objclass == objclass && (tmp->type == type || type < 0))
	{
	    if ((++n) >= numb)
		return tmp;
	}
    }

    M_err("GetComponent", "requested object not found");

    return 0;
}

void
fl_mark_composite_for_redraw(FL_OBJECT * ob)
{
    FL_OBJECT *tmp = ob->child;
    for (; tmp; tmp = tmp->nc)
    {
       /* M_err("Marking", "%s", fl_object_class_name(ob)); */
	tmp->redraw = 1;
    }
}

/*
 * copy the parent attributes. gravity stuff is taken care of by
 * fl_add_child()
 */

void
fl_inherit_attributes(FL_OBJECT * parent, FL_OBJECT * child)
{
    child->bw = parent->bw;
    child->lcol = parent->lcol;
    child->col1 = parent->col1;
    child->lsize = parent->lsize;
    child->lstyle = parent->lstyle;
}
