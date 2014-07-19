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
 * $Id: fd_select.c,v 1.0 2002/05/08 05:27:39 zhao Release $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 * Part of the Form Designer.
 *
 * This file contains all routines and data types to maintain the current
 * selection and manipulate (move, scale) and draw it.
 *
 */


#include "forms.h"
#include "fd_main.h"

#define MAXSEL	512

#define BackOBJ()    cur_form->first->next

static FL_OBJECT *selobj[MAXSEL];	/* The selected objects */
static int selnumb = 0;		/* Their number */
int backf = FALSE;		/* Whether the selection is the backface */

static FL_OBJECT *tmpobj[MAXSEL];	/* A temporary list of objects */
static int tmpnumb = 0;		/* Their number */

/* Returns the index of a particular object. -1 if it is not selected */
static int
find_selobject(FL_OBJECT * obj)
{
    int i;
    for (i = 0; i < selnumb; i++)
	if (selobj[i] == obj)
	    return i;
    return -1;
}

/* Cleans up the selection, ordering the objects and creating
   groups if all elements are in there */
static void
cleanup_selection(void)
{
    FL_OBJECT *obj, *begobj = 0;
    int i, tt, sel = -1;

    if (cur_form == NULL)
    {
	selnumb = 0;
	backf = FALSE;
	return;
    }

    /* Figure out whether whole groups are selected */
    obj = cur_form->first;
    while (obj != NULL)
    {
	if (obj->objclass == FL_BEGIN_GROUP)
	{
	    sel = 1;
	    begobj = obj;
	}
	else if (obj->objclass == FL_END_GROUP)
	{
	    if (sel)
	    {
		selobj[selnumb++] = begobj;
		selobj[selnumb++] = obj;
	    }
	    else
	    {
		if ((tt = find_selobject(begobj)) != -1)
		    selobj[tt] = NULL;
		if ((tt = find_selobject(obj)) != -1)
		    selobj[tt] = NULL;
	    }
	}
	else
	{
	    if (find_selobject(obj) == -1)
		sel = 0;
	}
	obj = obj->next;
    }

    /* make a new, ordered list of selected items, removing duplicates and
       NULL objects */
    obj = cur_form->first;
    tmpnumb = 0;
    for (; obj != NULL; obj = obj->next)
    {
	if ((tt = find_selobject(obj)) != -1)
	    tmpobj[tmpnumb++] = obj;
    }

    for (i = 0; i < tmpnumb; i++)
	selobj[i] = tmpobj[i];

    selnumb = tmpnumb;
    fillin_groups();
}

/* Returns whether object is selected */
int
is_selected(FL_OBJECT * obj)
{
    return (find_selobject(obj) != -1);
}

/* Adds an object to the current selection */
void
addto_selection(FL_OBJECT * obj)
{
    if (backf)
    {
	M_warn("", "ignoring backf");
	return;			/* Don't add objects with backface */
    }

    if (selnumb == MAXSEL)
    {
	fprintf(stderr, "Exceeding selection limits\n");
	return;
    }

    /* find the real parent */
    while (obj->is_child)
	obj = obj->parent;

    selobj[selnumb++] = obj;
    cleanup_selection();
}

/* Adds a group to the current selection */
void
addgroupto_selection(FL_OBJECT * obj)
{
    FL_OBJECT *ob;

    if (backf)
	return;			/* Don't add objects with backface */

    if (obj->objclass != FL_BEGIN_GROUP)
	return;

    for (ob = obj; ob && ob->objclass != FL_END_GROUP; ob = ob->next)
    {
	if (selnumb == MAXSEL)
	    return;
	selobj[selnumb++] = ob;
    }

    if (ob != NULL)
	selobj[selnumb++] = ob;

    cleanup_selection();
}

/* Removes an object from the selection (if present) */
void
deletefrom_selection(FL_OBJECT * obj)
{
    int ind = find_selobject(obj);
    if (ind != -1)
	selobj[ind] = NULL;
    cleanup_selection();
}

