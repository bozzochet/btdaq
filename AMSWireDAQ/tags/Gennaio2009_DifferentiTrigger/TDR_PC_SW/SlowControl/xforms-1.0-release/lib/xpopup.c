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
 * $Id: xpopup.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao
 *  All rights reserved.
 *.
 *
 *
 * Implementation of pop-up menus in Xlib. Not quite fit the
 * model of forms library, but it is needed to make other things
 * work.
 *
 * These functionalities should be someday rewritten using 
 * forms construct rather than Xlib.
 *
 */
#if defined(F_ID) || defined(DEBUG)
char *fl_id_xpup = "$Id: xpopup.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

#include "forms.h"

#define ALWAYSROOT 1		/* true to use root as parent. not working */
#define FL_MAXPUP  32		/* default maximum pups    */
#define PADH       FL_PUP_PADH	/* space between items     */


/****************************************************************
 * pop up menu structure and some defaults
 ****************************************************************/

#define NSC          8		/* max hotkeys   */

typedef struct
{
    char *str;			/* label               */
    FL_PUP_CB icb;		/* call back           */
    long *shortcut;		/* shortcut keys       */
    int subm;			/* sub menu            */
    unsigned mode;		/* various attributes  */
    int ret;			/* %x stuff            */
    short ulpos;		/* hotkeys in label    */
    short radio;		/* radio entry. 0 mean no radio */
    short len;
}
MenuItem;

typedef struct
{
    char *title;		/* Menu title            */
    Window win;			/* menu window           */
    Window parent;		/* and its paranet       */
    Cursor cursor;		/* cursor for the pup    */
    GC shadowGC;		/* GC for the shadow     */
    GC pupGC1;			/* GC for maintext       */
    GC pupGC2;			/* GC for inactive text  */
    MenuItem *item[FL_MAXPUPI + 1];
    FL_PUP_CB mcb;		/* call back routine     */
    FL_PUP_ENTERCB enter_cb;	/* enter callback routine */
    void *enter_data;
    FL_PUP_ENTERCB leave_cb;	/* enter callback routine */
    void *leave_data;
    unsigned long event_mask;
    int x, y;			/* origin relative to root */
    unsigned int w, h;		/* total dimension       */
    short titleh;
    short nitems;		/* no. of item in menu   */
    short titlewidth;		/* title width           */
    short maxw;
    short noshadow;
    short bw;
    short lpad;
    short rpad;
    short padh;
    short cellh;
    short isEntry;		/* true if menu is setup via entry struct */
}
PopUP;

static void grab_both(PopUP *);
static void reset_radio(PopUP *, MenuItem *);
static int subreturn;

/*
 * Resources that control the fontsize and other things
 */
static int pfstyle = FL_BOLDITALIC_STYLE;
static int tfstyle = FL_BOLDITALIC_STYLE;

#ifdef __sgi
static int pfsize = FL_SMALL_FONT, tfsize = FL_SMALL_FONT;
#else
static int pfsize = FL_NORMAL_FONT, tfsize = FL_NORMAL_FONT;
#endif

static FL_COLOR pupcolor = FL_COL1, puptcolor = FL_BLACK;
static FL_COLOR checkcolor = FL_BLUE;
static int puplevel;
static int fl_maxpup = FL_MAXPUP;
static int pupbw = 2;

static PopUP *menu_rec;

static XFontStruct *pup_fs;	/* pop main text font */
static int pup_ascent, pup_desc;	/* font properties    */
static XFontStruct *tit_fs;	/* tit text font      */
static int tit_ascent, tit_desc;
static Cursor pup_defcursor;

/************ data struct maintanance ******************{**/
static void
init_pupfont(void)
{
    int junk;
    XCharStruct chs;

    if (!tit_fs)
    {
	tit_fs = fl_get_fntstruct(tfstyle, tfsize);
	XTextExtents(tit_fs, "qjQb", 4, &junk, &tit_ascent, &tit_desc, &chs);
    }

    if (!pup_fs)
    {
	pup_fs = fl_get_fntstruct(pfstyle, pfsize);
	XTextExtents(pup_fs, "qjQb", 4, &junk, &pup_ascent, &pup_desc, &chs);
    }
}

#define PADW           15	/* space on each side(> 16) with marks */

/* initialize a particular menu */
static void
init_pup(PopUP * m)
{
    m->mcb = 0;
    m->enter_cb = m->leave_cb = 0;
    m->w = m->h = m->maxw = 0;
    m->nitems = m->titlewidth = 0;
    m->parent = m->win = 0;
    m->noshadow = 0;
    m->bw = pupbw;
    m->title = 0;
    m->item[0] = 0;
    m->padh = PADH;
    if (!pup_defcursor)
	pup_defcursor = fl_get_cursor_byname(XC_sb_right_arrow);
    m->cursor = pup_defcursor;
    m->lpad = m->rpad = 8;
    init_pupfont();
    m->cellh = pup_ascent + pup_desc + 2 * m->padh;
    m->isEntry = 0;
}

static int
find_index(Window win)
{
    PopUP *p = menu_rec, *ps = p + fl_maxpup;
    int i;

    for (i = 0; p < ps; p++, i++)
    {
	if (!p->title && !p->item[0] && !p->parent)
	{
	    init_pup(p);
	    p->parent = win;
	    return i;
	}
    }

    M_err("defpup", "Exceeded FL_MAXPUP (%d)", fl_maxpup);
    fprintf(stderr, "Please check for leaks. Current allocated menus are:\n");

    for (i = 0; i < fl_maxpup; i++)
	fprintf(stderr, "\t%d: %s\n", i,
		menu_rec[i].title ? menu_rec[i].title : "Notitle");

    return -1;
}


static void convert_shortcut(const char *, const char *, MenuItem *, int);

#define M_TITLE     1
#define M_ERR       2

static void wait_for_close(Window);

static void
reset_max_width(PopUP * m)
{
    int i, w = 0, tw;
    MenuItem **item = m->item;
    char *t;

    if (!m->parent || m->nitems <= 0)
	return;

    for (i = 0; i < m->nitems; i++)
    {
	if ((tw = fl_get_string_widthTAB(pfstyle, pfsize,
					 item[i]->str, item[i]->len)) > w)
	    w = tw;
    }

    m->maxw = w;
    t = m->title ? m->title : "";
    m->titlewidth = XTextWidth(tit_fs, t, strlen(t));
    m->cellh = pup_ascent + pup_desc + 2 * m->padh;
}

#define HAS_BOX(m)   ((m)&(FL_PUP_CHECK|FL_PUP_RADIO|FL_PUP_BOX))

/* Parse the menu entries */

