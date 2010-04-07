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
 * $Id: objects.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *
 */

#if defined(F_ID) || defined(DEBUG)
char *fl_id_obj = "$Id: objects.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include <string.h>
#include "forms.h"

/* Creates an empty form. NOT FOR USER. */
#define DELTA 0.4
#define PointToPixel(a)     ((a) * fl_dpi / 72.0  + DELTA)
#define MMToPixel(a)        ((a) * fl_dpi / 25.4  + DELTA)
#define CMMToPixel(a)       ((a) * fl_dpi / 2540.0 + DELTA)
#define CPointToPixel(a)    ((a) * fl_dpi / 7200.0 + DELTA)

#define TRANY(ob,form)   (form->h - ob->h - ob->y)

#define LInside(a)    (((a)==FL_ALIGN_CENTER) || ((a)&FL_ALIGN_INSIDE))

static void lose_focus(FL_OBJECT * ob);

FL_FORM *
fl_make_form(FL_Coord w, FL_Coord h)
{
    FL_FORM *form;

    form = fl_calloc(1, sizeof(FL_FORM));

    /* convert non-pixel unit into pixles */
    if (fl_cntl.coordUnit == FL_COORD_MM)
    {
	w = MMToPixel(w);
	h = MMToPixel(h);
    }
    else if (fl_cntl.coordUnit == FL_COORD_POINT)
    {
	w = PointToPixel(w);
	h = PointToPixel(h);
    }
    else if (fl_cntl.coordUnit == FL_COORD_centiPOINT)
    {
	w = CPointToPixel(w);
	h = CPointToPixel(h);
    }
    else if (fl_cntl.coordUnit == FL_COORD_centiMM)
    {
	w = CMMToPixel(w);
	h = CMMToPixel(h);
    }
    else if (fl_cntl.coordUnit != FL_COORD_PIXEL)
    {
	M_err("makeform", "Unknown unit: %d. Reset to pixel", fl_cntl.coordUnit);
	fl_cntl.coordUnit = FL_COORD_PIXEL;
    }

    /* initialize pointers and non-zero defaults */
    form->w = w;
    form->h = h;
    form->deactivated = 1;
    form->form_callback = 0;
    form->compress_mask = ExposureMask | ButtonMotionMask | PointerMotionMask;
    form->key_callback = form->push_callback = form->crossing_callback = 0;
    form->focusobj = 0;
    form->first = form->last = 0;
    form->hotx = form->hoty = -1;
    form->use_pixmap = fl_cntl.doubleBuffer;
    form->label = 0;
    form->u_vdata = 0;
    form->close_callback = 0;
    form->close_data = 0;
    form->icon_pixmap = form->icon_mask = 0;
    form->no_tooltip = 0;

    return form;
}

#define BUTTON_CLASS(i)   (i==FL_BUTTON || i==FL_ROUNDBUTTON ||\
			   i==FL_LIGHTBUTTON  || i==FL_CHECKBUTTON ||\
			   i==FL_BITMAPBUTTON || i==FL_PIXMAPBUTTON)

/* Creates an object, NOT FOR USER. */
FL_OBJECT *
fl_make_object(int objclass, int type, FL_Coord x, FL_Coord y,
	     FL_Coord w, FL_Coord h, const char *label, FL_HANDLEPTR handle)
{
    FL_OBJECT *ob;
#ifdef FL_WIN32
    int def = -2;
#else
    int def = FL_BOUND_WIDTH;
#endif

    ob = fl_calloc(1, sizeof(FL_OBJECT));


    ob->objclass = objclass;
    ob->type = type;
    ob->resize = FL_RESIZE_ALL;
    ob->nwgravity = ob->segravity = ForgetGravity;
    ob->boxtype = FL_NO_BOX;
    ob->bw = (fl_cntl.borderWidth && FL_abs(fl_cntl.borderWidth) <= 10) ?
	fl_cntl.borderWidth : def;

    ob->x = x;
    ob->y = y;
    ob->w = w;
    ob->h = h;

    if (fl_cntl.coordUnit == FL_COORD_PIXEL)
	;
    else if (fl_cntl.coordUnit == FL_COORD_MM)
	fl_scale_object(ob, fl_dpi / 25.4, fl_dpi / 25.4);
    else if (fl_cntl.coordUnit == FL_COORD_POINT)
	fl_scale_object(ob, fl_dpi / 72.0, fl_dpi / 72.0);
    else if (fl_cntl.coordUnit == FL_COORD_centiPOINT)
	fl_scale_object(ob, fl_dpi / 7200.0, fl_dpi / 7200.0);
    else if (fl_cntl.coordUnit == FL_COORD_centiMM)
	fl_scale_object(ob, fl_dpi / 2540.0, fl_dpi / 2540.0);
    else
    {
	M_err("MakeObject", "Unknown unit: %d. Reset", fl_cntl.coordUnit);
	fl_cntl.coordUnit = FL_COORD_PIXEL;
    }

    ob->wantkey = FL_KEY_NORMAL;

    ob->flpixmap = 0;

    if (!label)
	label = "";

    ob->label = fl_strdup(label);
    ob->handle = handle;
    ob->align = FL_ALIGN_CENTER;
    ob->lcol = FL_BLACK;
    ob->col1 = FL_COL1;
    ob->col2 = FL_MCOL;

    if (BUTTON_CLASS(objclass) && fl_cntl.buttonFontSize)
	ob->lsize = fl_cntl.buttonFontSize;
    else if (objclass == FL_MENU && fl_cntl.menuFontSize)
	ob->lsize = fl_cntl.menuFontSize;
    else if (objclass == FL_CHOICE && fl_cntl.choiceFontSize)
	ob->lsize = fl_cntl.choiceFontSize;
    else if (objclass == FL_INPUT && fl_cntl.inputFontSize)
	ob->lsize = fl_cntl.inputFontSize;
    else if (objclass == FL_SLIDER && fl_cntl.sliderFontSize)
	ob->lsize = fl_cntl.sliderFontSize;
#if 0
    else if (objclass == FL_BROWSER && fl_cntl.browserFontSize)
	ob->lsize = fl_cntl.browserFontSize;
#endif
    else if (fl_cntl.labelFontSize)
	ob->lsize = fl_cntl.labelFontSize;
    else
	ob->lsize = FL_DEFAULT_SIZE;

    ob->lstyle = FL_NORMAL_STYLE;
    ob->shortcut = fl_calloc(1, sizeof(*(ob->shortcut)));
    ob->shortcut[0] = 0;
    ob->active = 1;
    ob->visible = FL_VISIBLE;
    ob->object_callback = 0;
    ob->spec = 0;
    ob->next = ob->prev = 0;
    ob->form = 0;
    ob->dbl_background = FL_COL1;
    return ob;
}

