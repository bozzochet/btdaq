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
 * $Id: choice.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *
 * FL_CHOICE class
 *
 */

#if defined(F_ID) || defined(DEBUG)
char *fl_id_chc = "$Id: choice.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include <string.h>
#include <stdlib.h>
#include "forms.h"
#include "pchoice.h"

#define SPEC   FL_CHOICE_SPEC

#define FL_CHOICE_MAXSTR  128

static int lastpup_return;

#define SafeFree(s)  if(s) {fl_free(s); s=0;}

static void
free_choice(SPEC *sp)
{
    int i;

    for (i = 1; i <= FL_CHOICE_MAXITEMS; i++)
    {
	SafeFree(sp->items[i]);
	SafeFree(sp->shortcut[i]);
    }
}

/* Draws a choice object */
static void
draw_choice(FL_OBJECT * ob)
{
    FL_COLOR c1;
    SPEC *sp = ((SPEC *) (ob->spec));
    char *str, *cc;
    int absbw = FL_abs(ob->bw);
    int off1 = 0, off2 = 0;

    c1 = ob->belowmouse ? FL_CHOICE_MCOL : ob->col1;

    fl_drw_box(ob->boxtype, ob->x, ob->y, ob->w, ob->h, c1, ob->bw);

    if (ob->type == FL_NORMAL_CHOICE2)
    {
	int dh = (int)FL_max(6 + (ob->bw > 0), ob->h * 0.1f);
	int dw = (int)FL_max(0.11f * ob->w, 13);
	int dbh = FL_max(absbw - 1, 1);
	int align = sp->align & ~FL_ALIGN_INSIDE;

	fl_drw_box(FL_UP_BOX,
		   (FL_Coord) (ob->x + ob->w - dw - absbw - 2),
		   (FL_Coord) ob->y + (ob->h - dh) / 2,
		   (FL_Coord) dw, (FL_Coord) dh, ob->col1, -dbh);

	off1 = align == FL_ALIGN_CENTER ? (dw / 2) : 0;
	off2 = align == FL_ALIGN_RIGHT ? dw : 0;
    }

    fl_drw_text_beside(ob->align, ob->x, ob->y, ob->w, ob->h, ob->lcol,
		       ob->lstyle, ob->lsize, ob->label);

    /* string can conceivably contain ``type flags". need to get rid of them
       on the fly */
    if (sp->val > 0 && sp->val <= sp->numitems)
    {
	str = fl_strdup(sp->items[sp->val]);
	if ((cc = strchr(str, '%')))
	{
	    if (*(cc + 1) == '%')
		*(cc + 1) = '\0';
	    else
		*cc = '\0';
	}

	fl_set_text_clipping(ob->x + absbw, ob->y,
			     ob->w - 2 * absbw, ob->h);
	fl_drw_text(sp->align, ob->x - off1, ob->y, ob->w - off2,
		    ob->h, ob->col2, sp->fontstyle, sp->fontsize,
		    str + (str && (*str == '\010')));
	fl_unset_text_clipping();
	fl_free(str);
    }
}

static void
draw_droplist_choice(FL_OBJECT * ob)
{
    FL_COLOR c1;
    SPEC *sp = ((SPEC *) (ob->spec));
    char *str, *cc;
    FL_Coord dw = ob->h, dx = ob->w - dw, bw;

    c1 = sp->below ? FL_CHOICE_MCOL : ob->col1;
    bw = ob->bw;
    if (bw > 0)
	bw -= ob->bw > 1;

    /* arrows */
    fl_drw_box(sp->pushed ? FL_DOWN_BOX : FL_UP_BOX, ob->x + dx, ob->y,
	       dw, ob->h, c1, bw);
    fl_drw_text(FL_ALIGN_CENTER, ob->x + dx + 2, ob->y + 2, dw - 4, ob->h - 4,
		FL_BLACK, 0, 0, "@#2->");

    /* choice box */
    fl_drw_box(ob->boxtype, ob->x, ob->y, dx, ob->h, ob->col1, ob->bw);
    fl_drw_text_beside(ob->align, ob->x, ob->y, dx, ob->h, ob->lcol,
		       ob->lstyle, ob->lsize, ob->label);

    /* string can conceivably contain ``type flags". need to get rid of them
       on the fly */
    if (sp->val > 0)
    {
	str = fl_strdup(sp->items[sp->val]);
	if ((cc = strchr(str, '%')))
	{
	    if (*(cc + 1) == '%')
		*(cc + 1) = '\0';
	    else
		*cc = '\0';
	}

	fl_set_text_clipping(ob->x + FL_abs(ob->bw), ob->y,
			     ob->w - 2 * FL_abs(ob->bw), ob->h);
	fl_drw_text(sp->align, ob->x, ob->y, dx, ob->h, ob->col2,
		    sp->fontstyle, sp->fontsize,
		    str + (str && (*str == '\010')));
	fl_unset_text_clipping();
	fl_free(str);
    }
}