/* Removes a group to the current selection */
void
deletegroupfrom_selection(FL_OBJECT * obj)
{
    FL_OBJECT *ob;
    int ind;

    if (backf)
	return;			/* Don't remove objects with backface */

    if (obj->objclass != FL_BEGIN_GROUP)
	return;

    ob = obj;
    while (ob != NULL && ob->objclass != FL_END_GROUP)
    {
	ind = find_selobject(ob);
	if (ind != -1)
	    selobj[ind] = NULL;
	ob = ob->next;
    }

    cleanup_selection();
}

/* Clears the complete selection */
void
clear_selection(void)
{
    selnumb = 0;
    backf = FALSE;
    cleanup_selection();
}

/****
  Help procedures
****/

/* Compute the bounding box of the selection */
static void
compute_selbox(float *x, float *y, float *w, float *h)
{
    int i;
    float x1 = 8000, y1 = 8000, x2 = 0, y2 = 0;

    for (i = 0; i < selnumb; i++)
    {
	if (selobj[i]->objclass != FL_BEGIN_GROUP &&
	    selobj[i]->objclass != FL_END_GROUP)
	{
	    if (selobj[i]->x < x1)
		x1 = selobj[i]->x;
	    if (selobj[i]->y < y1)
		y1 = selobj[i]->y;
	    if (selobj[i]->x + selobj[i]->w > x2)
		x2 = selobj[i]->x + selobj[i]->w;
	    if (selobj[i]->y + selobj[i]->h > y2)
		y2 = selobj[i]->y + selobj[i]->h;
	}
    }
    *x = x1;
    *y = y1;
    *w = x2 - x1;
    *h = y2 - y1;
}

/* Find position of the mouse mouse */
static void
find_mousepos(float *mx, float *my)
{
    if (cur_form == NULL)
	return;
    fl_winset(main_window);
    get_mouse_pos(mx, my);
}

/* Returns the object under the mouse. */
static FL_OBJECT *
find_mouseobj(void)
{
    float xx, yy;
    if (cur_form == NULL)
	return NULL;
    find_mousepos(&xx, &yy);
    return fl_find_last(cur_form, FL_FIND_MOUSE, xx, yy);
}

/****
  Drawing routines
****/

#define HS	8

int hidden = FALSE;

/* Draw the selection box */
void
draw_selbox(void)
{
    float x, y, w, h;
    int i;
    FL_OBJECT *ob;

    if (selnumb == 0)
	return;

    /* draw object boxes */
    color(fd_red);
    for (i = 0; i < selnumb; i++)
    {
	ob = selobj[i];
	if (ob->objclass != FL_BEGIN_GROUP && ob->objclass != FL_END_GROUP)
	    rect(ob->x, ob->y, ob->x + ob->w - 1.0, ob->y + ob->h - 1.0);
    }

    if (hidden)
	return;

    /* draw the total box */
    compute_selbox(&x, &y, &w, &h);

    show_geometry("", (int) x, (int) y, (int) w, (int) h);

    if (!backf)
    {
	x -= 1.0;
	y -= 1.0, w += 2.0;
	h += 2.0;
    }
    color(fd_red);

    rect(x, y, x + w - 1.0, y + h - 1.0);
    rectf(x, y, x + HS - 1.0, y + HS - 1.0);
    rectf(x + w - HS, y, x + w - 1.0, y + HS - 1.0);
    rectf(x + w - HS, y + h - HS, x + w - 1.0, y + h - 1.0);
    rectf(x, y + h - HS, x + HS - 1.0, y + h - 1.0);

    show_selmessage(selobj, selnumb);
}

/****
  Interaction handling
****/

#if 0
/*
 * since objects may integer coordinates, scaling can introduce errors
 * that result in "gaps" between otherwise "glued together" objects.
 *
 * This function takes care of the precision.
 */


#define DELTA 0.2
static void
fl_scale_object(FL_OBJECT * ob, float xs, float ys)
{
    int xi, yi, xf, yf;

    xi = ob->x * xs + DELTA;
    xf = (ob->x + ob->w) * xs + DELTA;
    ob->w = xf - xi;
    ob->x = xi;

    yi = ob->y * ys + DELTA;
    yf = (ob->y + ob->h) * ys + DELTA;
    ob->h = yf - yi;
    ob->y = yi;
}