/* Frees the memory used by an object. */
void
fl_free_object(FL_OBJECT * obj)
{
    /* check whether ok to free it */
    if (obj == NULL)
    {
	fl_error("fl_free_object", "Trying to free NULL object.");
	return;
    }

    fl_handle_object(obj, FL_FREEMEM, 0, 0, 0, 0);

    if (obj->form != NULL)
	fl_delete_object(obj);

    /* free object */
    if (obj->label)
	fl_free(obj->label);
    if (obj->tooltip)
	fl_free(obj->tooltip);

    if (obj->shortcut)
	fl_free(obj->shortcut);

    obj->label = 0;
    obj->tooltip = 0;
    obj->shortcut = 0;

    if (obj->flpixmap)
    {
	fl_free_flpixmap(obj->flpixmap);
	fl_free(obj->flpixmap);
	obj->flpixmap = 0;
    }

    fl_addto_freelist(obj);
}


/* Adds an object to the form. */
void
fl_add_object(FL_FORM * form, FL_OBJECT * obj)
{
    /* Checking for correct behaviour. */
    if (obj == NULL)
    {
	fl_error("fl_add_object", "Trying to add NULL object.");
	return;
    }

    if (form == NULL)
    {
	M_err("addobject", "%s", fl_object_class_name(obj));
	fl_error("fl_add_object", "Trying to add object to NULL form.");
	return;
    }

    if (obj->automatic)
	form->has_auto++;

    obj->prev = obj->next = NULL;
    obj->form = form;

    if (fl_inverted_y)
	obj->y = TRANY(obj, form);

    if (fl_current_group)
	obj->group_id = fl_current_group->group_id;

    /* if adding to a group, find the end of group or the end of the object
       list on this form */
    if (fl_current_group)
    {
	FL_OBJECT *end = fl_current_group;
	for (; end && end->objclass != FL_END_GROUP; end = end->next)
	    ;
	if (end)
	{
	    /* must've opened the group with fl_addto_group */
	    end->prev->next = obj;
	    obj->prev = end->prev;
	    obj->next = end;
	    end->prev = obj;
	    fl_redraw_object(obj);
	    return;
	}
    }

    if (form->first == NULL)
	form->first = form->last = obj;
    else
    {
	obj->prev = form->last;
	form->last->next = obj;
	form->last = obj;
    }

    if (obj->input && form->focusobj == NULL)
	fl_set_focus_object(form, obj);

    if (obj->child && !obj->is_child)
	fl_add_composite(form, obj);

    fl_redraw_object(obj);
}

/* Insert object obj before object before. */
void
fl_insert_object(FL_OBJECT * obj, FL_OBJECT * before)
{
    FL_FORM *form;

    /* Checking for correct behaviour. */
    if (obj == NULL)
    {
	fl_error("fl_insert_object", "Trying to insert NULL object.");
	return;
    }

    if (before == NULL)
    {
	fl_error("fl_insert_object", "Trying to insert before NULL object.");
	return;
    }

    if (before->form == NULL)
    {
	fl_error("fl_insert_object", "Trying to insert object to NULL form.");
	return;
    }

    form = before->form;
    obj->next = before;
    obj->group_id = before->group_id;

    if (before == form->first)
    {
	form->first = obj;
	obj->prev = NULL;
    }
    else
    {
	obj->prev = before->prev;
	obj->prev->next = obj;
    }

    before->prev = obj;
    obj->form = form;
    if (obj->input && form->focusobj == NULL)
	fl_set_focus_object(form, obj);

    fl_redraw_form(form);
}

/* Deletes an object from its form. */
void
fl_delete_object(FL_OBJECT * obj)
{
    FL_FORM *form;

    if (obj == NULL)
    {
	fl_error("fl_delete_object", "Trying to delete NULL object.");
	return;
    }

    if (obj->form == NULL)
    {
	M_err("fl_delete_object", "delete %s from NULL form.", obj->label);
	return;
    }

    form = obj->form;
    if (obj->focus)
	fl_set_focus_object(form, NULL);

    if (obj == fl_pushobj)
	fl_pushobj = 0;
    if (obj == fl_mouseobj)
	fl_mouseobj = 0;

#ifdef DELAYED_ACTION
    fl_object_qflush_object(obj);
#endif

    if (obj->objclass != FL_BEGIN_GROUP && obj->objclass != FL_END_GROUP)
	obj->group_id = 0;

    obj->form = NULL;

    if (obj->prev != NULL)
	obj->prev->next = obj->next;
    else
	form->first = obj->next;

    if (obj->next != NULL)
	obj->next->prev = obj->prev;
    else
	form->last = obj->prev;

    if (form->focusobj == NULL)
	fl_set_focus_object(form, fl_find_first(form, FL_FIND_INPUT, 0, 0));

    /* if this object is a parent of a group, remove the entire group */
    if (obj->child)
	fl_delete_composite(obj);

    if (obj->visible && (form && form->visible == FL_VISIBLE ))
	fl_redraw_form(form);
}

void
fl_delete_group(FL_OBJECT * ob)
{
    if (!ob)
	return;

    if (ob->objclass != FL_BEGIN_GROUP)
    {
	fl_delete_object(ob);
	return;
    }

    for (; ob && ob->objclass != FL_END_GROUP; ob = ob->next)
    {
	fl_delete_object(ob);
    }

    if (ob)
	fl_delete_object(ob);
}

/*-----------------------------------------------------------------------
   Setting Attributes.
-----------------------------------------------------------------------*/

/* Sets the boxtype of the object */
void
fl_set_object_boxtype(FL_OBJECT * ob, int boxtype)
{
    if (ob == NULL)
    {
	fl_error("fl_set_object_boxtype", "Setting boxtype of NULL object.");
	return;
    }

    if (ob->boxtype != boxtype)
    {
	ob->boxtype = boxtype;
        if(ob->child)
            fl_handle_object(ob, FL_ATTRIB, 0, 0, 0, 0);
	fl_redraw_object(ob);
    }
}

void
fl_set_object_resize(FL_OBJECT * ob, unsigned what)
{
    if (ob == NULL)
    {
	fl_error("fl_set_object_resize", "Setting resize of NULL object.");
	return;
    }

    ob->resize = what;

    if (ob->child)
	fl_set_composite_resize(ob, what);

    /* check if this object is a group, if so, change all members */
    if (ob->objclass == FL_BEGIN_GROUP)
	for (; ob && ob->objclass != FL_END_GROUP; ob = ob->next)
	{
	    ob->resize = what;
	    fl_set_composite_resize(ob, what);
	}
}

void
fl_set_object_gravity(FL_OBJECT * ob, unsigned nw, unsigned se)
{
    if (ob == NULL)
    {
	fl_error("fl_set_object_gravity", "NULL object.");
	return;
    }

    ob->nwgravity = nw;
    ob->segravity = se;

    if (ob->child)
	fl_set_composite_gravity(ob, nw, se);

    /* check if this object is a group, if so, change all members */
    if (ob->objclass == FL_BEGIN_GROUP)
	for (; ob && ob->objclass != FL_END_GROUP; ob = ob->next)
	{
	    ob->nwgravity = nw;
	    ob->segravity = se;
	    fl_set_composite_gravity(ob, nw, se);
	}
}