/* due to grayout, need to find out which one is valid */
static int
set_valid_entry(SPEC *sp, int target, int dir)
{
    for (; target > 0 && target <= sp->numitems; target += dir)
	if (!(sp->mode[target] & FL_PUP_GREY))
	    return target;
    Bark("Choice", "No valid entries");
    return -1;
}

static int
do_pup(FL_OBJECT * ob)
{
    int menu;
    SPEC *sp = ob->spec;
    char title[256];
    int i;

    menu = fl_newpup(FL_ObjWin(ob));

    /* fake a title */
    if (ob->label && ob->label[0] && ob->type != FL_DROPLIST_CHOICE)
    {
	if (!sp->no_title)
        {
            snprintf(title,sizeof(title)-4,"%s",ob->label);
	    fl_addtopup(menu, strcat(title, "%t"));
        }
    }

    for (i = 1; i <= sp->numitems; i++)
    {
	fl_addtopup(menu, sp->items[i]);
	if (sp->modechange[i] || sp->mode[i] != FL_PUP_NONE)
	{
	    fl_setpup(menu, i, sp->mode[i]);
	    sp->modechange[i] = 0;
	}
	fl_setpup_shortcut(menu, i, sp->shortcut[i]);
    }

    fl_setpup_shadow(menu, (ob->type != FL_DROPLIST_CHOICE));
    fl_setpup_selection(menu, sp->val);

    fl_setpup_softedge(menu, (ob->bw < 0));

    i = fl_dopup(menu);

    if (i > 0)
    {
	sp->mode[i] = fl_getpup_mode(menu, i);
	sp->modechange[i] = 1;
    }

    fl_freepup(menu);
    return lastpup_return = i;
}

static int mousebutton = 0;

#define Within(x,y,w,h)   (mx >= (x) && mx <= (x + w) && my >= (y)\
                          && my <= (y + h))