static int
parse_entry(int n, const char *str, va_list ap)
{
    PopUP *m = menu_rec + n;
    MenuItem **item = m->item + m->nitems;
    char *s, *val, *p, tmp[128], *tt;
    unsigned flags;

    if (n < 0 || n >= fl_maxpup || !str)
	return -1;

    s = fl_strdup(str);

    for (val = strtok(s, "|"); val && m->nitems < FL_MAXPUPI - 1;
	 val = strtok((char *) 0, "|"))
    {
	flags = 0;
	*item = (MenuItem *) fl_calloc(1, sizeof(**item));
	(*item)->ret = m->nitems + 1;
	(*item)->ulpos = -1;
	(*item)->subm = -1;
	tt = tmp + 1;
	*tt = 0;
	while ((p = strchr(val, '%')))
	{
	    int nc = *(p + 1);
	    *p = '\0';
	    if (!*tt)
		strcpy(tt, val);
	    val = p + 2;
	    if (nc == 'F')
		m->mcb = va_arg(ap, FL_PUP_CB);
	    else if (nc == 'e')
		m->enter_cb = va_arg(ap, FL_PUP_ENTERCB);
	    else if (nc == 'f')
		(*item)->icb = va_arg(ap, FL_PUP_CB);
	    else if (nc == 'm')
		(*item)->subm = va_arg(ap, int);
	    else if (nc == 'E')
		fl_setpup_entries(n, va_arg(ap, FL_PUP_ENTRY *));
	    else if (nc == 't')
		flags |= M_TITLE;
	    else if (nc == 'l')
		*--tt = '\010';
	    else if (nc == 'b')
		(*item)->mode |= FL_PUP_TOGGLE;
	    else if (nc == 'i')
		(*item)->mode |= FL_PUP_INACTIVE;
	    else if (nc == 'x')
	    {
		(*item)->ret = atoi(p + 2);
		for (; isdigit( ( int ) *val) ||
			 isspace( ( int ) *val); val++)
		    ;
	    }
	    else if (nc == 'r' || nc == 'R')
	    {
		(*item)->radio = atoi(p + 2);
		(*item)->mode |= FL_PUP_BOX;
		if (nc == 'R')
		    (*item)->mode |= FL_PUP_CHECK;
		for (; isdigit( ( int ) *val) ||
			 isspace( ( int ) *val); val++)
		    ;
	    }
	    else if (nc == 'B')
	    {
		(*item)->mode |= FL_PUP_CHECK | FL_PUP_TOGGLE;
	    }
	    else if (nc == 'd')
	    {
		(*item)->mode |= FL_PUP_GREY;
	    }
	    else if (nc == 'h' || nc == 's')
	    {
		char *sc = va_arg(ap, char *);
		M_info(0, "shortcut=%s for %s", sc, tt);
		convert_shortcut(sc, tt, *item, NSC);
	    }
	    else if (nc == '%')
		strcat(tt, "%");
	    else
	    {
		flags |= M_ERR;
		M_err("ParsePup", "Unknown sequence %%%c", nc);
	    }
	}

	if (flags & M_ERR)
	{
	    M_err("PupParse", "Error while parsing pup entry");
	    continue;
	}

	if (HAS_BOX((*item)->mode))
	    m->lpad = PADW + 2 * FL_abs(pupbw);

	if ((*item)->subm >= 0)
	    m->rpad = PADW + 2 * FL_abs(pupbw);

	if (*tt == 0)
	    tt = val;

	if (flags & M_TITLE)
	{
	    m->title = fl_strdup(tt);
	    m->titlewidth = XTextWidth(tit_fs, tt, strlen(tt));
	}
	else
	{
	    int w, len;

	    (*item)->str = fl_strdup(tt);
	    len = (*item)->len = strlen(tt);
	    if ((w = fl_get_string_widthTAB(pfstyle, pfsize, tt, len)) > m->maxw)
		m->maxw = w;

	    m->nitems++;
	    item++;

	}

	if (flags & (M_ERR | M_TITLE))
	{
	    fl_free(*item);
	    *item = 0;
	}
    }

    if (val)
	M_err("Xpup", "too many menu items. Max=%d", FL_MAXPUPI);


    fl_free(s);
    return 0;
}

/************** End of data struct maint. *********}***************/
static void
close_pupwin(PopUP * pup)
{
    if (pup->win)
    {
	XDestroyWindow(flx->display, pup->win);
	wait_for_close(pup->win);
	pup->win = 0;
    }
}

/* initialize the menu system. Must be called first. Made defpup/newpup
 * etc. auto call fl_init_pup (instead of letting fl_initialize to call it)
 * and we save about ~25k in exe size for app not using pups
 */
void
fl_init_pup(void)
{
    if (!menu_rec)
    {
	menu_rec = fl_calloc(fl_maxpup, sizeof(*menu_rec));
	fl_setpup_default_fontsize(fl_cntl.pupFontSize);
    }
}

int
fl_setpup_default_fontsize(int size)
{
    PopUP *pup, *pups;
    int opfsize = pfsize;

    if (size <= 0)
	return opfsize;

    fl_init_pup();
    pup = menu_rec;

    pfsize = size;
    tfsize = size;

    pup_fs = tit_fs = 0;

    if (!flx->display)
	return opfsize;

    init_pupfont();

    for (pups = pup + fl_maxpup; pup < pups; pup++)
    {
	reset_max_width(pup);
	if (pup->win)
	    close_pupwin(pup);
    }

    return opfsize;
}



int
fl_setpup_default_fontstyle(int style)
{
    PopUP *pup, *pups;
    int opfstyle;

    if (style < 0)
	return pfstyle;

    fl_init_pup();
    pup = menu_rec;

    opfstyle = pfstyle;
    pfstyle = style;
    tfstyle = style;
    pup_fs = tit_fs = 0;

    if (!flx->display)
	return opfstyle;

    init_pupfont();

    for (pups = menu_rec + fl_maxpup; pup < pups; pup++)
    {
	reset_max_width(pup);
    }

    return opfstyle;
}

void
fl_setpup_default_color(FL_COLOR fg, FL_COLOR bg)
{
    pupcolor = fg;
    puptcolor = bg;
}

void
fl_setpup_default_checkcolor(FL_COLOR col)
{
    checkcolor = col;
}

/********************************************************************
 * Public routines
 ****************************************************************{***/

/*** Allocate a new PopUP ID **/
int
fl_newpup(Window win)
{

    fl_init_pup();

    if (puplevel)
    {
	M_warn("Defpup", "Inconsistent puplevel %d", puplevel);
	puplevel = 0;
    }

    if (win == 0)
	win = fl_root;

    /* if not private colormap, it does not matter who the popup's parent is
       and root probably makes more sense */

#if ALWAYSROOT
    return find_index(fl_root);
#else
    {
	FL_State *fs = fl_state + fl_vmode;
	int nrt = (fs->pcm ||
	     (fl_visual(fl_vmode) != DefaultVisual(flx->display, fl_screen)));
	return find_index(nrt ? win : fl_root);
    }
#endif
}

/*** Add pop-up entries ***/