/* Sets the color of the object */
void
fl_set_object_color(FL_OBJECT * ob, FL_COLOR col1, FL_COLOR col2)
{
    if (ob == NULL)
    {
	fl_error("fl_set_object_color", "Setting color of NULL object.");
	return;
    }

    if (ob->col1 != col1 || ob->col2 != col2)
    {
	ob->col1 = col1;
	ob->col2 = col2;
	fl_redraw_object(ob);
    }
}

void
fl_set_object_dblbuffer(FL_OBJECT * ob, int y)
{
    FL_COLOR bkcol;

    if (ob == NULL)
    {
	fl_error("fl_set_object_dblbuffer", "Setting color of NULL object.");
	return;
    }

    /* never bother with composite object */
    if (ob->parent == ob)
	return;

    if (ob->use_pixmap == y)
	return;

    if ((ob->use_pixmap = y) && !ob->flpixmap)
	ob->flpixmap = fl_calloc(1, sizeof(FL_pixmap));

    /* figure out the double buffer background */
    if (ob->form && ob->form->first)
    {
	bkcol = ob->form->first->col1;
	if (ob->form->first->boxtype == FL_NO_BOX && ob->form->first->next)
	    bkcol = ob->form->first->next->col1;
	ob->dbl_background = bkcol;
    }
}

/* really visible */
#define ObjIsVisible(ob)   (ob->visible && \
                            ob->form && ob->form->visible==FL_VISIBLE)

/* sets the label of an object */
void
fl_set_object_label(FL_OBJECT * ob, const char *label)
{
    if (ob == NULL)
    {
	fl_error("fl_set_object_label", "Setting label of NULL object.");
	return;
    }

    if (!label)
	label = "";

    if (strcmp(ob->label, label) == 0)
	return;


    if (LInside(ob->align))
    {
	ob->label = fl_realloc(ob->label, strlen(label) + 1);
	strcpy(ob->label, label);
	fl_redraw_object(ob);
    }
    else
    {
	int visible = ObjIsVisible(ob);
	if (visible)
	    fl_hide_object(ob);
	ob->label = fl_realloc(ob->label, strlen(label) + 1);
	strcpy(ob->label, label);
	if (visible)
	    fl_show_object(ob);
    }
}

/* sets the label color of an object */
void
fl_set_object_lcol(FL_OBJECT * ob, FL_COLOR lcol)
{
    if (ob == NULL)
    {
	fl_error("fl_set_object_lcol", "Setting label color of NULL object.");
	return;
    }

    if (ob->objclass == FL_BEGIN_GROUP)
    {
	fl_freeze_form(ob->form);
	for (; ob && ob->objclass != FL_END_GROUP; ob = ob->next)
	{
	    if (ob->lcol != lcol)
	    {
		ob->lcol = lcol;
		if (ob->objclass != FL_BEGIN_GROUP)
		    fl_redraw_object(ob);
	    }
	}
	fl_unfreeze_form(ob->form);
    }
    else if (ob->lcol != lcol)
    {
	ob->lcol = lcol;
	fl_redraw_object(ob);
    }
}

/* sets the label size of an object */
void
fl_set_object_lsize(FL_OBJECT * ob, int lsize)
{
    if (ob == NULL)
    {
	fl_error("fl_set_object_lsize", "Setting label size of NULL object.");
	return;
    }

    /* no nested groups */
    if (ob->objclass == FL_BEGIN_GROUP)
    {
	ob->lsize = lsize;
	for (ob = ob->next; ob && ob->objclass != FL_END_GROUP; ob = ob->next)
	    fl_set_object_lsize(ob, lsize);
    }
    else if (ob->lsize != lsize)
    {

	if (LInside(ob->align))
	{
	    ob->lsize = lsize;
	    fl_redraw_object(ob);
	}
	else
	{
	    int visible = ObjIsVisible(ob);
	    if (visible)
		fl_hide_object(ob);
	    ob->lsize = lsize;
	    if (visible)
		fl_show_object(ob);
	}
    }

}

/* sets the label style of an object */
void
fl_set_object_lstyle(FL_OBJECT * ob, int lstyle)
{
    if (ob == NULL)
    {
	fl_error("fl_set_object_lstyle",
		 "Setting label style of NULL object.");
	return;
    }

    if (ob->objclass == FL_BEGIN_GROUP)
    {
	ob->lstyle = lstyle;
	for (ob = ob->next; ob && ob->objclass != FL_END_GROUP; ob = ob->next)
	    fl_set_object_lstyle(ob, lstyle);
    }
    else if (ob->lstyle != lstyle)
    {

	if (LInside(ob->align))
	{
	    ob->lstyle = lstyle;
	    fl_redraw_object(ob);
	}
	else
	{
	    int visible = ObjIsVisible(ob);
	    if (visible)
		fl_hide_object(ob);
	    ob->lstyle = lstyle;
	    if (visible)
		fl_show_object(ob);
	}
    }
}

/* sets the label alignment of an object */
void
fl_set_object_lalign(FL_OBJECT * ob, int align)
{
    int visible;

    if (ob == NULL)
    {
	fl_error("fl_set_object_align",
		 "Setting label alignment of NULL object.");
	return;
    }

    if (ob->align == align)
	return;

    visible = ObjIsVisible(ob);

    if (LInside(ob->align) && LInside(align))
    {
	ob->align = align;
	fl_redraw_object(ob);
    }
    else
    {
	if (visible)
	    fl_hide_object(ob);
	ob->align = align;
	if (visible)
	    fl_show_object(ob);
    }
}

#if 0
void
fl_set_object_dragndrop(FL_OBJECT * ob, int yes)
{
    if (ob == NULL)
    {
	fl_error("fl_set_object_dnd",
		 "Setting drag'n drop of NULL object.");
	return;
    }
    ob->dnd = yes;
}

#endif


/* makes an object active if it was deactivated by fl_deactivate_object */
void
fl_activate_object(FL_OBJECT * ob)
{
    if (ob == NULL)
    {
	fl_error("fl_activate_object", "Trying to activate NULL object.");
	return;
    }

    if (ob->objclass == FL_BEGIN_GROUP)
	while (ob != NULL && ob->objclass != FL_END_GROUP)
	{
	    if (ob->active == DEACTIVATED)
		ob->active = 1;
	    if (ob->input && ob->form->focusobj == NULL)
		fl_set_focus_object(ob->form, ob);
	    if (ob->child)
		fl_activate_composite(ob);
	    ob = ob->next;
	}
    else
    {
	if (ob->active == DEACTIVATED)
	    ob->active = 1;
	if (ob->input && ob->form->focusobj == NULL)
	    fl_set_focus_object(ob->form, ob);
	if (ob->child)
	    fl_activate_composite(ob);
    }
}