#endif


/* Handles the moving of the selection (by mouse) */

int
within_selection(float mx, float my)
{
    float x, y, w = 0, h = 0;

    if (!selnumb || !cur_form || !cur_form->first)
	return 0;

    compute_selbox(&x, &y, &w, &h);

    /* if backface, only within scale knob is considered within */
    if (selobj[selnumb - 1] == BackOBJ())
	return (mx >= x + w - HS && mx < x + w &&
		my >= y + h - HS && my < y + h);
    else
	return (mx > x && mx < x + w && my > y && my < y + h);
}

void
handle_move(const XEvent * xev)
{
    float x, y, w, h;
    float mx, my;
    float ox, oy, ow, oh;
    FL_Coord xx, yy;
    int i, s;

    if (cur_form == NULL)
	return;

    fl_winset(main_window);

    s = ShiftIsDown(xev->xbutton.state);

    find_mousepos(&mx, &my);
    compute_selbox(&x, &y, &w, &h);

    if (mx < x || mx > x + w || my < y || my > y + h)
	return;			/* Not in box */

    hidden = TRUE;
    redraw_the_form(0);

    if (backf)
    {
	if (mx >= x + w - HS && my >= y + h - HS)
	{
	    fl_get_winsize(main_window, &xx, &yy);
	    set_bounding_box(0.0f, 0.0f, (float) xx, (float) yy);
	    scale_box(&x, &y, &w, &h);
	    cur_form->w = w;
	    cur_form->h = h;
	    selobj[0]->w = w;
	    selobj[0]->h = h;
	    set_bounding_box(0.0f, 0.0f, cur_form->w, cur_form->h);
	    fl_winresize(main_window, cur_form->w, cur_form->h);
	}
    }
    else if (s)
    {
	copy_selection();
	paste_selection();
    }
    else
    {
	ox = x;
	oy = y;
	ow = w;
	oh = h;

	/* show the rubberband box */
	if (mx <= x + HS && my <= y + HS)
	{
	    x += w;
	    y += h;
	    w = -w;
	    h = -h;
	    scale_box(&x, &y, &w, &h);
	}
	else if (mx <= x + HS && my >= y + h - HS)
	{
	    x += w;
	    w = -w;
	    scale_box(&x, &y, &w, &h);
	}
	else if (mx >= x + w - HS && my <= y + HS)
	{
	    y += h;
	    h = -h;
	    scale_box(&x, &y, &w, &h);
	}
	else if (mx >= x + w - HS && my >= y + h - HS)
	{
	    scale_box(&x, &y, &w, &h);
	}
	else
	    move_box(&x, &y, &w, &h, TRUE);

	/* recompute object sizes */
	for (i = 0; i < selnumb; i++)
	{
	    if (selobj[i]->objclass != FL_BEGIN_GROUP &&
		selobj[i]->objclass != FL_END_GROUP)
	    {
		selobj[i]->x -= ox;
		selobj[i]->y -= oy;
		fl_scale_object(selobj[i], w / ow, h / oh);
		selobj[i]->x += x;
		selobj[i]->y += y;
	    }
	}
    }

    hidden = FALSE;
    redraw_the_form(backf);
    changed = TRUE;
}

/* We know how many pixles to move */
void
move_selection(FL_Coord dx, FL_Coord dy)
{
    int i;
    float x, y, w, h;
    float ox, oy;
    FL_OBJECT *ob;

    if (cur_form == NULL || backf || selnumb == 0)
	return;

    compute_selbox(&x, &y, &w, &h);

    ox = x;
    oy = y;

    if ((x += dx) < 0)
	x = 0;
    else if (x > (winw - w))
	x = winw - w;

    if ((y += dy) < 0)
	y = 0;
    else if (y > (winh - h))
	y = winh - h;

    if ((dx = x - ox) == 0 && (dy = y - oy) == 0)
	return;

    for (i = 0; i < selnumb; i++)
    {
	ob = selobj[i];
	if (ob->objclass != FL_BEGIN_GROUP && ob->objclass != FL_END_GROUP)
	{
	    ob->x += dx;
	    ob->y += dy;
	}
    }
    redraw_the_form(1);
    changed = TRUE;
}