int
fl_addtopup(int n, const char *str,...)
{
    va_list ap;

    if (n >= 0 && n < fl_maxpup)
    {
#if (FL_DEBUG >= M_DEBUG)
	{
	    char *q = fl_strdup(str), *p;
	    while ((p = strchr(q, '%')))
		*p = 'P';	/* % can cause problems */
	    M_info("addtopup", q);
	    fl_free(q);
	}
#endif
	va_start(ap, str);
	parse_entry(n, str, ap);
	va_end(ap);
	return n;
    }
    return -1;
}

/*** Allocate PopUP ID and optionally set all entries ***/
int
fl_defpup(Window win, const char *str,...)
{
    int n;
    va_list ap;

    if ((n = fl_newpup(win)) < 0)
    {
	fl_error("XPopUP", "Can't Allocate");
	return n;
    }

    va_start(ap, str);
    parse_entry(n, str, ap);
    va_end(ap);

#if (FL_DEBUG > ML_WARN)
    if (fl_cntl.debug > 1)
    {
	PopUP *m = menu_rec + n;
	int i;
	fprintf(stderr, "Defpup for string: %s\n", str);
	for (i = 0; i < m->nitems; i++)
	    fprintf(stderr, "%i %s ret=%d %s %s\n",
		    i, m->item[i]->str, m->item[i]->ret,
		    m->item[i]->shortcut ? "shortcut" : "",
		    m->item[i]->icb ? "callback" : "");
    }
#endif
    return n;
}

/* check to see if the requested value exists in popup m */
static MenuItem *
ind_is_valid(PopUP * m, register int ind)
{
    register MenuItem **is = m->item, **ise, *item = 0;


    for (ise = is + m->nitems; is < ise && !item; is++)
    {
	if ((*is)->ret == ind)
	    item = *is;
	else if ((*is)->subm >= 0)
	    item = ind_is_valid(menu_rec + (*is)->subm, ind);
    }
    return item;
}


static MenuItem *
requested_item_isvalid(const char *where, int nm, int ni)
{
    if (nm < 0 || nm >= fl_maxpup)
    {
	M_err(where, "Bad popup index %d", nm);
	return 0;
    }
    return ind_is_valid(menu_rec + nm, ni);
}

/*** change attributes of a popup item ***/
int
fl_setpup_mode(int nm, int ni, unsigned int mode)
{
    MenuItem *item;

    if ((item = requested_item_isvalid("setpup", nm, ni)))
    {
	if ((item->mode = mode) & FL_PUP_CHECK)
	    item->mode |= FL_PUP_BOX;
	if (item->mode & FL_PUP_RADIO)
	{
	    item->mode |= FL_PUP_BOX;
	    if (!item->radio)
		item->radio = 255;
	}

	if (HAS_BOX(mode))
	    menu_rec[nm].lpad = PADW + 2 * FL_abs(pupbw);
    }
    return 0;
}

#define AltMask  FL_ALT_VAL

static void
convert_shortcut(const char *sc, const char *str, MenuItem * item, int n)
{
    if (!item->shortcut)
	item->shortcut = fl_calloc(1, sizeof(*(item->shortcut)) * NSC);
    item->ulpos = fl_get_underline_pos(str, sc) - 1;
    fl_convert_shortcut(sc, item->shortcut);
    if (sc[0] == '&')
	M_info("sc=%s keysym=%ld\n", sc, item->shortcut[0]);
}

static void draw_only(PopUP *);
static void draw_item(PopUP *, int, int);

#define PADTITLE       14	/* extran space for title  */
#define SHADE           6
#define CHECKW          6	/* check box size          */

#define TITLEH         (tit_ascent + tit_desc + PADTITLE)

#define BLOCK


#ifndef BLOCK
static long old_delta;
static int
popclose(XEvent * xev, void *data)
{
    if (xev->type == DestroyNotify)
	fl_context->idle_delta = old_delta;
    return 0;
}

#endif

static void
wait_for_close(Window win)
{
#ifdef BLOCK
    long emask = 0x00ffffff;
    XEvent xev;
    XSync(flx->display, 0);
    while (XCheckWindowEvent(flx->display, win, emask, &xev))
	fl_xevent_name("PopClose", &xev);
#else
    fl_add_event_callback(win, DestroyNotify, popclose, 0);
    fl_add_event_callback(win, UnmapNotify, popclose, 0);
    old_delta = fl_context->idle_delta;
    fl_context->idle_delta = 10;
#endif
}

/****************************************************************
 * Global routine of doing pop-ups. Never returns unless user
 * does something with the pointer. For "hanging" pop-ups, a
 * pointer & focus grab will be activated and released upon returning
 ****************************************************************/
/* since requested item might be inactive, search for next active
   item if current one is not
 */
static int
get_valid_entry(PopUP * m, int target, int dir)
{
    if (target < 1)
	target = dir < 0 ? m->nitems : 1;
    if (target > m->nitems)
	target = dir < 0 ? m->nitems : 1;

    for (; target > 0 && target <= m->nitems; target += dir)
	if (!(m->item[target - 1]->mode & (FL_PUP_GREY | FL_PUP_INACTIVE)))
	    return target;

    /* wrap */
    if (target < 1)
	target = dir < 0 ? m->nitems : 1;
    if (target > m->nitems)
	target = dir < 0 ? m->nitems : 1;

    for (; target > 0 && target <= m->nitems; target += dir)
	if (!(m->item[target - 1]->mode & (FL_PUP_GREY | FL_PUP_INACTIVE)))
	    return target;

    M_err("PopUp", "No valid entries among total of %d", m->nitems);
    return 0;
}

#define alt_down    (metakey_down(keymask) != 0)

static int
handle_shortcut(PopUP * m, KeySym keysym, unsigned keymask)
{
    MenuItem **mi = m->item;
    int i, j;
    int sc, alt;

    for (i = 0; i < m->nitems; i++)
    {
	if (!(mi[i]->mode & (FL_PUP_GREY | FL_PUP_INACTIVE)) && mi[i]->shortcut)
	    for (j = 0; j < NSC && mi[i]->shortcut[j]; j++)
	    {
		sc = mi[i]->shortcut[j];
		alt = (sc & AltMask) == AltMask;
		sc &= ~AltMask;
		if (sc == keysym && !(alt ^ alt_down))
		    return i + 1;
	    }
    }
    return 0;
}

static int
handle_submenu(PopUP * m, MenuItem * item, int *val)
{
    int c;

    if (!(item->mode & (FL_PUP_GREY | FL_INACTIVE)) && item->subm >= 0)
    {
	fl_setpup_position(m->x + m->w - 25, m->y + m->cellh * (*val) - 20);
	if ((c = fl_dopup(item->subm)) <= 0)
	    grab_both(m);
	else
	{
	    *val = c;
	    subreturn = c;
	    return 1;
	}
    }
    return 0;
}

/*
 * keyboard. Also checks shortcut
 */