/* Deactivates an object */
void
fl_deactivate_object(FL_OBJECT * ob)
{
    if (ob == NULL)
    {
	fl_error("fl_deactive_object", "Trying to deactive NULL object.");
	return;
    }

    if (ob->objclass == FL_BEGIN_GROUP)
	while (ob != NULL && ob->objclass != FL_END_GROUP)
	{
	    if (ob->active > 0)
		ob->active = DEACTIVATED;
	    if (ob == ob->form->focusobj)
		fl_set_focus_object(ob->form,
			      fl_find_first(ob->form, FL_FIND_INPUT, 0, 0));
	    if (ob->child)
		fl_deactivate_composite(ob);

	    ob = ob->next;
	}
    else
    {
	if (ob->active > 0)
	    ob->active = DEACTIVATED;
	if (ob == ob->form->focusobj)
	    fl_set_focus_object(ob->form,
			      fl_find_first(ob->form, FL_FIND_INPUT, 0, 0));
	if (ob->child)
	    fl_deactivate_composite(ob);
    }
}

/* Always makes an object visible and sets the visible flag to 1 */
void
fl_show_object(FL_OBJECT * ob)
{
    FL_OBJECT *obj = ob;

    if (ob == NULL)
    {
	fl_error("fl_show_object", "Trying to show NULL object.");
	return;
    }

    if (ob->objclass == FL_BEGIN_GROUP)
	while (ob != NULL && ob->objclass != FL_END_GROUP)
	{
	    ob->visible = 1;
	    if (ob->child)
		fl_show_composite(ob);
	    if (ob->input && ob->form->focusobj == NULL)
		fl_set_focus_object(ob->form, ob);
	    ob = ob->next;
	}
    else
    {
	ob->visible = 1;
	fl_show_composite(ob);
	if (ob->input && ob->form->focusobj == NULL)
	    fl_set_focus_object(ob->form, ob);
    }

    fl_redraw_object(obj);
}

/* makes an object invisible */
void
fl_hide_object(FL_OBJECT * ob)
{
    FL_OBJECT *obj = ob;
    FL_RECT xrect;
    Region reg;
    int doclip, has_frame = 0;

    if (ob == NULL)
    {
	fl_error("fl_hide_object", "Trying to hide NULL object.");
	return;
    }

    if (!ob->visible)
    {
	M_warn("fl_hide_object", "%s already invisible", ob->label);
	return;
    }


    if (ob->objclass == FL_BEGIN_GROUP)
    {
	reg = XCreateRegion();

	ob->visible = has_frame = 0;
	/* need to start after group leader as group has bogus size */
	for (ob = ob->next; ob && ob->objclass != FL_END_GROUP; ob = ob->next)
	{
	    if (ob->objclass == FL_CANVAS || ob->objclass == FL_GLCANVAS)
	    {
		int extra = 3;
		fl_hide_canvas(ob);
		xrect.x = ob->x - extra;
		xrect.y = ob->y - extra;
		xrect.width = ob->w + 2 * extra + 1;
		xrect.height = ob->h + 2 * extra + 1;
	    }
	    else
	    {
		fl_get_object_bbox_rect(ob, &xrect);
	    }


	    if (ob->child)
		fl_hide_composite(ob);

	    has_frame = has_frame || ob->objclass == FL_FRAME;

	    XUnionRectWithRegion(&xrect, reg, reg);

	    if (ob == ob->form->focusobj)
		lose_focus(ob);
	    ob->visible = 0;
	}
	XClipBox(reg, &xrect);
	XDestroyRegion(reg);
    }
    else if (ob->objclass == FL_CANVAS || ob->objclass == FL_GLCANVAS)
    {
	int extra = 3;
	fl_hide_canvas(ob);
	ob->visible = 0;
	xrect.x = ob->x - extra;
	xrect.y = ob->y - extra;
	xrect.width = ob->w + 2 * extra + 1;
	xrect.height = ob->h + 2 * extra + 1;
    }
    else
    {

	if (ob->child)
	    fl_hide_composite(ob);

	/* in addition to being expensive, also may cause fdesign -convert to
	   fail if not checked */
	if (ob->form->visible)
	    fl_get_object_bbox_rect(ob, &xrect);

	if (ob == ob->form->focusobj)
	    lose_focus(ob);

	ob->visible = 0;

	if (ob == fl_pushobj)
	    fl_pushobj = 0;
	if (ob == fl_mouseobj)
	    fl_mouseobj = 0;

#ifdef DELAYED_ACTION
	fl_object_qflush_object(ob);
#endif
    }

    if (!ob->form->visible)
	return;

    doclip = 1;

    /* frame is drawn OUTSIDE of the bounding box */
    if (obj->objclass == FL_FRAME || has_frame)
    {
	int extra = FL_abs(ob->bw);
	xrect.x -= extra;
	xrect.y -= extra;
	xrect.width += 2 * extra + 1;
	xrect.height += 2 * extra + 1;
    }

    if (doclip)
    {
	fl_set_perm_clipping(xrect.x, xrect.y, xrect.width, xrect.height);
	fl_set_clipping(xrect.x, xrect.y, xrect.width, xrect.height);
    }

    fl_redraw_form(obj->form);

    if (doclip)
    {
	fl_unset_perm_clipping();
	fl_unset_clipping();
	fl_unset_text_clipping();
    }
}

/* Sets the list of shortcuts for the object. Shortcuts are specified
   with a string. #x means Alt_x and ^x means Cntl_x. ^ can be
   used as an escape, i.e., ^# means #.
 */
#include <ctype.h>

#define ALT_CHAR       '#'
#define CNTL_CHAR      '^'
#define FKEY_CHAR      '&'

#define MAX_SHORTCUTS   17

int
fl_convert_shortcut(const char *str, long sc[])
{
    int i, j, offset, key;

    /* sstr might be obj->shortcut */
    i = j = offset = 0;
    while (str[i] != '\0' && j < MAX_SHORTCUTS)
    {
	if (str[i] == ALT_CHAR)
	    offset = FL_ALT_VAL;
	else if (str[i] == CNTL_CHAR)
	{
	    i++;
	    if (str[i] >= 'A' && str[i] <= 'Z')
		sc[j++] = str[i] - 'A' + 1 + offset;
	    else if (str[i] >= 'a' && str[i] <= 'z')
		sc[j++] = str[i] - 'a' + 1 + offset;
	    else if (str[i] == '[')
		sc[j++] = 27 + offset;
	    else
		sc[j++] = str[i] + offset;
	    offset = 0;
	}
	else if (str[i] == FKEY_CHAR)	/* special characters */
	{
	    i++;
	    if (str[i] == FKEY_CHAR)
		sc[j++] = FKEY_CHAR + offset;
	    else if (str[i] == 'A')
		sc[j++] = offset + XK_Up;
	    else if (str[i] == 'B')
		sc[j++] = offset + XK_Down;
	    else if (str[i] == 'C')
		sc[j++] = offset + XK_Right;
	    else if (str[i] == 'D')
		sc[j++] = offset + XK_Left;
	    else if (isdigit( ( int ) str[i]) && (key = atoi(str + i)) < 35)
	    {
		i += (key >= 10);
		sc[j++] = offset + XK_F1 + key - 1;
	    }
	    offset = 0;
	}
	else
	{
	    sc[j++] = str[i] + offset;
	    offset = 0;
	}
	i++;
    }

    if (j >= MAX_SHORTCUTS)
    {
	j = j - 1;
	M_err("ConvertShortcuts", "Too many shortcuts (>%d)", MAX_SHORTCUTS - 1);
    }

    sc[j] = 0;

    return j;
}

