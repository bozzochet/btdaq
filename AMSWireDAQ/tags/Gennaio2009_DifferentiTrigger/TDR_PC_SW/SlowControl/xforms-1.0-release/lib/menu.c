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
 * $Id: menu.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *
 *  XForms Class FL_MENU.
 *    call PopUp to handle the actual random access
 *
 *  possible optimization:
 *   upon first creation of the popup, set extern_menu to the popup ID
 *   and let popup manage all the modes/values from then on.
 *
 *  OR use the following to simplify code for extern pup
 *    when extern_menu is used, gets all text and mode, then
 *    all the code would be the same for extern and native menu.
 *
 */
#if defined(F_ID) || defined(DEBUG)
char *fl_id_menu = "$Id: menu.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include <string.h>
#include <stdlib.h>
#include "forms.h"
#include "pmenu.h"


#define SPEC        FL_MENU_SPEC
#define ISPUP(sp)   (sp->extern_menu >= 0)

/* Due to the possibility of %t %x present in the item str, an item's
 * value and its index may be different.
 */
static int
val_to_index(SPEC *sp, int val)
{
    int i;

    for (i = 1; i <= sp->numitems; i++)
	if (val == sp->mval[i])
	    return i;
    return 0;
}

/* Creates the menu and shows it. Returns the item selected. */
static int
do_menu(FL_OBJECT * ob)
{
    int i, val;
    SPEC *sp = ((SPEC *) (ob->spec));

    if (sp->numitems == 0 && sp->extern_menu < 0)
	return 0;

    if (sp->extern_menu >= 0)
    {
	Window oparent, win;
	fl_getpup_window(sp->extern_menu, &oparent, &win);
	if (*ob->label && ob->type != FL_PULLDOWN_MENU)
	    fl_setpup_title(sp->extern_menu, ob->label);
	fl_reparent_pup(sp->extern_menu, FL_ObjWin(ob));
	val = (int) fl_dopup(sp->extern_menu);
	/* menu might go away, need to restore old parent */
	fl_reparent_pup(sp->extern_menu, oparent);
	return val;
    }

    sp->menu = fl_newpup(FL_ObjWin(ob));

    if (ob->type != FL_PULLDOWN_MENU)
	fl_setpup_title(sp->menu, ob->label);
    else
    {
	fl_setpup_shadow(sp->menu, 0);
	fl_setpup_softedge(sp->menu, 1);
    }

    for (i = 1; i <= sp->numitems; i++)
    {
	fl_addtopup(sp->menu, sp->items[i]);
	if ((sp->modechange[i] || sp->mode[i] != FL_PUP_NONE) && sp->mval[i])
	{
	    fl_setpup_mode(sp->menu, sp->mval[i], sp->mode[i]);
	    sp->modechange[i] = 0;
	}
	fl_setpup_shortcut(sp->menu, i, sp->shortcut[i]);
    }

    val = (int) fl_dopup(sp->menu);

    if (val > 0)
    {
	/* if shown for the first time, need to get all mode right as the
	   menu item string may have embedded mode setting strings in it
	   (e.g., R1 etc) */
	if (sp->shown == 0)
	{
	    for (i = 1; i <= sp->numitems; i++)
	    {
		int m = fl_getpup_mode(sp->menu, sp->mval[i]);
		sp->modechange[i] = sp->mode[i] != m;
		sp->mode[i] = m;
		sp->shown = 1;
	    }
	}
	else
	{
	    int k = val_to_index(sp, val);
	    sp->mode[k] = fl_getpup_mode(sp->menu, val);
	    sp->modechange[k] = 1;
	    /* old val also might change mode if binary */
	    if (sp->val > 0)
	    {
		int m = fl_getpup_mode(sp->menu, sp->val);
		k = val_to_index(sp, sp->val);
		sp->modechange[k] = sp->mode[k] != m;
		sp->mode[k] = m;
	    }
	}
    }

    fl_freepup(sp->menu);
    sp->menu = 0;

    return val;
}


/* Handles an event, returns whether value has changed. Here
 * we only update sp->val if dopup return a positive value,
 * otherwise two radio button might be set due to mode[val]
 * and sp->val checking reset
 */