static int
pup_keyboard(XKeyEvent * xev, PopUP * m, int *val)
{
    KeySym keysym = NoSymbol;
    char buf[16];
    int i, oldval = *val;

    XLookupString(xev, buf, sizeof(buf), &keysym, 0);

    if (IsHome(keysym))
    {
	draw_item(m, *val, FL_FLAT_BOX);
	*val = get_valid_entry(m, 1, -1);
	draw_item(m, *val, FL_UP_BOX);
    }
    else if (IsEnd(keysym))
    {
	draw_item(m, *val, FL_FLAT_BOX);
	*val = get_valid_entry(m, m->nitems, 1);
	draw_item(m, *val, FL_UP_BOX);
    }
    else if (IsUp(keysym))
    {
	draw_item(m, *val, FL_FLAT_BOX);
	*val = get_valid_entry(m, (*val) - 1, -1);
	draw_item(m, *val, FL_UP_BOX);
    }
    else if (IsDown(keysym))
    {
	draw_item(m, *val, FL_FLAT_BOX);
	*val = get_valid_entry(m, *val + 1, 1);
	draw_item(m, *val, FL_UP_BOX);
    }
    else if (IsRight(keysym))
    {
	if (*val > 0 && *val <= m->nitems)
	{
	    if (handle_submenu(m, m->item[*val - 1], val))
		keysym = XK_Return;
	}
    }
    else if (IsLeft(keysym))
    {
#if 0
	if (puplevel > 1)	/* not allow closing the root menu */
#endif
	{
	    *val = -1;
	    keysym = XK_Escape;
	}
    }
    else if (keysym == XK_Escape || keysym == XK_Cancel)
    {
	draw_item(m, *val, FL_FLAT_BOX);
	*val = -1;
    }
    else if (keysym == XK_Return)
    {
	if (*val > 0 && *val <= m->nitems)
	    handle_submenu(m, m->item[*val - 1], val);
    }
    else
    {
	if ((i = handle_shortcut(m, keysym, xev->state)))
	{
	    *val = i;
	    handle_submenu(m, m->item[*val - 1], val);
	    keysym = XK_Return;
	}
    }

    if (oldval != *val && (m->enter_cb || m->leave_cb))
    {
	if (oldval > 0 && oldval <= m->nitems && m->leave_cb)
	    m->leave_cb(m->item[oldval - 1]->ret, m->leave_data);
	if (*val > 0 && *val <= m->nitems && m->enter_cb)
	    m->enter_cb(m->item[*val - 1]->ret, m->enter_data);
    }

    return (keysym == XK_Escape || keysym == XK_Return || keysym == XK_Cancel);
}

/* given mouse position and a menu, return if the mouse is on
   the title bar
 */
static int
is_on_title(PopUP * m, int mx, int my)
{
    int val;

    val = (mx < m->x || mx > m->x + m->w) ? -1 : (my - m->y + m->titleh) / m->cellh;
    if (val == 0 && (!m->title || !*m->title))
	val = (mx > m->x + m->w / 3 || mx < (m->x - 2)) ? -1 : 0;

    return val == 0 ? 1 : 0;
}

/*
 * mouse moved. val is set to the item number (not value) upon return
 */
static MenuItem *
handle_motion(PopUP * m, int mx, int my, int *val)
{
    int cval;
    MenuItem *item = 0;
    int titleh = m->titleh;
    static MenuItem *lastitem;

    cval = (mx < 0 || mx > m->w) ? -1 : ((my - titleh + m->cellh) / m->cellh);

    /* if released on title bar, cval is zero. However, if there is no title,
       change cval to -1 (invalid) if "too right" */

    if (cval == 0 && (!m->title || !*m->title))
	cval = (mx > m->w / 3) ? -1 :
	    ((puplevel > 1 && mx < m->x) ? -1 : 0);
    else if (cval > m->nitems || cval < 0)
	cval = -1;
    else if (cval > 0)
	item = m->item[cval - 1];

    if (cval != *val)
    {
	draw_item(m, *val, FL_FLAT_BOX);
	draw_item(m, cval, FL_UP_BOX);
	*val = cval;
    }

    item = (item && !(item->mode & (FL_PUP_GREY | FL_PUP_INACTIVE))) ? item : 0;

    if (lastitem && item != lastitem && m->leave_cb)
	m->leave_cb(lastitem->ret, m->leave_data);

    if (item && m->enter_cb && item != lastitem)
	m->enter_cb(item->ret, m->enter_data);

    lastitem = item;

    return item;
}

/*
 * Interaction routine. If mouse is released on the title bar,
 * consider its a "hanging" pop-up request else return
 */
/* #define USEEVENT */
static int
pup_interact(PopUP * m)
{
    XEvent ev;
    int val = 0, timeout, done, timer_cnt = 0;
    MenuItem *item;

    fl_reset_time(FL_PUP_T);
    m->event_mask |= KeyPressMask;
    ev.xmotion.time = 0;

    for (done = timeout = 0; !done && !timeout;)
    {
	timeout = (fl_time_passed(FL_PUP_T) > 40.0f);
	if (!XCheckWindowEvent(flx->display, m->win, m->event_mask, &ev))
	{
    	    XEvent xev;
	    if (XCheckTypedEvent(flx->display, ButtonRelease, &xev))
	    {
		/* fl_print_xevent_name("pop",&xev); */
	        /* terminate popup if not on the title bar */
	        if (!is_on_title(m, xev.xbutton.x_root, xev.xbutton.y_root))
	        {
		    val = -1;
		    done = 1;
	        }
	    }

	    fl_watch_io(fl_context->io_rec, fl_context->idle_delta);

	    /* mouse pos do not matter. However if idle is 1, need to pass a
	       valid event. */

	    if ((timer_cnt++ % 10) == 0)
	    {
		FL_Coord x, y;
		unsigned km;
		timer_cnt = 0;
		fl_get_win_mouse(m->win, &x, &y, &km);
		/* only set some of the field in the synthetic event */
		ev.type = MotionNotify;
		ev.xmotion.send_event = 1;
		ev.xmotion.is_hint = 0;
		ev.xmotion.display = flx->display;
		ev.xmotion.x = x;
		ev.xmotion.y = y;
		ev.xmotion.state = km;
		ev.xmotion.window = m->win;
		ev.xmotion.time += 200;
	    }

	    fl_handle_automatic(&ev, 1);
	    continue;
	}

	timer_cnt = 0;
	fl_winset(m->win);

	fl_xevent_name("PopUP", &ev);

	/* this could be a little better is enter/leave is checked and a loop
	   over all valid pup is done */
	switch (ev.type)
	{
	case Expose:
	    /* need to redraw form first */
	    if (fl_check_forms() == FL_EVENT)
		fl_XNextEvent(&ev);
	    draw_only(m);
	    break;

	case MotionNotify:
	    fl_compress_event(&ev, ButtonMotionMask);
	    /* FALL THROUGH */
	case ButtonPress:
	    /* taking adv. of xbutton.x == xcrossing.x */
	    item = handle_motion(m, ev.xbutton.x, ev.xbutton.y, &val);
	    if (item && item->subm >= 0 && ev.xbutton.x > (m->w - 20))
		done = handle_submenu(m, item, &val);
	    else if (puplevel > 1 && val < 0)
		done = (ev.xmotion.x < 0);
	    break;

	case ButtonRelease:
	    item = handle_motion(m, ev.xbutton.x, ev.xbutton.y, &val);
	    if (item && item->subm >= 0)
		done = handle_submenu(m, item, &val);
	    else
		done = (val != 0);
	    break;

	case KeyPress:
	    done = pup_keyboard((XKeyEvent *) & ev, m, &val);
	    break;
	case UnmapNotify:	/* must be by external routine */
	    done = 1;
	    val = -1;
	    break;
	default:
	    break;
	}
    }

    return timeout ? -1 : val;
}