int
fl_get_underline_pos(const char *label, const char *sc)
{
    int c;
    const char *p;

    /* find the first non-special char in shortcut str */
    for (c = 0, p = sc; !c && *p; p++)
    {
	if (isalnum( ( int ) *p))
	{
	    if (p == sc)
		c = *p;
	    else if (*(p - 1) != FKEY_CHAR && !isdigit( ( int ) *(p - 1)))
		c = *p;
	}
    }

    if (!c)
	return -1;

    /* find where the matches occur */
    if (c == sc[0])
	p = strchr(label, c);
    else if (!(p = strchr(label, c)))
	p = strchr(label, islower(c) ? toupper(c) : tolower(c));

    if (!p)
	return -1;

    return (p - label) + 1;
}

void
fl_set_object_shortcut(FL_OBJECT * obj, const char *sstr, int showit)
{
    long sc[MAX_SHORTCUTS];	/* converted shortcuts */
    int scsize, n;

    if (obj == NULL)
    {
	fl_error("fl_set_object_shortcut", "Object is NULL.");
	return;
    }

    if (!obj->active)
    {
	M_err("fl_set_object_shortcut", "setting shortcut for inactive obj");
	return;
    }

    if (!sstr || !*sstr)
    {
	obj->shortcut[0] = 0;
	return;
    }

    n = fl_convert_shortcut(sstr, sc);
    scsize = (n + 1) * sizeof(*(obj->shortcut));
    obj->shortcut = fl_realloc(obj->shortcut, scsize);
    memcpy((char *) obj->shortcut, (char *) sc, scsize);

    if (!showit || !obj->label || !obj->label[0] || obj->label[0] == '@')
	return;

    /* find out where to underline */
    if ((n = fl_get_underline_pos(obj->label, sstr)) > 0 &&
	!strchr(obj->label, *fl_ul_magic_char))
    {
	char *q = obj->label;

	obj->label = fl_malloc(strlen(q) + 2);
	strncpy(obj->label, q, n);
	obj->label[n] = *fl_ul_magic_char;
	strcpy(obj->label + n + 1, q + n);
	fl_free(q);
    }
}

/* set a shortcut with keysyms directly */
void
fl_set_object_shortcutkey(FL_OBJECT * obj, unsigned int keysym)
{
    int n, scsize;
    long *p = obj->shortcut;

    for (n = 0; *p; n++, p++)
	;
    /* always have a terminator, thus n+2 */
    scsize = (n + 2) * sizeof(*(obj->shortcut));
    obj->shortcut = fl_realloc(obj->shortcut, scsize);
    obj->shortcut[n] = keysym;
    obj->shortcut[n + 1] = 0;
}

/* Sets the object in the form on which the input is focussed. */
void
fl_set_focus_object(FL_FORM * form, FL_OBJECT * obj)
{
    if (form == NULL)
    {
	fl_error("fl_set_focus_object", "Setting focus in NULL form.");
	return;
    }

    if (obj == form->focusobj)
	return;

    fl_handle_object_direct(form->focusobj, FL_UNFOCUS, 0, 0, 0, 0);
    fl_handle_object_direct(obj, FL_FOCUS, 0, 0, 0, 0);
}

FL_OBJECT *
fl_get_focus_object(FL_FORM * form)
{
#if 0
    return (form && form->focusobj) ? form->focusobj : 0;
#else
    if (form && form->focusobj)
    {
	if (form->focusobj->type == FL_MULTILINE_INPUT)
	    return form->focusobj->parent;
	else
	    return form->focusobj;
    }
    return 0;
#endif
}

/*-----------------------------------------------------------------------
   Searching in forms
-----------------------------------------------------------------------*/

/* returns object in form starting at obj of type find. If
 * find_object does not return object, the event that triggered
 * the call would be eaten. This is how the deactived and
 * inactive object rejects events. Modify with care!
 */
FL_OBJECT *
fl_find_object(FL_OBJECT * obj, int find, FL_Coord mx, FL_Coord my)
{
    while (obj != NULL)
    {
	if (obj->objclass != FL_BEGIN_GROUP && obj->objclass != FL_END_GROUP &&
	     (obj->visible && (!obj->is_child || obj->parent->visible) &&
	       (obj->active > 0 || (obj->posthandle && !obj->active) ||
               ((obj->tooltip && *obj->tooltip) && !obj->active))
             )
            )
	{
	    if (find == FL_FIND_INPUT && obj->input)
		return obj;

	    if (find == FL_FIND_AUTOMATIC && obj->automatic)
		return obj;

	    if (find == FL_FIND_MOUSE &&
		mx >= obj->x && mx <= (obj->x + obj->w) &&
		my >= obj->y && my <= (obj->y + obj->h))
		return obj;

	    if (find == FL_FIND_KEYSPECIAL && (obj->wantkey & FL_KEY_SPECIAL))
		return obj;
	}
	obj = obj->next;
    }
    return NULL;
}

/* returns the first object of type find */
FL_OBJECT *
fl_find_first(FL_FORM * form, int find, FL_Coord mx, FL_Coord my)
{
    return (fl_find_object(form->first, find, mx, my));
}

/* returns the last object of the type find */
FL_OBJECT *
fl_find_last(FL_FORM * form, int find, FL_Coord mx, FL_Coord my)
{
    FL_OBJECT *last, *obj;
    last = obj = fl_find_first(form, find, mx, my);
    while (obj != NULL)
    {
	last = obj;
	obj = fl_find_object(obj->next, find, mx, my);
    }
    return last;
}

/*-----------------------------------------------------------------------
   Drawing Routines.
-----------------------------------------------------------------------*/



static int
object_is_clipped(FL_OBJECT * ob)
{
    FL_RECT xr;
    int extra = 1;

    fl_get_object_bbox_rect(ob, &xr);
    xr.x -= extra;
    xr.y -= extra;
    xr.width += 2 * extra;
    xr.height += 2 * extra;
    return (fl_union_rect(&xr, &fl_perm_xcr) == 0);
}

/* Redraws all marked objects and reduces the mark. It is important
 * NOT to set any clip masks inside this function (except for free objects)
 * in that it will prevent the drawing function from drawing labels. That
 * would be wrong since fl_redraw_object() calls redraw_marked
 * directly. All clip must done prior to calling this routines
 */