static int
handle_menu(FL_OBJECT * ob, int event, FL_Coord mx, FL_Coord my,
	    int key, void *ev)
{
    SPEC *sp = ((SPEC *) (ob->spec));
    static int i = -1;
    int boxtype = ob->boxtype, bw = ob->bw, dm;
    FL_COLOR col;

#if (FL_DEBUG >= ML_DEBUG)
    M_info2("HandleMenu", fl_event_name(event));
#endif

    switch (event)
    {
    case FL_DRAW:
	/* Draw the object */
	if ((ob->type == FL_PUSH_MENU && ob->pushed) ||
	    (ob->type == FL_TOUCH_MENU && ob->belowmouse))
	    col = ob->col2;
	else
	    col = ob->col1;

	if (ob->type == FL_PULLDOWN_MENU && ob->pushed)
	{
	    /* col = ob->col2; */
	    boxtype = FL_UP_BOX;
	    bw = -2;
	}

	fl_drw_box(boxtype, ob->x, ob->y, ob->w, ob->h, col, bw);
	fl_drw_text(ob->align, ob->x, ob->y, ob->w, ob->h,
		    ob->lcol, ob->lstyle, ob->lsize, ob->label);
	if (sp->showsymbol)
	{
	    dm = 0.85 * FL_min(ob->w, ob->h);
	    fl_drw_text(0, (ob->x + ob->w - dm - 1), ob->y + 1,
			dm, dm, col, 0, 0, "@menu");
	}
	return 0;
    case FL_ENTER:
	if (ob->type == FL_TOUCH_MENU)
	{
	    fl_redraw_object(ob);
	    if ((i = do_menu(ob)) > 0)
		sp->val = i;
	}
	return 0;
    case FL_LEAVE:
	fl_redraw_object(ob);
	return (ob->type == FL_TOUCH_MENU && sp->val != -1 && i > 0);
    case FL_PUSH:
	if (ob->type == FL_PUSH_MENU || ob->type == FL_PULLDOWN_MENU)
	{
	    fl_redraw_object(ob);
	    if (ob->type == FL_PULLDOWN_MENU)
		fl_setpup_position(ob->form->x + ob->x + 1,
			 ob->form->y + ob->y + ob->h + 2 * FL_PUP_PADH + 1);
	    if ((i = do_menu(ob)) > 0)
		sp->val = i;
	    fl_redraw_object(ob);
	}
	return 0;
    case FL_RELEASE:
	fl_redraw_object(ob);
	return (ob->type != FL_TOUCH_MENU && sp->val != -1 && i > 0);
    case FL_SHORTCUT:
	ob->pushed = 1;
	fl_redraw_object(ob);
	if (ob->type == FL_PULLDOWN_MENU)
	    fl_setpup_position(ob->form->x + ob->x + 1,
			 ob->form->y + ob->y + ob->h + 2 * FL_PUP_PADH + 1);
	else
	    fl_setpup_position(ob->form->x + ob->x + 5,
			       ob->form->y + ob->y + ob->h + 5);

	if ((i = do_menu(ob)) != sp->val && i > 0)
	    sp->val = i;

	ob->pushed = 0;
	fl_redraw_object(ob);
	return sp->val != -1 && i > 0;
    case FL_FREEMEM:
	fl_clear_menu(ob);
	fl_free(ob->spec);
	return 0;
    default:
	return 0;
    }
}

/*-------------------------------------------*/

/* creates an object */
FL_OBJECT *
fl_create_menu(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
	       const char *label)
{
    FL_OBJECT *ob;
    SPEC *sp;

    ob = fl_make_object(FL_MENU, type, x, y, w, h, label, handle_menu);
    if (type == FL_PULLDOWN_MENU)
	ob->boxtype = FL_FLAT_BOX;
    else
	ob->boxtype = FL_MENU_BOXTYPE;
    ob->col1 = FL_MENU_COL1;
    ob->col2 = FL_MENU_COL2;
    ob->lcol = FL_MENU_LCOL;
    ob->lstyle = FL_BOLD_STYLE;
    ob->align = FL_MENU_ALIGN;

    ob->spec = fl_calloc(1, sizeof(SPEC));
    sp = ((SPEC *) (ob->spec));
    sp->extern_menu = -1;
    return ob;
}