static void
grab_both(PopUP * m)
{
    unsigned int evmask = m->event_mask;

    /* get rid of all non-pointer events in event_mask */
    evmask &= ~(ExposureMask | KeyPressMask);
    XFlush(flx->display);
    fl_msleep(30);
    XChangeActivePointerGrab(flx->display, evmask, m->cursor, CurrentTime);

    fl_winset(m->win);

    /* do both pointer and keyboard grab */
    if (XGrabPointer(flx->display, m->win, True, evmask, GrabModeAsync,
		GrabModeAsync, None, m->cursor, CurrentTime) != GrabSuccess)
	M_err("dopup", "Can't grab pointer");

    XGrabKeyboard(flx->display, m->win, False, GrabModeAsync,
		  GrabModeAsync, CurrentTime);
}

int
fl_dopup(int n)
{
    PopUP *m = menu_rec + n;
    int val = 0;
    MenuItem *item = 0;

    if (n < 0 || n >= fl_maxpup)
    {
	M_err("dopup", "bad pupID: %d\n", n);
	return -1;
    }

    subreturn = -1;

    if (puplevel == 0)
	fl_context->pup_id = n;

    puplevel++;
    fl_showpup(n);
    grab_both(m);

    /* pup_interact returns the item number */
    val = pup_interact(m);

    if (m->win)
    {
	XUngrabPointer(flx->display, CurrentTime);
	XUngrabKeyboard(flx->display, CurrentTime);
	XUnmapWindow(flx->display, m->win);
	wait_for_close(m->win);
	if (m->win == fl_context->pup_win)
	    fl_context->pup_win = 0;
    }
    else
    {
	M_err("dopup", "WinClosed");
    }

    if (puplevel > 1)
    {
	/* need to remove all MotionNotify otherwise wrong coord */
	XEvent xev;
	while (XCheckMaskEvent(flx->display, ButtonMotionMask, &xev))
	    fl_xevent_name("SyncFlush", &xev);
    }

    /* handle call back if any  */
    puplevel--;
    if (val > 0 && val <= m->nitems &&
	(subreturn < 0 || (subreturn > 0 && puplevel > 0)))
    {
	item = m->item[val - 1];
	if ((item->mode & (FL_PUP_GREY | FL_PUP_INACTIVE)))
	    return -1;

	if (item->subm >= 0)
	    return val;

	if (item->radio)
	    reset_radio(m, item);
	else if (item->mode & FL_PUP_CHECK)
	{
	    item->mode &= ~FL_PUP_CHECK;
	    item->mode |= FL_PUP_BOX;
	}
	else if (item->mode & FL_PUP_BOX)
	{
	    item->mode |= FL_PUP_CHECK;
	}

	val = item->ret;
	if (item->icb)
	    val = item->icb(val);
	if (m->mcb)
	    val = m->mcb(val);
    }

    if (puplevel <= 0)
	fl_context->pup_id = -1;

#if 0
    fprintf(stderr, "val=%d sunreturnval=%d\n", val, subreturn);
    if (subreturn > 0 && submenu && subreturn <= submenu->nitems)
    {
	fprintf(stderr, "subitems=%d\n", submenu->nitems);
	item = submenu->item[subreturn - 1];
	if ((item->mode & (FL_PUP_GREY | FL_PUP_INACTIVE)))
	    return -1;
	val = item->ret;
    }
    fprintf(stderr, "final val=%d sunreturnval=%d\n", val, subreturn);
#endif
    if (subreturn > 0)
	val = subreturn;


    return val;
}

void
fl_freepup(int n)
{
    PopUP *p = menu_rec + n;
    int i;

    if (n < 0 || n >= fl_maxpup)
	return;

    if (!p->parent)
    {
	M_warn("freepup", "freeing a unallocated/free'ed popup %d\n", n);
	return;
    }

    for (i = 0; i < FL_MAXPUPI; i++)
    {

	if (p->item[i])
	{
	    if (p->item[i]->subm >= 0 && p->isEntry)
		fl_freepup(p->item[i]->subm);
	    fl_safe_free(p->item[i]->str);
	    fl_safe_free(p->item[i]->shortcut);
	}
	fl_safe_free(p->item[i]);
    }

    p->parent = 0;

    fl_safe_free(p->title);

    close_pupwin(p);
}

/*
 * Some convenience functions
 */
void
fl_setpup_shortcut(int nm, int ni, const char *sc)
{
    MenuItem *item;

    if (!sc || !(item = requested_item_isvalid("pupshortcut", nm, ni)))
	return;
    convert_shortcut(sc, item->str, item, NSC);
}

FL_PUP_CB
fl_setpup_menucb(int nm, FL_PUP_CB cb)
{
    PopUP *m = menu_rec + nm;
    FL_PUP_CB oldcb = 0;

    if (nm >= 0 && nm < fl_maxpup && m->parent)
    {
	oldcb = m->mcb;
	m->mcb = cb;
    }
    return oldcb;
}

FL_PUP_ENTERCB
fl_setpup_entercb(int nm, FL_PUP_ENTERCB cb, void *data)
{
    FL_PUP_ENTERCB oldcb = 0;
    PopUP *m;
    int n, subm;

    if (nm >= 0 && nm < fl_maxpup)
    {
	m = menu_rec + nm;
	oldcb = m->enter_cb;
	m->enter_cb = cb;
	m->enter_data = data;
	for (n = 0; n < m->nitems; n++)
	{
	    if ((subm = m->item[n]->subm) >= 0 && !menu_rec[subm].enter_cb)
		fl_setpup_entercb(subm, cb, data);
	}
    }

    return oldcb;
}