/* change the selection  size */
#define MINSIZE 5
#define DELTA 0.2

void
resize_selection(FL_Coord dx, FL_Coord dy)
{
    float x, y, w, h, ox, oy, ow, oh;
    float yscale, xscale;
    int i;

    if (!cur_form || selnumb == 0)
	return;

    compute_selbox(&x, &y, &w, &h);

    ox = x;
    oy = y;
    ow = w;
    oh = h;

    if (backf)
    {
	winw = fl_scrw;
	winh = fl_scrh;
    }

    if ((w += dx) > winw)
	w = winw;
    else if (w < MINSIZE)
	w = MINSIZE;

    if ((h += dy) > winh)
	h = winh;
    else if (h < MINSIZE)
	h = MINSIZE;

    if (w == ow && oh == h)
	return;

    xscale = (float) w / ow;
    yscale = (float) h / oh;

    /* recompute object sizes */
    for (i = 0; i < selnumb; i++)
    {
	if (selobj[i]->objclass != FL_BEGIN_GROUP &&
	    selobj[i]->objclass != FL_END_GROUP)
	{
	    selobj[i]->x -= ox;
	    selobj[i]->y -= oy;
	    fl_scale_object(selobj[i], xscale, yscale);
	    selobj[i]->x += x;
	    selobj[i]->y += y;
	}
    }

    if (backf)
    {
	cur_form->w = selobj[0]->w;
	cur_form->h = selobj[0]->h;
	fl_winresize(main_window, cur_form->w, cur_form->h);
    }
    redraw_the_form(1);
    changed = TRUE;

}

/* handles the selection of objects */
void
handle_select(const XEvent * xev)
{
    int s;
    FL_OBJECT *obj, *mouseobj;
    float x, y, w, h, mx, my;
    float stepsize;

    if (cur_form == NULL)
	return;

    s = ShiftIsDown(xev->xbutton.state);

    mouseobj = find_mouseobj();
    find_mousepos(&mx, &my);

    if (s)			/* Shift Push */
    {
	if (!cur_form->first)
	    fprintf(stderr, "something is wrong\n");
	if (mouseobj == NULL || mouseobj == BackOBJ())
	    return;

	if (find_selobject(mouseobj) == -1)
	    addto_selection(mouseobj);
	else
	    deletefrom_selection(mouseobj);
    }
    else
    {
	clear_selection();
	x = mx;
	y = my;
	w = 0.0;
	h = 0.0;
	stepsize = get_step_size();
	set_step_size(0.0f);
	if (xev->type != ButtonRelease)
	    scale_box(&x, &y, &w, &h);
	set_step_size(stepsize);
	obj = BackOBJ()->next;

	while (obj != NULL)
	{
	    if (obj->objclass != FL_BEGIN_GROUP && obj->objclass != FL_END_GROUP &&
		obj->x >= x && obj->y >= y && obj->x + obj->w <= x + w &&
		obj->y + obj->h <= y + h)
	    {
		addto_selection(obj);
	    }
	    obj = obj->next;
	}

	if (selnumb == 0)
	{
	    if (mouseobj == NULL)
		return;
	    else if (mouseobj == BackOBJ())
	    {
		addto_selection(mouseobj);
		backf = TRUE;
	    }
	    else
		addto_selection(mouseobj);
	}
    }
}

/* Selects all objects in the form. */
void
select_all(void)
{
    FL_OBJECT *obj;

    if (cur_form == NULL)
	return;

    clear_selection();
    obj = BackOBJ()->next;

    while (obj != NULL)
    {
	if (!obj->is_child)
	    selobj[selnumb++] = obj;
	obj = obj->next;
    }

    cleanup_selection();
}

/****
  Operations on the selection
****/

/* clone curobj's attributes to the currently selected objects */
void
change_selected_objects(FL_OBJECT * curobj)
{
    int i;
    FL_OBJECT *ob;

    for (i = 0; i < selnumb; i++)
    {
	ob = selobj[i];
	if (ob->objclass != FL_BEGIN_GROUP && ob->objclass != FL_END_GROUP)
	{
	    change_type(ob, curobj->type);
	    set_attribs(ob, curobj->boxtype, curobj->col1, curobj->col2,
			curobj->lcol, curobj->align,
			curobj->lsize, curobj->lstyle, selobj[i]->label);
	    set_miscattribs(ob, curobj->nwgravity, curobj->segravity,
			    curobj->resize);
	}
    }
}