/* Adds an object */
FL_OBJECT *
fl_add_menu(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
	    const char *label)
{
    FL_OBJECT *ob;
    ob = fl_create_menu(type, x, y, w, h, label);
    fl_add_object(fl_current_form, ob);
    return ob;
}

/*-------------------------------------*/

/* Clears the menu object */
void
fl_clear_menu(FL_OBJECT * ob)
{
    int i;
    SPEC *sp = ((SPEC *) (ob->spec));

    sp->val = 0;
    sp->cur_val = 0;

    if (ISPUP(sp))
    {
	fl_freepup(sp->extern_menu);
	sp->extern_menu = -1;
	return;
    }

    for (i = 1; i <= sp->numitems; i++)
    {
	fl_free(sp->items[i]);
	fl_free(sp->shortcut[i]);
	sp->mode[i] = FL_PUP_NONE;
    }

    sp->numitems = 0;
}

/* Adds a line to the menu item. */
static void
addto_menu(FL_OBJECT * ob, const char *str)
{
    SPEC *sp = ((SPEC *) (ob->spec));
    int n;

    if (sp->numitems >= FL_MENU_MAXITEMS)
	return;

    n = ++(sp->numitems);
    sp->items[n] = (char *) fl_malloc(FL_MENU_MAXSTR + 1);
    strncpy(sp->items[n], str, FL_MENU_MAXSTR);
    sp->items[n][FL_MENU_MAXSTR] = '\0';
    sp->shortcut[n] = fl_malloc(1);
    sp->shortcut[n][0] = '\0';
    sp->mode[n] = FL_PUP_NONE;

    /* If we want to support %x, need to parse the string */

    if (!strstr(sp->items[n], "%t"))
	sp->mval[n] = ++sp->cur_val;
}


/* Sets the menu to a particular menu string */
void
fl_set_menu(FL_OBJECT * ob, const char *menustr)
{
    fl_clear_menu(ob);
    fl_addto_menu(ob, menustr);
}

/* Adds a line to the menu item. */
int
fl_addto_menu(FL_OBJECT * ob, const char *menustr)
{
    char ttt[256];
    int i = 0, j = 0;
    SPEC *sp;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_MENU))
    {
	Bark("AddtoMenu", "%s is not Menu class", ob ? ob->label : "");
	return 0;
    }
#endif
    sp = ob->spec;

    while (menustr[i] != '\0')
    {
	if (menustr[i] == '|')
	{
	    ttt[j] = '\0';
	    addto_menu(ob, ttt);
	    j = 0;
	}
	else
	    ttt[j++] = menustr[i];
	i++;
    }

    if (j != 0)
    {
	ttt[j] = '\0';
	addto_menu(ob, ttt);
    }

    return sp->numitems;
}

/* Replaces a line in the menu item. */
void
fl_replace_menu_item(FL_OBJECT * ob, int numb, const char *str)
{
    SPEC *sp = ((SPEC *) (ob->spec));

    if (ISPUP(sp))
    {
	fl_replacepup_text(sp->extern_menu, numb, str);
    }
    else
    {
	if (numb < 1 || numb > sp->numitems)
	    return;
	strncpy(sp->items[numb], str, FL_MENU_MAXSTR);
	sp->items[numb][FL_MENU_MAXSTR] = '\0';
    }
}

#if 0
/* assign menu item values. Currently %x is not supported */
static void
gen_index(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    int i;

    sp->cur_val = 0;
    for (i = 1; i <= sp->numitems; i++)
	if (!strstr(sp->items[i], "%t"))
	    sp->mval[i] = ++sp->cur_val;
}
#endif


/* Removes a line from the menu item. */
void
fl_delete_menu_item(FL_OBJECT * ob, int numb)
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
	sp->mode[i] = sp->mode[i + 1];
	sp->modechange[i] = sp->modechange[i + 1];
	sp->mval[i] = sp->mval[i + 1] - 1;
	sp->shortcut[i] = sp->shortcut[i + 1];
    }

    sp->mode[sp->numitems] = FL_PUP_NONE;
    sp->items[sp->numitems] = 0;
    sp->numitems--;
    sp->cur_val--;
}