FL_PUP_LEAVECB
fl_setpup_leavecb(int nm, FL_PUP_LEAVECB cb, void *data)
{
    FL_PUP_LEAVECB oldcb = 0;
    PopUP *m;
    int n, subm;

    if (nm >= 0 && nm < fl_maxpup)
    {
	m = menu_rec + nm;
	oldcb = m->leave_cb;
	m->leave_cb = cb;
	m->leave_data = data;
	for (n = 0; n < m->nitems; n++)
	{
	    if ((subm = m->item[n]->subm) >= 0 && !menu_rec[subm].enter_cb)
		fl_setpup_leavecb(subm, cb, data);
	}
    }

    return oldcb;
}

FL_PUP_CB
fl_setpup_itemcb(int nm, int ni, FL_PUP_CB cb)
{
    MenuItem *item;
    FL_PUP_CB oldcb = 0;

    if ((item = requested_item_isvalid("pupitemcb", nm, ni)))
    {
	oldcb = item->icb;
	item->icb = cb;
    }
    return oldcb;
}

void
fl_setpup_title(int nm, const char *title)
{
    PopUP *m = menu_rec + nm;

    if (nm >= 0 && nm < fl_maxpup && title)
    {
	if (m->title)
	    fl_free(m->title);
	m->title = fl_strdup(title);
	m->titlewidth = XTextWidth(tit_fs, m->title, strlen(m->title));
    }
}

Cursor
fl_setpup_cursor(int nm, int cursor)
{
    PopUP *m = menu_rec + nm;
    Cursor old = 0;

    if (nm >= 0 && nm < fl_maxpup)
    {
	old = m->cursor;
	m->cursor = cursor ? fl_get_cursor_byname(cursor) : pup_defcursor;
    }
    return old;
}

Cursor
fl_setpup_default_cursor(int cursor)
{
    Cursor old = pup_defcursor;
    pup_defcursor = fl_get_cursor_byname(cursor);
    return old;
}

void
fl_setpup_pad(int n, int padw, int padh)
{
    PopUP *m = menu_rec + n;

    if (n >= 0 && n < fl_maxpup)
    {
	m->padh = padh;
	m->rpad = m->lpad = padw;
	m->cellh = pup_ascent + pup_desc + 2 * m->padh;
    }
}

static void
recurse(PopUP * m, void (*set) (int, int), int val)
{
    int i;

    for (i = 0; i < m->nitems; i++)
	if (m->item[i]->subm)
	    set(m->item[i]->subm, val);
}

void
fl_setpup_shadow(int n, int y)
{
    PopUP *m = menu_rec + n;
    int i;

    if (n >= 0 && n < fl_maxpup)
    {
	m->noshadow = !y;
	for (i = 0; i < m->nitems; i++)
	    if (m->item[i]->subm)
		fl_setpup_shadow(m->item[i]->subm, y);
    }
}


void
fl_setpup_bw(int n, int bw)
{
    PopUP *m = menu_rec + n;
    int i;

    if (n >= 0 && n < fl_maxpup)
    {
	m->bw = bw;
	for (i = 0; i < m->nitems; i++)
	    if (m->item[i]->subm)
		fl_setpup_bw(m->item[i]->subm, bw);
    }
}

void
fl_setpup_softedge(int n, int y)
{
    PopUP *m = menu_rec + n;

    if (n >= 0 && n < fl_maxpup)
    {
	m->bw = y ? -FL_abs(m->bw) : FL_abs(m->bw);
	recurse(m, fl_setpup_softedge, y);
    }
}

static void
reset_radio(PopUP * m, MenuItem * item)
{
    MenuItem **ii;
    for (ii = m->item; ii < (m->item + m->nitems); ii++)
	if ((*ii)->radio == item->radio)
	    (*ii)->mode &= ~FL_PUP_CHECK;
    item->mode |= FL_PUP_CHECK;
}

void
fl_setpup_selection(int nm, int ni)
{
    MenuItem *item;
    if ((item = requested_item_isvalid("pupselection", nm, ni)) && item->radio)
	reset_radio(menu_rec + nm, item);
}

void
fl_setpup_submenu(int m, int i, int subm)
{
    MenuItem *item;

    if ((item = requested_item_isvalid("subm", m, i)))
    {
	menu_rec[m].rpad = PADW + 2 * FL_abs(pupbw);
	item->subm = subm;
    }
}


/**** End of PUBLIC routines for pop-ups *******************}*/


/**** ALL drawing routines */

/* draw item. Index starts from 1 */
static void
draw_item(PopUP * m, int i, int style)
{
    int j = i - 1;
    int bw = FL_abs(m->bw);
    int x, y = m->titleh + m->cellh * j + 1, dy = m->cellh - 2;
    char *str;
    MenuItem *item;
    GC gc;

    if (j < 0 || j >= m->nitems)
	return;

    if (bw > 3)
	x = bw * 1.5;
    else
	x = bw * 2;

    item = m->item[j];
    gc = (item->mode & FL_PUP_GREY) ? m->pupGC2 : m->pupGC1;
    str = item->str;

    if (!(item->mode & FL_PUP_GREY))
	fl_drw_box(style, x - 1, y, m->w - 2 * x + 2 + (m->bw == -2),
		   dy, pupcolor, m->bw == -1 ? -1 : -2);

    fl_winset(m->win);

    if ((item->mode & FL_PUP_BOX) && !(item->mode & FL_PUP_CHECK))
    {
	int w = item->radio ? CHECKW : (CHECKW + 2);
	int bbw = item->radio ? -2 : -1;
	(item->radio ? fl_drw_checkbox : fl_drw_box)
	    (FL_UP_BOX, x + 3, (y + (dy - CHECKW) / 2),
	     w, w, pupcolor, bbw);
    }

    if (item->mode & FL_PUP_CHECK)
    {
	int w = item->radio ? CHECKW : (CHECKW + 2);
	int bbw = item->radio ? -3 : -2;
	(item->radio ? fl_drw_checkbox : fl_drw_box)
	    (FL_DOWN_BOX, x + 3, (y + (dy - CHECKW) / 2),
	     w, w, fl_depth(fl_vmode) == 1 ? FL_BLACK : checkcolor, bbw);
    }

    /* show text */
    j = str[0] == '\010';
    fl_drw_stringTAB(m->win, gc,
		     m->lpad, y + m->padh + pup_ascent - 1,
		     pfstyle, pfsize, str + j, strlen(str) - j, 0);

    /* do underline */
    if (item->ulpos >= 0)
    {
	XRectangle *xr;
	xr = fl_get_underline_rect(pup_fs, m->lpad,
				   y + m->padh + pup_ascent - 1,
				   str, item->ulpos);
	XFillRectangle(flx->display, m->win, gc,
		       xr->x, xr->y, xr->width, xr->height);
    }

    if (j)
	fl_draw_symbol("@DnLine", x, y + dy, m->w - 2 * x, 1, FL_COL1);

    if (item->subm >= 0)
	fl_draw_symbol((style == FL_UP_BOX &&
			!(item->mode & (FL_PUP_GREY | FL_PUP_INACTIVE))) ?
		       "@DnArrow" : "@UpArrow",
		       m->w - m->rpad + 1, y + dy / 2 - 7, 16, 16, FL_BLACK);
}