static void
redraw_marked(FL_FORM * form)
{
    FL_OBJECT *ob;

    if (!form->visible || form->frozen > 0)
	return;

    fl_set_form_window(form);
    ob = form->first;

    fl_create_form_pixmap(form);

    for (; ob; ob = ob->next)
    {
	if (ob->visible && ob->redraw-- > 0 &&
	    (!ob->is_child || ob->parent->visible))
	{
	    /* no point redrawing unexposed object */

	    if (fl_perm_clip && object_is_clipped(ob))
	    {
#if (FL_DEBUG >= ML_WARN)
		M_warn("Redraw", "%s is clipped", ob->label);
#endif
		continue;
	    }

	    fl_create_object_pixmap(ob);

	    /* will not allow free object draw outside of its box. Check
	       perm_clip so we don't have draw regions we don't have to
	       (Expose etc.) */

	    if ((ob->objclass == FL_FREE || ob->clip) && !fl_perm_clip)
	    {
		fl_set_clipping(ob->x, ob->y, ob->w, ob->h);
		fl_set_text_clipping(ob->x, ob->y, ob->w, ob->h);
	    }

	    fl_handle_object(ob, FL_DRAW, 0, 0, 0, 0);

	    if ((ob->objclass == FL_FREE || ob->clip) && !fl_perm_clip)
	    {
		fl_unset_clipping();
		fl_unset_text_clipping();
	    }

	    fl_show_object_pixmap(ob);
	}
    }

    fl_show_form_pixmap(form);
}

/* The actual drawing routine seen by the user */
void
fl_redraw_object(FL_OBJECT * obj)
{
    FL_OBJECT *ob;

    if (obj == NULL)
    {
	fl_error("fl_redraw_object", "Trying to draw NULL object.");
	return;
    }

    if (obj->form == NULL)
	return;

    if (obj->objclass == FL_BEGIN_GROUP)
    {
	ob = obj;
	while ((ob = ob->next) != NULL && ob->objclass != FL_END_GROUP)
	    ob->redraw = 1;
    }
    else
	obj->redraw = 1;

    /* if composite object, flag all children */
    if (obj->child && obj->parent->visible && obj->visible)
	fl_mark_composite_for_redraw(obj);

    /* if obj is a child object and the parent is not visible, do nothing */
    if (obj->visible && (!obj->is_child || obj->parent->visible))
	redraw_marked(obj->form);

}

/* Draws a form */
void
fl_redraw_form(FL_FORM * form)
{
    FL_OBJECT *ob;

    if (form == NULL)
    {
	fl_error("fl_redraw_form", "Drawing NULL form.");
	return;
    }

    for (ob = form->first; ob; ob = ob->next)
	ob->redraw = 1;

    redraw_marked(form);
}

/* Disables drawing of form */
void
fl_freeze_form(FL_FORM * form)
{
    if (form == NULL)
    {
	fl_error("fl_freeze_form", "Freezing NULL form.");
	return;
    }
    form->frozen++;
}

/* Enable drawing of form */
void
fl_unfreeze_form(FL_FORM * form)
{
    if (form == NULL)
    {
	fl_error("fl_unfreeze_form", "Unfreezing NULL form.");
	return;
    }

    if (form->frozen == 0)
    {
	M_err("fl_unfreeze_form", "Unfreezing non-frozen form.");
	return;
    }

    form->frozen--;

    if (form->frozen == 0)
	redraw_marked(form);
}

/*-----------------------------------------------------------------------
   Handling Routines.
-----------------------------------------------------------------------*/

static FL_OBJECT *refocus;

/* should only be used as a response to FL_UNFOCOS */
void
fl_reset_focus_object(FL_OBJECT * ob)
{
    refocus = ob;
}

/* handle tooltips */
static void
tooltip_handler(int ID, void *data)
{
    FL_OBJECT *obj = data;

    if (obj->tooltip && *(obj->tooltip))
	fl_show_tooltip(obj->tooltip, obj->form->x + obj->x,
			obj->form->y + obj->y + obj->h + 1);
    obj->tipID = 0;
}

/* handles an event for an object */
static int
fl_handle_it(FL_OBJECT * obj, int event, FL_Coord mx, FL_Coord my, int key,
	     XEvent * xev)
{
    static unsigned long last_clicktime;
    static int last_dblclick, last_key;
    static FL_Coord lmx, lmy;
    int status = 0, moved;
    int cur_event;

    if (obj == NULL)
	return 0;

#if (FL_DEBUG >= ML_WARN)
    if (obj->form == 0 && event != FL_FREEMEM)
    {
	M_err("fl_handle_it", "Bad object %s. Event=%s", 
              obj->label ? obj->label : "", fl_event_name(event));
	return 0;
    }
#endif

    if (obj->objclass == FL_BEGIN_GROUP || obj->objclass == FL_END_GROUP)
	return 0;

    if (obj->handle == NULL)
	return 0;

    switch (event)
    {
    case FL_ENTER:
	if (obj->tooltip && *(obj->tooltip) && !obj->form->no_tooltip)
	    obj->tipID = fl_add_timeout(fl_context->tooltip_time,
					tooltip_handler, obj);
	obj->belowmouse = 1;
	break;
    case FL_LEAVE:
	if (obj->tooltip && *(obj->tooltip))
	{
	    fl_hide_tooltip();
	    if (obj->tipID)
		fl_remove_timeout(obj->tipID);
	    obj->tipID = 0;
	}
	obj->belowmouse = 0;
	break;
    case FL_PUSH:
	if (obj->tooltip && *(obj->tooltip))
	{
	    fl_hide_tooltip();
	    if (obj->tipID)
		fl_remove_timeout(obj->tipID);
	    obj->tipID = 0;
	}
	obj->pushed = 1;
	break;
    case FL_RELEASE:
	if (!obj->radio)
	    obj->pushed = 0;
	moved = FL_abs(lmx - mx) > 4 || FL_abs(lmy - my) > 4;
	if (key != 2 && key == last_key && xev && !moved &&
	    (xev->xbutton.time - last_clicktime) < obj->click_timeout)
	    event = (last_dblclick ? FL_TRPLCLICK : FL_DBLCLICK);

	last_dblclick = event == FL_DBLCLICK;
	last_clicktime = xev ? xev->xbutton.time : 0;
	last_key = key;
	lmx = mx;
	lmy = my;
	break;
    case FL_FOCUS:
	if (refocus && refocus->form)  /* ?? from Lyx patch */
	{
	    obj = refocus;
	    refocus = 0;
	}
        if(obj->form)
        {
	   obj->form->focusobj = obj;
	   obj->focus = 1;
        }
	break;
    case FL_UNFOCUS:
	obj->form->focusobj = NULL;
	obj->focus = 0;
	break;
    case FL_DRAW:
	if (obj->objclass == FL_FREE)
	{
	    fl_set_clipping(obj->x, obj->y, obj->w, obj->h);
	    fl_set_text_clipping(obj->x, obj->y, obj->w, obj->h);
	}
	break;
    }

    cur_event = event;
    if (event == FL_DBLCLICK || event == FL_TRPLCLICK)
	event = FL_RELEASE;

  recover:
    if (obj->prehandle)
    {
	int st = obj->prehandle(obj, event, mx, my, key, xev);
	if (st == FL_PREEMPT)
	    return 0;
    }

    status = obj->handle(obj, event, mx, my, key, xev);

    if (obj->posthandle)
    {
	obj->posthandle(obj, event, mx, my, key, xev);
    }

    if (cur_event == FL_DBLCLICK || cur_event == FL_TRPLCLICK)
    {
	event = cur_event;
	cur_event = 0;
	if (status)
	    fl_object_qenter(obj);
	goto recover;
    }

    if (obj->objclass == FL_FREE && event == FL_DRAW)
    {
	fl_unset_clipping();
	fl_unset_text_clipping();
    }

    return (event == FL_DBLCLICK || event == FL_TRPLCLICK) ? 0 : status;
}