/* interactively change the attributes of the selection */
void
change_selection(void)
{
    FL_OBJECT *firstobj = NULL;
    int objclass = -1, i;
    FL_OBJECT *ob;

    if (cur_form == NULL)
	return;

    if (selnumb == 0)
    {
	fl_show_alert("", "Please select an object first",
		      "by clicking the right button on the object", 0);
	return;
    }


    if (selnumb == 1)
	change_object(selobj[0], TRUE);
    else
    {
	for (i = 0; i < selnumb; i++)
	{
	    ob = selobj[i];
	    if (ob->objclass != FL_BEGIN_GROUP && ob->objclass != FL_END_GROUP)
	    {
		if (firstobj == NULL)
		{
		    firstobj = ob;
		    objclass = ob->objclass;
		}
		else if (objclass != ob->objclass)
		{
		    fl_show_messages("Selected objects have different classes");
		    return;
		}
	    }
	}

	if (firstobj == NULL)
	    return;

	if (!change_object(firstobj, FALSE))
	    return;

	change_selected_objects(firstobj);
    }
    changed = 1;
}


/* Aligns the objects in the selection */
void
align_selection(int dir)
{
    float x, y, w, h, gap;
    int used[10000], current;
    int i, j;

    if (backf || !cur_form)
	return;			/* Cannot align the backface */

    if (selnumb <= 1)
	return;			/* Nothing to align */

    compute_selbox(&x, &y, &w, &h);

    if (dir == FD_HEQUAL)	/* Horizontal equal distance */
    {
	gap = 0.0;
	for (i = 0; i < selnumb; i++)
	    gap += selobj[i]->w;
	gap = (w - gap) / (selnumb - 1);
	for (i = 0; i < selnumb; i++)
	    used[i] = 0;
	for (j = 0; j < selnumb; j++)
	{
	    current = -1;
	    for (i = 0; i < selnumb; i++)
		if (!used[i])
		    if (current == -1 || selobj[i]->x < selobj[current]->x)
			current = i;
	    used[current] = 1;
	    selobj[current]->x = x;
	    x += selobj[current]->w + gap;
	}
    }
    else if (dir == FD_VEQUAL)	/* Vertical equal distance */
    {
	gap = 0.0;
	for (i = 0; i < selnumb; i++)
	    gap += selobj[i]->h;
	gap = (h - gap) / (selnumb - 1);
	for (i = 0; i < selnumb; i++)
	    used[i] = 0;
	for (j = 0; j < selnumb; j++)
	{
	    current = -1;
	    for (i = 0; i < selnumb; i++)
		if (!used[i])
		    if (current == -1 || selobj[i]->y < selobj[current]->y)
			current = i;
	    used[current] = 1;
	    selobj[current]->y = y;
	    y += selobj[current]->h + gap;
	}
    }
    else
	for (i = 0; i < selnumb; i++)
	{
	    switch (dir)
	    {
	    case FD_LEFT:	/* Left */
		selobj[i]->x = x;
		break;
	    case FD_HCENTER:	/* Center */
		selobj[i]->x = x + w / 2.0 - selobj[i]->w / 2.0;
		break;
	    case FD_RIGHT:	/* Right */
		selobj[i]->x = x + w - selobj[i]->w;
		break;
	    case FD_TOP:
		selobj[i]->y = y;
		break;
	    case FD_VCENTER:	/* Center */
		selobj[i]->y = y + h / 2.0 - selobj[i]->h / 2.0;
		break;
	    case FD_BOTTOM:
		selobj[i]->y = y + h - selobj[i]->h;
		break;
	    }
	}
    redraw_the_form(0);
    changed = 1;
}


/* Shows all objects in the selection. */
void
show_selection(void)
{
    int i;
    if (backf)
	return;			/* Cannot show the backface */
    if (cur_form == NULL)
	return;
    for (i = 0; i < selnumb; i++)
	fl_show_object(selobj[i]);
}