static void
draw_title(Display * d, Drawable w, int x, int y, char *s, int n)
{

    if (!s || !*s)
	return;
#if 0
    fl_drw_text(FL_ALIGN_CENTER, x - 2, y - 5,
		XTextWidth(pup_fs, s, strlen(s)),
		0, FL_SLATEBLUE, tfsize,
		tfstyle + FL_EMBOSSED_STYLE, s);
#else
    fl_set_font(tfstyle, tfsize);
    fl_textcolor(puptcolor);
    XDrawString(d, w, flx->textgc, x - 1, y - 1, s, n);
    XDrawString(d, w, flx->textgc, x, y - 1, s, n);
    XDrawString(d, w, flx->textgc, x + 1, y - 1, s, n);
    XDrawString(d, w, flx->textgc, x - 1, y, s, n);
    XDrawString(d, w, flx->textgc, x + 1, y, s, n);
    XDrawString(d, w, flx->textgc, x - 1, y + 1, s, n);
    XDrawString(d, w, flx->textgc, x, y + 1, s, n);
    XDrawString(d, w, flx->textgc, x + 1, y + 1, s, n);
    fl_textcolor(FL_WHITE);
    XDrawString(d, w, flx->textgc, x, y, s, n);
#endif
}

/*
 * Instead of poping up the menu at mouse location, use externally
 * set position. Good for programmatical pop-ups
 */
static int extpos;
static FL_Coord extx, exty;
void
fl_setpup_position(int x, int y)
{
    extpos = !(x == -1 && y == -1);
    extx = x;
    exty = y;
}

static void
draw_only(PopUP * m)
{
    int i;

    flx->win = m->win;

    if (m->title)
	m->titleh = TITLEH;
    else
	m->titleh = m->padh;

    if (!m->noshadow)
    {
/** create the shadow  ***/
	XFillRectangle(flx->display, m->win, m->shadowGC, m->w, SHADE, SHADE, m->h);
	XFillRectangle(flx->display, m->win, m->shadowGC,
		       SHADE, m->h, m->w - SHADE, SHADE);
    }

/*** make the popup box  ***/
    fl_drw_box(FL_UP_BOX, 0, 0, m->w, m->h, pupcolor, m->bw);

/*** title box ***/
    if (m->title)
    {
	fl_drw_box(FL_FRAME_BOX, 3, 3, m->w - 6, m->titleh - 6, pupcolor, 1);

	draw_title(flx->display, m->win, (m->w - m->titlewidth) / 2,
		   PADTITLE / 2 + tit_ascent, m->title, strlen(m->title));
    }

    for (i = 1; i <= m->nitems; i++)
	draw_item(m, i, FL_FLAT_BOX);
}

void
fl_showpup(int n)
{
    XEvent ev;
    int x, y;
    FL_Coord px = 1, py = 1, pw = fl_scrw, ph = fl_scrh, mw, mh;
    unsigned int kmask;
    XGCValues xgcv;
    PopUP *m = menu_rec + n;

    if (n < 0 || n >= fl_maxpup)
    {
	fprintf(stderr, "bad pupID: %d\n", n);
	return;
    }

    if (m->title)
	m->titleh = TITLEH;
    else
	m->titleh = m->padh;

    if (!m->win)
    {
	int bw = 0, w, h;
	XSetWindowAttributes xswa;
	unsigned long wmask;
	unsigned depth = fl_depth(fl_vmode);
	Visual *visual = fl_visual(fl_vmode);

	m->maxw = FL_max(m->titlewidth, m->maxw);
	m->w = m->maxw + m->rpad + m->lpad;
	m->h = m->nitems * m->cellh + m->titleh + 1 + (m->padh > 1);
	m->h += 2 * (FL_abs(m->bw) > 2);

	m->event_mask = (ExposureMask |
			 ButtonPressMask | ButtonReleaseMask |
			 ButtonMotionMask | OwnerGrabButtonMask |
			 PointerMotionHintMask |
			 StructureNotifyMask |	/* for UnmapNotify */
#if 1
			 EnterWindowMask |
#endif
			 KeyPressMask);

	xswa.event_mask = m->event_mask;
	xswa.save_under = True;
	xswa.backing_store = WhenMapped;	/* fl_cntl.backingStore; */
	xswa.cursor = m->cursor;	/* fl_get_cursor_byname(XC_sb_right_arrow)); 








					 */
	wmask = CWEventMask | CWSaveUnder | CWCursor | CWBackingStore;

	xswa.border_pixel = 0;
	wmask |= CWBorderPixel;

	xswa.colormap = fl_colormap(fl_vmode);
	wmask |= CWColormap;

	/* set transient hint does not do the trick if parent is rootwin */
	if (m->parent == fl_root)
	{
	    xswa.override_redirect = True;
	    wmask |= CWOverrideRedirect;
	}

	/* don't bother others */
	xswa.do_not_propagate_mask = ButtonPress | ButtonRelease | KeyPress;
	wmask |= CWDontPropagate;

#if ALWAYSROOT
	if (m->parent == fl_root && (fl_state[fl_vmode].pcm ||
	     (fl_visual(fl_vmode) != DefaultVisual(flx->display, fl_screen))))
	{
	    xswa.colormap = fl_colormap(fl_vmode);
	    wmask |= CWColormap;
	}
#endif

	w = m->w;
	h = m->h;

	if (!m->noshadow)
	{
	    w += SHADE;
	    h += SHADE;
	}

	m->win = XCreateWindow(flx->display, m->parent,
			       0, 0, w, h, bw,
			       depth, InputOutput, visual,
			       wmask, &xswa);


	XSetTransientForHint(flx->display, m->win, m->parent);
	XStoreName(flx->display, m->win, m->title);

	if (!m->shadowGC)
	{
	    /* GC for the shadow */
	    xgcv.foreground = fl_get_flcolor(puptcolor);
	    xgcv.font = pup_fs->fid;
	    xgcv.subwindow_mode = IncludeInferiors;
	    xgcv.stipple = fl_inactive_pattern;
	    kmask = GCForeground | GCFont | GCSubwindowMode | GCStipple;

	    m->shadowGC = XCreateGC(flx->display, m->win, kmask, &xgcv);
	    XSetFillStyle(flx->display, m->shadowGC, FillStippled);

	    /* GC for main text */
	    m->pupGC1 = XCreateGC(flx->display, m->win, kmask, &xgcv);

	    /* GC for inactive text */
	    xgcv.foreground = fl_get_flcolor(FL_INACTIVE);
	    m->pupGC2 = XCreateGC(flx->display, m->win, kmask, &xgcv);
	    /* special hack for B&W */
	    if (fl_dithered(fl_vmode))
		XSetFillStyle(flx->display, m->pupGC2, FillStippled);
	}
    }

    /* external coord is given relative to root */
    if (!extpos)
	fl_get_mouse(&extx, &exty, &kmask);
    else if (extx < 0)
	extx = -extx - m->w;
    else if (exty < 0)
	exty = -exty - m->h;

    /* if parent is not root, need to find its geometry */
    if (m->parent != fl_root)
	fl_get_win_geometry(m->parent, &px, &py, &pw, &ph);

    x = extx;
    y = exty;
    mw = m->w;
    mh = m->h;

#if !ALWAYSROOT
    /* check if the stuff is inside the window, if not, make it so  */
    if ((x + mw) > (px + pw))
	x = px + pw - mw;
    if ((y + mh) > (py + ph))
	y = py + ph - mh;
#endif

    /* parent might out of sight */
    if ((x + mw) > fl_scrw)
	x = fl_scrw - mw;
    if ((y + mh) > fl_scrh)
	y = fl_scrh - mh;

    /* if window is too small, show whatever we can */
    if (x < 1)
	x = 1;
    if (y < 1)
	y = 1;

    /* see if we need to warp mouse. If external coord, don't do it */
    if (!extpos && (x != extx || y != exty))
	XWarpPointer(flx->display, None, None, 0, 0, 0, 0,
		     (x - extx), (y - exty));
    extpos = 0;
    m->x = x;
    m->y = y;

    /* window is created at (0,0). Move to new locations */
    XMoveWindow(flx->display, m->win, x - px, y - 2 * m->padh - py);
    XMapRaised(flx->display, m->win);
    XSetWMColormapWindows(flx->display, m->parent, &m->win, 1);
    fl_context->pup_win = m->win;

    /* make sure the popup window shows up */
    XSync(flx->display, 0);
    while (XCheckWindowEvent(flx->display, m->win, 0x00ffffff, &ev))
	;
    draw_only(m);
}