/* Handles an event, returns whether value has changed. */
static int
handle_choice(FL_OBJECT * ob, int event, FL_Coord mx, FL_Coord my,
	      int key, void *ev)
{
    int val, i, in;
    SPEC *sp = ((SPEC *) (ob->spec));

#if (FL_DEBUG >= ML_DEBUG)
    M_info2("HandleChoice", fl_event_name(event));
#endif

    switch (event)
    {
    case FL_DRAW:
	/* always force outside alignment */
	ob->align &= ~FL_ALIGN_INSIDE;
	(ob->type == FL_DROPLIST_CHOICE ? draw_droplist_choice : draw_choice) (ob);	/* Draw 






											   the 






											   object 






											 */
	break;
    case FL_DRAWLABEL:
	fl_drw_text_beside(ob->align, ob->x, ob->y, ob->w, ob->h, ob->lcol,
			   ob->lstyle, ob->lsize, ob->label);
	break;
    case FL_PUSH:
	mousebutton = key;
	if (sp->numitems == 0)
	    return 0;
	sp->counter = 0;
	if (mousebutton == 1 &&
	    (ob->type == FL_NORMAL_CHOICE || ob->type == FL_NORMAL_CHOICE2))
	{
	    if ((val = do_pup(ob)) > 0)
		sp->val = val;
	    fl_redraw_object(ob);
	}
	break;
    case FL_MOUSE:
	if (sp->numitems == 0)
	    return 0;

	if (ob->type == FL_DROPLIST_CHOICE &&
	    (in = Within(ob->x + ob->w - ob->h, ob->y, ob->h, ob->h)))
	{
	    if (!(in ^ !sp->pushed))
	    {
		sp->pushed = !sp->pushed;
		fl_redraw_object(ob);
	    }
	    return 0;
	}

	if (mousebutton == 3 && (sp->counter++ % 20) == 0)
	{
	    if (sp->val == sp->numitems)
		sp->val = set_valid_entry(sp, 1, 1);
	    else
		sp->val = set_valid_entry(sp, sp->val + 1, 1);
	    fl_redraw_object(ob);
	    lastpup_return = sp->val;
	}
	else if (mousebutton == 2 && (sp->counter++ % 15) == 0)
	{
	    if (sp->val <= 1)
		sp->val = set_valid_entry(sp, sp->numitems, -1);
	    else
		sp->val = set_valid_entry(sp, sp->val - 1, -1);
	    fl_redraw_object(ob);
	    lastpup_return = sp->val;
	}
	break;
    case FL_RELEASE:
	mousebutton = 0;
	if (ob->type == FL_DROPLIST_CHOICE && sp->pushed)
	{
	    sp->pushed = 0;
	    fl_setpup_position(-(ob->form->x + ob->x + ob->w),
			       ob->form->y + ob->y + ob->h + FL_PUP_PADH);
	    if ((val = do_pup(ob)) > 0)
		sp->val = val;
	    fl_redraw_object(ob);
	    return lastpup_return > 0;
	}
	sp->pushed = 0;
	return (lastpup_return > 0) || (ob->type == FL_DROPLIST_CHOICE);
    case FL_LEAVE:
	sp->below = 0;
	/* FALL THROUGH */
    case FL_ENTER:
	fl_redraw_object(ob);
	break;
    case FL_MOTION:
	if (ob->type == FL_DROPLIST_CHOICE)
	{
	    in = Within(ob->x + ob->w - ob->h, ob->y, ob->h, ob->h);
	    if (sp->below ^ in)
	    {
		sp->below = in;
		fl_redraw_object(ob);
	    }
	}
	break;
    case FL_SHORTCUT:
	fl_setpup_position(ob->form->x + ob->x + 10,
			   ob->form->y + ob->y + ob->h / 2);
	if ((i = do_pup(ob)) != sp->val && i >= 0)
	{
	    sp->val = i;
	    fl_redraw_object(ob);
	    return 1;
	}
	break;
    case FL_FREEMEM:
	free_choice(ob->spec);
	fl_free(ob->spec);
	break;
    default:
	break;
    }
    return 0;
}

/*-------------------------------------------*/

/* creates an object */
FL_OBJECT *
fl_create_choice(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		 const char *label)
{
    FL_OBJECT *ob;
    int i;
    SPEC *sp;

    ob = fl_make_object(FL_CHOICE, type, x, y, w, h, label, handle_choice);
    ob->boxtype = type == FL_NORMAL_CHOICE2 ? FL_UP_BOX : FL_CHOICE_BOXTYPE;
    ob->col1 = FL_CHOICE_COL1;
    ob->col2 = FL_CHOICE_COL2;
    ob->lcol = FL_CHOICE_LCOL;
    ob->align = FL_CHOICE_ALIGN;
    ob->spec = fl_calloc(1, sizeof(SPEC));

    sp = (ob->spec);
    sp->fontsize = fl_cntl.choiceFontSize ?
	fl_cntl.choiceFontSize : FL_DEFAULT_FONT;
    sp->fontstyle = FL_NORMAL_STYLE;
    sp->align = FL_ALIGN_CENTER;
    sp->val = 0;

    for (i = 0; i <= FL_CHOICE_MAXITEMS; i++)
    {
	sp->items[i] = 0;
	sp->shortcut[i] = 0;
    }

    return ob;
}

/* Adds an object */
FL_OBJECT *
fl_add_choice(int type, FL_Coord x, FL_Coord y,
	      FL_Coord w, FL_Coord h, const char *l)
{
    FL_OBJECT *ob;
    ob = fl_create_choice(type, x, y, w, h, l);
    fl_add_object(fl_current_form, ob);
    return ob;
}