/* Hides all objects in the selection. */
void
hide_selection(void)
{
    int i;
    if (backf)
	return;			/* Cannot hide the backface */
    if (cur_form == NULL)
	return;
    for (i = 0; i < selnumb; i++)
	fl_hide_object(selobj[i]);
}

/* Raises the selected objects */
void
raise_selection(void)
{
    int i;

    if (backf)
	return;			/* Cannot raise the backface */
    if (cur_form == NULL)
	return;

    for (i = 0; i < selnumb; i++)
    {
	fl_delete_object(selobj[i]);

	/* add_object expects the coordinates of the object measured from
	   lower-left corner. Translate to conform */
	TRANY(selobj[i], cur_form);
	fl_add_object(cur_form, selobj[i]);
    }
    changed = 1;
}

/* Lowers the selection */
void
lower_selection(void)
{
    int i;

    if (backf)
	return;			/* Cannot lower the backface. */
    if (cur_form == NULL)
	return;

    for (i = selnumb - 1; i >= 0; i--)
    {
	if (selobj[i] != BackOBJ()->next)
	{
	    fl_delete_object(selobj[i]);
	    fl_insert_object(selobj[i], BackOBJ()->next);
	}
    }
    changed = 1;
}

static FL_OBJECT *cutbuf[MAXSEL];	/* Buffered objects */
static int ncut = 0;		/* Their number */

static void
clear_cutbuffer(void)
{
    for (; --ncut >= 0;)
    {
	if (cutbuf[ncut]->u_vdata)
	    fl_free(cutbuf[ncut]->u_vdata);
	fl_free_object(cutbuf[ncut]);
    }

}

/* Removes all elements in the selection. */
void
cut_selection(void)
{
    int i;

    if (backf)
	return;			/* Cannot cut the backface. */

    if (cur_form == NULL)
    {
	fl_show_alert("Warning", "Please Add a form first", "", 0);
	return;
    }

    if (selnumb == 0)
	return;

    clear_cutbuffer();

    /* Make new deletion and save it */
    for (i = 0; i < selnumb; i++)
    {
	fl_delete_object(selobj[i]);
	cutbuf[i] = copy_object(selobj[i], 1);
    }

    ncut = selnumb;
    clear_selection();

    changed = 1;
}

/* Pastes elements form buffer into form */

void
paste_selection(void)
{
    FL_OBJECT *obj;
    float x, y, w, h, ox, oy;
    int i;

    if (cur_form == NULL || ncut == 0)
	return;

    is_pasting = 1;		/* horrible hack */

    /* Copy selection from buffer */
    clear_selection();
    redraw_the_form(0);

    for (i = 0; i < ncut; i++)
    {
	obj = copy_object(cutbuf[i], 1);

	/* fix label:  if underlining caused by cutbuf shortcut, remove it.
	   Note can't use cutbuf as cutbuf does not contain shortcut info */
	if (strchr(obj->label, fl_ul_magic_char[0]) && selobj[i]->shortcut[0])
	{
	    char tmpbuf[128], *dp, *sp;
	    for (dp = tmpbuf, sp = obj->label; *sp; sp++)
		if (*sp != *fl_ul_magic_char)
		    *dp++ = *sp;
	    *dp = '\0';
	    fl_set_object_label(obj, tmpbuf);
	}

	TRANY(obj, cur_form);
	fl_add_object(cur_form, obj);
	selobj[selnumb++] = obj;
    }

    /* Move the selection to the correct place */
    compute_selbox(&x, &y, &w, &h);
    ox = x;
    oy = y;
    move_box(&x, &y, &w, &h, FALSE);
    /* recompute object sizes */
    for (i = 0; i < selnumb; i++)
    {
	if (selobj[i]->objclass != FL_BEGIN_GROUP &&
	    selobj[i]->objclass != FL_END_GROUP)
	{
	    selobj[i]->x += x - ox;
	    selobj[i]->y += y - oy;
	}
    }

    cleanup_selection();
    redraw_the_form(0);
    changed = 1;
    is_pasting = 0;
}