void
fl_hidepup(int n)
{
    if (n >= 0 && n < fl_maxpup)
	close_pupwin(menu_rec + n);
    if (n == fl_context->pup_id)
	fl_context->pup_id = -1;
}

unsigned
fl_getpup_mode(int nm, int ni)
{
    MenuItem *item;
    if ((item = requested_item_isvalid("getpup", nm, ni)))
	return item->mode;
    return 0;
}

const char *
fl_getpup_text(int nm, int ni)
{
    MenuItem *item;

    if ((item = requested_item_isvalid("getpup", nm, ni)))
	return item->str;

    return 0;
}

void
fl_replacepup_text(int nm, int ni, const char *nt)
{
    MenuItem *item;

    if (!nt)
	nt = "";

    if ((item = requested_item_isvalid("getpup", nm, ni)))
    {
	fl_safe_free(item->str);
	item->str = fl_strdup(nt);
    }
}

int
fl_setpup_maxpup(int n)
{
    int i;

    if (n < FL_MAXPUP)
	return FL_MAXPUP;

    fl_init_pup();

    menu_rec = fl_realloc(menu_rec, n * sizeof(*menu_rec));
    for (i = fl_maxpup; i < n; i++)
    {
	menu_rec[i].title = 0;
	menu_rec[i].item[0] = 0;
    }

    return fl_maxpup = n;
}

/******************************************************************
 * a slightly high(ier) level interface
 *****************************************************************/

/* if no callback function is supplied, ignore the selection */
static int
ignore_item(int h)
{
    M_warn("DoPUP", "ignored=%d", h);
    return h;
}

/* build the menu using low-level pup support */
static int
generate_menu(int n, const FL_PUP_ENTRY * pup, int top)
{
    static const FL_PUP_ENTRY *p;
    static PopUP *menu;
    static int val;
    char buf[256];

    if (top)
    {
	val = 1;
	menu = menu_rec + n;
	menu->isEntry = 1;
	p = pup;
    }

    for (; p && p->text; p++, val++)
    {
	if (*p->text == '/')
	{
	    int m = fl_newpup(menu->parent);

	    if (p->text[1] != '_')
		snprintf(buf, sizeof(buf), "%s%%x%d%%m", (p->text) + 1, val);
	    else
		snprintf(buf, sizeof(buf), "%s%%x%d%%l%%m", (p->text) + 2, val);

	    fl_addtopup(n, buf, m);

	    if (p->mode)
		fl_setpup_mode(n, val, p->mode);
	    if (p->shortcut && *p->shortcut)
		fl_setpup_shortcut(n, val, p->shortcut);

	    /* advance val due to recursion */
	    val++;
	    generate_menu(m, ++p, 0);
	    menu_rec[m].isEntry = 1;
	}
	else
	{
	    /* regular entry */
	    if (*p->text == '_')
		snprintf(buf,sizeof(buf),"%s%%l%%x%d%%f", p->text + 1, val);
	    else
		snprintf(buf,sizeof(buf), "%s%%x%d%%f", p->text, val);

	    fl_addtopup(n, buf, p->callback ? p->callback : ignore_item);

	    if (p->mode)
		fl_setpup_mode(n, val, p->mode);

	    if (p->shortcut && *p->shortcut)
		fl_setpup_shortcut(n, val, p->shortcut);
	}
    }

    return n;
}


int
fl_setpup_entries(int nm, FL_PUP_ENTRY * entries)
{
    return generate_menu(nm, entries, 1);

}

void
fl_reparent_pup(int n, Window newwin)
{
    FL_State *fs = fl_state + fl_vmode;
    int nrt = (fs->pcm ||
	     (fl_visual(fl_vmode) != DefaultVisual(flx->display, fl_screen)));

    if (newwin == 0)
	newwin = fl_root;

    /* if we are using default visual/depth, root is a good choice */
    if (!nrt)
	newwin = fl_root;
#if ALWAYSROOT
    newwin = fl_root;
#endif

    if (n >= 0 && n < fl_maxpup)
    {
	if (menu_rec[n].win)
	{
	    XEvent xev;
	    XReparentWindow(flx->display, menu_rec[n].win, newwin, 0, 0);
	    while (!XCheckTypedEvent(flx->display, ReparentNotify, &xev))
		;
	}
	else
	    menu_rec[n].parent = newwin;
    }
}

void
fl_getpup_window(int n, Window * parent, Window * win)
{
    if (n >= 0 && n < fl_maxpup)
    {
	*parent = menu_rec[n].parent;
	*win = menu_rec[n].win;
    }
    else
	*parent = *win = 0;
}

int
fl_getpup_items(int n)
{
    int m = 0, k, i;

    if (n >= 0 && n < fl_maxpup)
    {
	m = k = menu_rec[n].nitems;
	for (i = 0; i < k; i++)
	    if (menu_rec[n].item[i]->subm >= 0)
		m += fl_getpup_items(menu_rec[n].item[i]->subm);
    }
    return m;
}

int
fl_current_pup(void)
{
    return fl_context->pup_id;
}

int
fl_setpup_default_bw(int bw)
{
    int ori = pupbw;

    pupbw = bw;
    return ori;
}