/*-------------------------------------*/

/* Clears the choice object */
void
fl_clear_choice(FL_OBJECT * ob)
{

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_CHOICE))
    {
	Bark("ClearChoice", "%s is not choice class", ob ? ob->label : "");
	return;
    }
#endif

    free_choice(ob->spec);

    ((SPEC *) ob->spec)->val = 0;
    ((SPEC *) ob->spec)->numitems = 0;
    fl_redraw_object(ob);
}

/* add a single choice item */
static void
addto_choice(FL_OBJECT * ob, const char *str)
{
    SPEC *sp = ((SPEC *) (ob->spec));
    int cur_item;

    if (sp->numitems >= FL_CHOICE_MAXITEMS)
	return;

    cur_item = ++(sp->numitems);

#ifdef FL_CHOICE_MAXSTR
    sp->items[cur_item] = (char *) fl_malloc(FL_CHOICE_MAXSTR + 1);
    strncpy(sp->items[cur_item], str, FL_CHOICE_MAXSTR);
    sp->items[cur_item][FL_CHOICE_MAXSTR] = 0;
#else
    sp->items[cur_item] = fl_strdup(str);
#endif

    sp->shortcut[cur_item] = (char *) fl_malloc(1);
    sp->shortcut[cur_item][0] = '\0';
    sp->mode[cur_item] = FL_PUP_NONE;
    sp->modechange[cur_item] = 0;

    if (sp->val == 0)
    {
	sp->val = 1;
	fl_redraw_object(ob);
    }
}

/* user interface routine. | allowed */
int
fl_addto_choice(FL_OBJECT * ob, const char *str)
{
    SPEC *sp = ((SPEC *) (ob->spec));
    char ttt[128];
    int i, j;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_CHOICE))
    {
	Bark("AddtoChoice", "%s is not choice class", ob ? ob->label : "");
	return 0;
    }
#endif

    if (sp->numitems >= FL_CHOICE_MAXITEMS)
	return sp->numitems;

    for (i = j = 0; str[i] != '\0'; i++)
    {
	if (str[i] == '|')
	{
	    ttt[j] = '\0';
	    addto_choice(ob, ttt);
	    j = 0;
	}
	else
	    ttt[j++] = str[i];
    }

    if (j != 0)
    {
	ttt[j] = '\0';
	addto_choice(ob, ttt);
    }

    return sp->numitems;
}

/* Replaces a line to the choice item. */
void
fl_replace_choice(FL_OBJECT * ob, int numb, const char *str)
{
    SPEC *sp = ((SPEC *) (ob->spec));

    if (numb < 1 || numb > sp->numitems)
	return;
#ifdef FL_CHOICE_MAXSTR
    strncpy(sp->items[numb], str, FL_CHOICE_MAXSTR);
    sp->items[numb][FL_CHOICE_MAXSTR] = 0;
#else
    fl_free(sp->items[numb]);
    sp->items[numb] = fl_strdup(str);
#endif

    if (sp->val == numb)
	fl_redraw_object(ob);
}

/* Removes a line from the choice item. */
void
fl_delete_choice(FL_OBJECT * ob, int numb)
{
    int i;
    SPEC *sp = ((SPEC *) (ob->spec));

    if (numb < 1 || numb > sp->numitems)
	return;

    fl_free(sp->items[numb]);
    fl_free(sp->shortcut[numb]);
    for (i = numb; i < sp->numitems; i++)
    {
	sp->items[i] = sp->items[i + 1];
	sp->shortcut[i] = sp->shortcut[i + 1];
    }

    sp->items[sp->numitems] = NULL;
    sp->numitems--;

    if (sp->val == numb)
    {
	if (sp->val > sp->numitems)
	    sp->val = sp->numitems;
	fl_redraw_object(ob);
    }
    else if (sp->val > numb)
	sp->val--;
}

/*------------------------------------------*/

/* Sets the number of the choice. */
void
fl_set_choice(FL_OBJECT * ob, int choice)
{
    SPEC *sp = ((SPEC *) (ob->spec));

    if (choice < 1 || choice > sp->numitems)
	sp->val = 0;
    else
	sp->val = choice;
    fl_redraw_object(ob);
}