/* handle and store if successfull */
void
fl_handle_object(FL_OBJECT * obj, int event, FL_Coord mx, FL_Coord my, int key,
		 XEvent * xev)
{
    if (fl_handle_it(obj, event, mx, my, key, xev))
	fl_object_qenter(obj);
}

/* handle but returns whether successfull */
int
fl_handle_object_direct(FL_OBJECT * obj, int event,
			FL_Coord mx, FL_Coord my, int key, XEvent * xev)
{
    return fl_handle_it(obj, event, mx, my, key, xev);
}

/* Sets the call_back routine for the object */
FL_CALLBACKPTR
fl_set_object_callback(FL_OBJECT * obj, FL_CALLBACKPTR callback, long argument)
{
    FL_CALLBACKPTR old;

    if (obj == NULL)
    {
	fl_error("SetObjectCallback", "Setting callback of NULL object.");
	return 0;
    }
    old = obj->object_callback;
    obj->object_callback = callback;
    obj->argument = argument;
    return old;
}

void
fl_set_object_bw(FL_OBJECT * ob, int bw)
{
    if (FL_abs(bw) > 10)
	return;

    if (bw == 0)
	bw = -1;

    if (!ob)
    {
	fl_error("fl_set_object_bw", "Trying to set NULL object.");
	return;
    }

    /* check if this object is a group, if so, change all members */
    if (ob->objclass == FL_BEGIN_GROUP)
    {
	fl_freeze_form(ob->form);
	for (; ob && ob->objclass != FL_END_GROUP; ob = ob->next)
	{
	    if (ob->bw != bw)
	    {
		ob->bw = bw;
		if (ob->objclass != FL_BEGIN_GROUP)
		    fl_redraw_object(ob);
	    }
	}
	fl_unfreeze_form(ob->form);
    }
    else if (ob->bw != bw)
    {
	ob->bw = bw;
	fl_redraw_object(ob);
    }
}

Window
fl_get_real_object_window(FL_OBJECT * ob)
{
    FL_pixmap *objp = ob->flpixmap;
    FL_pixmap *formp = ob->form->flpixmap;
    Window win = 0;

    if (objp && objp->win)
	win = objp->win;
    else if (ob->objclass == FL_CANVAS && ob->objclass == FL_GLCANVAS &&
	     fl_get_canvas_id(ob))
	win = fl_get_canvas_id(ob);
    else if (formp && formp->win)
	win = formp->win;
    else
	win = ob->form->window;

    return win;
}



#define MAX_RECT_BUF  5

const FL_RECT *
fl_union_rect(const FL_RECT * r1, const FL_RECT * r2)
{
    static FL_RECT over[MAX_RECT_BUF];
    static int nb;
    register FL_RECT *p = over + (nb++ % MAX_RECT_BUF);
    register int xi, yi, xf, yf;
    int w, h;

    xi = p->x = FL_max(r1->x, r2->x);
    yi = p->y = FL_max(r1->y, r2->y);
    xf = FL_min(r1->x + r1->width, r2->x + r2->width) - 1;
    yf = FL_min(r1->y + r1->height, r2->y + r2->height) - 1;

    p->width = w = xf - xi + 1;
    p->height = h = yf - yi + 1;

    return (w > 0 && h > 0) ? p : 0;
}

const FL_RECT *
fl_bounding_rect(const FL_RECT * r1, const FL_RECT * r2)
{
    static FL_RECT rect;
    int xi, yi, xf, yf;

    xi = rect.x = FL_min(r1->x, r2->x);
    yi = rect.y = FL_min(r1->y, r2->y);
    xf = FL_max(r1->x + r1->width, r2->x + r2->width) + 1;
    yf = FL_max(r1->y + r1->height, r2->y + r2->height) + 1;

    rect.width = xf - xi + 1;
    rect.height = yf - yi + 1;

    return &rect;
}


void
fl_scale_length(FL_Coord * x, FL_Coord * w, double s)
{
    int xi, xf;

    xi = *x * s + DELTA;
    xf = (*x + *w) * s + DELTA;
    *x = xi;
    *w = xf - xi;
}

/* Scale an object. For internal use. The calculation takes care of
 * round-off errors and has the property that if two objects
 * are "glued" before scaling, they will remain so.
 */
void
fl_scale_object(FL_OBJECT * ob, double xs, double ys)
{
    int xi, yi, xf, yf;

    if (xs == 1.0 && ys == 1.0)
	return;

    xi = ob->x * xs + DELTA;
    xf = (ob->x + ob->w) * xs + DELTA;
    ob->w = xf - xi;
    ob->x = xi;

    yi = ob->y * ys + DELTA;
    yf = (ob->y + ob->h) * ys + DELTA;
    ob->h = yf - yi;
    ob->y = yi;
}

/* register a preemptive object handler */

FL_HANDLEPTR
fl_set_object_prehandler(FL_OBJECT * ob, FL_HANDLEPTR phandler)
{
    FL_HANDLEPTR oldh = ob->prehandle;
    ob->prehandle = phandler;
    return oldh;
}

FL_HANDLEPTR
fl_set_object_posthandler(FL_OBJECT * ob, FL_HANDLEPTR post)
{
    FL_HANDLEPTR oldh = ob->posthandle;
    ob->posthandle = post;
    return oldh;
}

void
fl_trigger_object(FL_OBJECT * obj)
{
    if (obj && obj != FL_EVENT && obj->form && obj->visible && obj->active > 0)
	fl_object_qenter(obj);
}

void
fl_set_object_clip(FL_OBJECT * obj, int yes_no)
{
    if (obj->clip != yes_no)
    {
	obj->clip = yes_no;
	if (yes_no == 1)
	{
	    fl_hide_object(obj);
	    fl_show_object(obj);
	}
	else
	    fl_redraw_object(obj);
    }
}

void
fl_draw_object_label(FL_OBJECT * ob)
{
    int align = ob->align % FL_ALIGN_INSIDE;

    (align != ob->align ? fl_drw_text : fl_drw_text_beside)
	(align, ob->x, ob->y, ob->w, ob->h,
	 ob->lcol, ob->lstyle, ob->lsize, ob->label);
}

void
fl_draw_object_label_outside(FL_OBJECT * ob)
{
    fl_drw_text_beside(ob->align & ~FL_ALIGN_INSIDE, ob->x, ob->y, ob->w,
		       ob->h, ob->lcol, ob->lstyle, ob->lsize, ob->label);
}