/* Copies all elements in the selection  to the buffer */
void
copy_selection(void)

{
    FL_OBJECT *obj;
    int i;

    if (backf || selnumb == 0 || !cur_form)
	return;

    clear_cutbuffer();

    /* Copy the objects */
    for (i = 0; i < selnumb; i++)
    {
	obj = copy_object(selobj[i], 0);
	cutbuf[i] = obj;
    }

    ncut = selnumb;
}

/* make a copy of the current selection */
void *
dup_selection(void)
{
    FL_OBJECT **ob;
    int i;

    if (!selnumb)
	return 0;

    ob = fl_calloc(selnumb + 1, sizeof(*ob));
    for (i = 0; i < selnumb; i++)
	ob[i] = copy_object(selobj[i], 1);
    return ob;
}

void
free_dupped_selection(void *a)
{
    FL_OBJECT **ob = a;
    for (; *ob; ob++)
	fl_free_object(*ob);
    fl_free(a);
}

/* change the selection to a new list of objects and show it */
void
set_selection(void *a)
{
    FL_OBJECT *obj, **ob = a;
    int i;

    for (i = 0; i < selnumb; i++)
	fl_delete_object(selobj[i]);

    clear_selection();
    redraw_the_form(0);

    for (selnumb = 0; ob[selnumb]; selnumb++)
    {
	obj = copy_object(ob[selnumb], 1);
	TRANY(obj, cur_form);
	fl_add_object(cur_form, obj);
	selobj[selnumb] = obj;
    }
    redraw_the_form(0);
}

void
next_selection(void)
{
    if (!cur_form || !BackOBJ())
	return;
    do
    {
	if (!selnumb || !selobj[0]->next)
	    selobj[0] = BackOBJ()->next ? BackOBJ()->next : BackOBJ();
	else if (selnumb && selobj[0]->next)
	    selobj[0] = selobj[0]->next;

    }
    while (selobj[0]->is_child);

    selnumb = 1;
    redraw_the_form(0);

}

void
prev_selection(void)
{
    if (!cur_form || !BackOBJ())
	return;
    do
    {
	if (!selnumb || !selobj[0]->prev)
	    selobj[0] = BackOBJ()->prev ? BackOBJ()->prev : BackOBJ();
	else if (selnumb && selobj[0]->prev)
	    selobj[0] = selobj[0]->prev;

    }
    while (selobj[0]->is_child);

    selnumb = 1;
    redraw_the_form(0);

}


/* Groups all elements in the selection into a group */
void
group_selection(void)
{
    int i;
    FL_OBJECT *obj;
    const char *s;

    if (backf)
	return;			/* Cannot group the backface */

    if (cur_form == NULL || selnumb == 0)
	return;

    if (!(s = fl_show_input("Give group name", "")))
	return;

    obj = add_an_object(FL_BEGIN_GROUP, -1, 0, 0, 0, 0);
    for (i = 0; i < selnumb; i++)
    {
	{
	    fl_delete_object(selobj[i]);
	    if (selobj[i]->objclass != FL_BEGIN_GROUP &&
		selobj[i]->objclass != FL_END_GROUP)
	    {
		TRANY(selobj[i], cur_form);
		fl_add_object(cur_form, selobj[i]);
	    }
	}
    }

    add_an_object(FL_END_GROUP, -1, 0, 0, 0, 0);
    clear_selection();
    set_object_name(obj, s, "", "");
    addgroupto_selection(obj);
    changed = 1;
}

/* Removes begin and endgroups from the selection */
void
flatten_selection(void)
{
    int i;

    if (backf)
	return;			/* Cannot flatten the backface */

    if (cur_form == NULL)
	return;

    for (i = 0; i < selnumb; i++)
    {
	if (selobj[i]->objclass == FL_BEGIN_GROUP ||
	    selobj[i]->objclass == FL_END_GROUP)
	{
	    fl_delete_object(selobj[i]);
	    /* CAUSES PROBLEMS WITH NAMING fl_free_object(selobj[i]); */
	    selobj[i] = NULL;
	}
    }

    cleanup_selection();
    changed = 1;
}