/* similar to set_choice, except we use txt */
void
fl_set_choice_text(FL_OBJECT * ob, const char *txt)
{
    SPEC *sp;
    int i;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_CHOICE))
    {
	Bark("SetChoiceText", "%s not choice class", ob ? ob->label : "");
	return;
    }
#endif
    sp = ob->spec;

    for (i = 1; i <= sp->numitems; i++)
	if (strcmp(txt, sp->items[i]) == 0)
	{
	    fl_set_choice(ob, i);
	    return;
	}
    M_err("SetChoiceText", "%s not found", txt);
}

void
fl_set_choice_item_mode(FL_OBJECT * ob, int item, unsigned mode)
{
    SPEC *sp = ((SPEC *) (ob->spec));

    if (item < 1 || item > sp->numitems)
    {
	M_err("ChoiceMode", "Bad item index %d", item);
	return;
    }
    sp->mode[item] = mode;
    sp->modechange[item] = 1;
}

void
fl_set_choice_item_shortcut(FL_OBJECT * ob, int item, const char *sc)
{
    SPEC *sp = ob->spec;

    if (item < 1 || item > sp->numitems)
    {
	M_err("ChoiceShortcut", "Bad item index %d", item);
	return;
    }
    sp->shortcut[item] = fl_realloc(sp->shortcut[item], 1 + strlen(sc));
    strcpy(sp->shortcut[item], sc);
}

/* Returns the number of the choice. */
int
fl_get_choice(FL_OBJECT * ob)
{
    SPEC *sp = ((SPEC *) (ob->spec));

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_CHOICE))
    {
	Bark("GetChoice", "%s is not choice class", ob ? ob->label : "");
	return 0;
    }
#endif
    return sp->val;
}

int
fl_get_choice_maxitems(FL_OBJECT * ob)
{
    return ((SPEC *) (ob->spec))->numitems;
}

/* Returns the text of the choice. */
const char *
fl_get_choice_text(FL_OBJECT * ob)
{
    SPEC *sp = ((SPEC *) (ob->spec));

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_CHOICE))
    {
	Bark("GetChoiceText", "%s is not choice class", ob ? ob->label : "");
	return 0;
    }
#endif
    if (sp->val == 0)
	return NULL;
    return sp->items[sp->val];
}

const char *
fl_get_choice_item_text(FL_OBJECT * ob, int n)
{
    SPEC *sp = ob->spec;

    if (n < 1 || n > sp->numitems)
	return 0;
    return sp->items[n];
}

/*--------------------------------------------------*/

/* Sets the font size inside the choice. */
void
fl_set_choice_fontsize(FL_OBJECT * ob, int size)
{
    if (((SPEC *) (ob->spec))->fontsize != size)
    {
	((SPEC *) (ob->spec))->fontsize = size;
	fl_redraw_object(ob);
    }
}

/* Sets the font style inside the choice. */
void
fl_set_choice_fontstyle(FL_OBJECT * ob, int style)
{
    if (((SPEC *) (ob->spec))->fontstyle != style)
    {
	((SPEC *) (ob->spec))->fontstyle = style;
	fl_redraw_object(ob);
    }
}

void
fl_set_choice_align(FL_OBJECT * ob, int align)
{
    if (((SPEC *) (ob->spec))->align != align)
    {
	((SPEC *) (ob->spec))->align = align;
	fl_redraw_object(ob);
    }
}

/* ignore the callback stuff */
int
fl_set_choice_entries(FL_OBJECT * ob, FL_PUP_ENTRY * ent)
{
    int i, k;

    fl_clear_choice(ob);

    for (k = 0; ent && ent->text; ent++, k++)
    {
	i = fl_addto_choice(ob, ent->text);
	if (ent->mode == FL_PUP_GRAY)
	    fl_set_choice_item_mode(ob, i, ent->mode);
	if (ent->shortcut && *ent->shortcut)
	    fl_set_choice_item_shortcut(ob, i, ent->shortcut);
    }

    return k;
}

int
fl_set_choice_notitle(FL_OBJECT * ob, int n)
{
    SPEC *sp = ob->spec;
    int old = sp->no_title;

    sp->no_title = n;
    return old;
}