void
fl_call_object_callback(FL_OBJECT * ob)
{
    if (ob && ob->object_callback)
	ob->object_callback(ob, ob->argument);
}

void
fl_set_object_position(FL_OBJECT * obj, FL_Coord x, FL_Coord y)
{
    int visible = obj->visible;

    if (obj->x == x && obj->y == y)
	return;

    if (visible)
	fl_hide_object(obj);

    obj->x = x;
    obj->y = y;

    if (fl_inverted_y)
	obj->y = TRANY(obj, obj->form);

    if (visible)
	fl_show_object(obj);
}

void
fl_move_object(FL_OBJECT *obj, FL_Coord dx, FL_Coord dy)
{
     FL_Coord x, y;
     FL_OBJECT *ob;

    if (obj->objclass == FL_BEGIN_GROUP)
    {
        fl_freeze_form(obj->form);
        for ( ob = obj->next;  ob->objclass != FL_END_GROUP; ob=ob->next)
        {
              fl_get_object_position(ob, &x, &y);
              fl_set_object_position(ob, x+dx, y+dy);
        }
        fl_unfreeze_form(obj->form);
    }
    else
    {
        fl_get_object_position(obj, &x, &y);
        fl_set_object_position(obj, x+dx, y+dy);
    }
}

void
fl_get_object_position(FL_OBJECT * ob, FL_Coord * x, FL_Coord * y)
{
    *x = ob->x;
    *y = fl_inverted_y ? TRANY(ob, ob->form) : ob->y;
}

void
fl_get_object_geometry(FL_OBJECT * ob, FL_Coord * x, FL_Coord * y,
		       FL_Coord * w, FL_Coord * h)
{
    fl_get_object_position(ob, x, y);
    *w = ob->w;
    *h = ob->h;
 /*    fl_redraw_object(ob); */
}

void
fl_set_object_size(FL_OBJECT * obj, FL_Coord w, FL_Coord h)
{
    int visible = obj->visible;

    if (obj->w == w && obj->h == h)
	return;

    if (visible)
	fl_hide_object(obj);

    obj->w = w;
    obj->h = h;
    fl_handle_object(obj, FL_ATTRIB, 0, 0, 0, 0);

    if (visible)
	fl_show_object(obj);
}

void
fl_set_object_geometry(FL_OBJECT * obj, FL_Coord x, FL_Coord y,
		       FL_Coord w, FL_Coord h)
{
    int visible = obj->visible;

    if (visible)
	fl_hide_object(obj);

    fl_set_object_position(obj, x, y);
    fl_set_object_size(obj, w, h);

    if (visible)
	fl_show_object(obj);
}

/* compute object geometry also takes into account the label */
void
fl_get_object_bbox(FL_OBJECT * obj, FL_Coord * x, FL_Coord * y,
		   FL_Coord * w, FL_Coord * h)
{
    char *label = obj->label;
    FL_OBJECT *tmp;
    int extra = 1;
    XRectangle lrect, orect;
    const XRectangle *xr;

    if (obj->objclass == FL_FRAME || obj->objclass == FL_LABELFRAME)
	extra += FL_abs(obj->bw);

    if (obj->objclass >= FL_USER_CLASS_START &&
	obj->objclass <= FL_USER_CLASS_END)
	extra = FL_abs(obj->bw) + obj->lsize;

    lrect.x = orect.x = obj->x - extra;
    lrect.y = orect.y = obj->y - extra;
    lrect.width = orect.width = obj->w + 2 * extra;
    lrect.height = orect.height = obj->h + 2 * extra;

    /* label position */
    if (label && *label)
    {
	int len = strlen(label);
	int sw, sh;
	int xx, yy, a, d;

	fl_get_string_dimension(obj->lstyle, obj->lsize, label, len, &sw, &sh);
	fl_get_char_height(obj->lstyle, obj->lsize, &a, &d);
	fl_get_align_xy(obj->align, obj->x, obj->y, obj->w, obj->h,
			sw, sh + d, 3, 3, &xx, &yy);
	lrect.x = xx - 1;
	lrect.y = yy - 1;
	lrect.width = sw + 2;
	lrect.height = sh + d + 2 + a;
    }

    xr = fl_bounding_rect(&lrect, &orect);


    for (tmp = obj->child; tmp; tmp = tmp->nc)
    {
	lrect = *xr;
	orect.x = tmp->x;
	orect.y = tmp->y;
	orect.width = tmp->w;
	orect.height = tmp->h;
	xr = fl_bounding_rect(&lrect, &orect);
    }

    *x = xr->x;
    *y = xr->y;
    *w = xr->width;
    *h = xr->height;
}

void
fl_get_object_bbox_rect(FL_OBJECT * obj, FL_RECT * xr)
{
    int x, y, w, h;
    fl_get_object_bbox(obj, &x, &y, &w, &h);
    xr->x = x;
    xr->y = y;
    xr->height = h;
    xr->width = w;
}

void
fl_set_object_automatic(FL_OBJECT * ob, int flag)
{
    if (ob->automatic != flag)
    {
	ob->automatic = flag;
	if (flag)
	{
	    if (ob->form)
		ob->form->has_auto++;
	}
	else
	{
	    if (ob->form)
		ob->form->has_auto--;
	}
	fl_recount_auto_object();
    }
}

#include "pcanvas.h"
void
fl_hide_canvas(FL_OBJECT * ob)
{
    FL_CANVAS_SPEC *sp = ob->spec;

    if (sp->window && sp->cleanup)
	sp->cleanup(ob);

    /* if parent is unmapped, sp->window is also unmapped */
    if (ob->visible && sp->window && ob->form && ob->form->window)
    {
	/* must cleanup canvas specific stuff before closing window */
	fl_winclose(sp->window);
    }

    sp->window = 0;
}

Window
fl_get_canvas_id(FL_OBJECT * ob)
{
    FL_CANVAS_SPEC *sp = ob->spec;

#if (FL_DEBUG >= ML_DEBUG)
    if (!IsValidCanvas(ob))
    {
	M_err("CanvasID", "%s not a canvas", ob ? ob->label : "");
	return 0;
    }
#endif

    return sp->window ? sp->window : 0;
}

static void
lose_focus(FL_OBJECT * ob)
{
    if (fl_handle_object_direct(ob, FL_UNFOCUS, 0, 0, 0, 0))
    {
	if (ob->object_callback)
	    ob->object_callback(ob, ob->argument);
    }
}

void
fl_for_all_objects(FL_FORM * form, int (*cb) (FL_OBJECT *, void *), void *v)
{
    FL_OBJECT *ob;
    int ret;

    if (!form)
	return;

    for (ret = 0, ob = form->first; ob && !ret; ob = ob->next)
	ret = cb(ob, v);
}

const char *
fl_get_object_label(FL_OBJECT * ob)
{
    return ob ? ob->label : 0;
}

void
fl_set_object_helper(FL_OBJECT * ob, const char *tip)
{
    char *s = ob->tooltip;

    ob->tooltip = tip ? fl_strdup(tip) : 0;

    if(s)
      free(s);
}