void
fl_set_menu_item_shortcut(FL_OBJECT * ob, int numb, const char *str)
{
    SPEC *sp = ((SPEC *) (ob->spec));
    sp->shortcut[numb] = fl_realloc(sp->shortcut[numb], strlen(str) + 1);
    strcpy(sp->shortcut[numb], str);
}

/* Sets the display mode for the menu item */
void
fl_set_menu_item_mode(FL_OBJECT * ob, int numb, unsigned mode)
{
    SPEC *sp = ((SPEC *) (ob->spec));

    if (ISPUP(sp))
    {
	fl_setpup_mode(sp->extern_menu, numb, mode);
    }
    else
    {
	if (numb < 1 || numb > sp->numitems)
	    return;

	sp->mode[numb] = mode;
	sp->modechange[numb] = 1;

	if ((mode & FL_PUP_CHECK))
	    sp->val = numb;
    }
}

/* Makes the menu symbol visible or not */
void
fl_show_menu_symbol(FL_OBJECT * ob, int show)
{
    SPEC *sp = ((SPEC *) (ob->spec));
    sp->showsymbol = show;
    fl_redraw_object(ob);
}

/* Returns the number of the menu item selected. */
int
fl_get_menu(FL_OBJECT * ob)
{
    SPEC *sp = ((SPEC *) (ob->spec));

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_MENU))
    {
	Bark("GetMenu", "%s is not Menu class", ob ? ob->label : "");
	return 0;
    }
#endif

    return sp->val;
}

int
fl_get_menu_maxitems(FL_OBJECT * ob)
{
    SPEC *sp = ((SPEC *) (ob->spec));

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_MENU))
    {
	Bark("GetMenu", "%s is not Menu class", ob ? ob->label : "");
	return 0;
    }
#endif
    return ISPUP(sp) ? fl_getpup_items(sp->extern_menu) : sp->numitems;
}

/* Returns the text of the menu item selected. */
const char *
fl_get_menu_text(FL_OBJECT * ob)
{
    SPEC *sp = ((SPEC *) (ob->spec));
    const char *s;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_MENU))
    {
	Bark("GetMenuText", "%s is not Menu class", ob ? ob->label : "");
	return 0;
    }
#endif
    if (ISPUP(sp))
	s = fl_getpup_text(sp->extern_menu, sp->val);
    else
	s = (sp->val < 1 || sp->val > sp->numitems) ? 0 : sp->items[sp->val];
    return s;
}

const char *
fl_get_menu_item_text(FL_OBJECT * ob, int n)
{
    SPEC *sp = ((SPEC *) (ob->spec));
    const char *s;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_MENU))
    {
	Bark("GetMenuText", "%s is not Menu class", ob ? ob->label : "");
	return 0;
    }
#endif
    if (ISPUP(sp))
	s = fl_getpup_text(sp->extern_menu, n);
    else
	s = (n < 1 || n > sp->numitems) ? 0 : sp->items[n];
    return s;
}

unsigned
fl_get_menu_item_mode(FL_OBJECT * ob, int n)
{
    SPEC *sp;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_MENU))
    {
	Bark("GetMenuItemMode", "%s is not Menu class", ob ? ob->label : "");
	return 0;
    }
#endif

    sp = ob->spec;

    if (ISPUP(sp))
	return fl_getpup_mode(sp->extern_menu, n);
    else
	return (n > 0 && n <= sp->numitems) ? sp->mode[n] : 0;
}

void
fl_set_menu_popup(FL_OBJECT * ob, int pup)
{

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_MENU))
    {
	Bark("setmenuPup", "%s is not Menu class", ob ? ob->label : "");
	return;
    }
#endif
    ((SPEC *) (ob->spec))->extern_menu = pup;
    if (ob->type == FL_PULLDOWN_MENU)
	fl_setpup_shadow(pup, 0);
}

int
fl_set_menu_entries(FL_OBJECT * ob, FL_PUP_ENTRY * ent)
{
    int n;

    fl_clear_menu(ob);

    n = fl_newpup(0);
    fl_set_menu_popup(ob, fl_setpup_entries(n, ent));

    if (ob->type == FL_PULLDOWN_MENU)
    {
	fl_setpup_bw(n, -2);
	fl_setpup_shadow(n, 0);
    }

    return n;
}

int
fl_get_menu_popup(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    return ISPUP(sp) ? sp->extern_menu : -1;
}
