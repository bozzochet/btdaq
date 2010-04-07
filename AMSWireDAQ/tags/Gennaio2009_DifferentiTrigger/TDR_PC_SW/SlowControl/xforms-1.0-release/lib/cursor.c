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
 * $Id: cursor.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao
 *  All rights reserved.
 *.
 *
 *  Cursor routines
 *
 *     name is one of the standard symbolic cursor names (XC_watch etc) or
 *          the constants defined by XFORMS (FL_INVISIBLE_CURSOR etc)
 *
 *     Cursor is the cursor usable in XDefineCursor(win, cursor)
 *
 */

#if defined(F_ID) || defined(DEBUG)
char *fl_id_cur = "$Id: cursor.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include "forms.h"
#include "extern.h"

#define MAX_CURSORS   64	/* max. number of bitmap cursors     */
#define MAX_SEQ       24	/* max. number cursors per animation */

/* cursor cache */
typedef struct
{
    int name;
    int ncursor;
    int cur_cursor;
    Window win;
    int timeout_id;
    int timeout;
    Cursor cur[MAX_SEQ];
}
CurStruct;


/* pre-built cursors */
static CurStruct prebuilt[] =
{
    {XC_watch},
    {XC_sb_right_arrow},
    {XC_hand2},
    {XC_top_left_arrow},
    {XC_tcross},
    {XC_pirate}
};

static CurStruct *cursors;

static int fl_default_curname = XC_top_left_arrow;	/* xforms' default */

#ifndef XC_num_glyphs
#define XC_num_glyphs 255
#endif

static int user_cur_name = XC_num_glyphs;

static CurStruct *find_timeout(Window win);

static CurStruct *
add_cursor(int name, Cursor cur)
{
    CurStruct *c;
    static int warned;

    for (c = cursors; c->name && c->name != name; c++)
	;

    if (c < cursors + MAX_CURSORS)
    {
	c->name = name;
	c->cur[c->ncursor++] = cur;
    }
    else if (!warned)
    {
	M_err("AddCursor", "too many cursors");
	warned = 1;
    }
    return c;
}

static Cursor
create_bitmap_cursor(const char *source, const char *mask,
		     int w, int h, int hotx, int hoty)
{
    Pixmap s, m = None;
    XColor fg, bg;
    Cursor cur;
    Window win = fl_root;

    s = XCreateBitmapFromData(flx->display, win, (char *) source, w, h);

    if (mask)
	m = XCreateBitmapFromData(flx->display, win, (char *) mask, w, h);

    fg.red = fg.green = fg.blue = 0;
    bg.red = bg.green = bg.blue = 0xffff;
    cur = XCreatePixmapCursor(flx->display, s, m, &fg, &bg, hotx, hoty);

    return cur;
}


static void
init_cursors(void)
{
    static int ok;
    CurStruct *c;
    static char c_bits[] =
    {0x00};

    if (ok)
	return;

    ok = 1;

    if (!cursors)
    {
	cursors = fl_calloc(MAX_CURSORS, sizeof(*c));
	memcpy(cursors, prebuilt, sizeof(prebuilt));
    }

    for (c = cursors; c->name; c++)
	c->cur[c->ncursor++] = XCreateFontCursor(flx->display, c->name);

    /* create an invisible cursor */
    add_cursor(FL_INVISIBLE_CURSOR,
	       create_bitmap_cursor(c_bits, c_bits, 1, 1, 0, 0));

    /* add the default cursor     */
    add_cursor(FL_DEFAULT_CURSOR,
	       XCreateFontCursor(flx->display, fl_default_curname));
}


static void
animate_cursor(int id, void *data)
{
    CurStruct *cur = data;

    cur->timeout_id = 0;
    fl_set_cursor(cur->win, cur->name);
}

/*
 * Set cursor for win to name. Name is either the standard XC_ or
 * Form defined
 */
void
fl_set_cursor(Window win, int name)
{
    CurStruct *c = cursors;

    init_cursors();

    if (win == 0)
    {
	M_err("SetCuror", "Bad Window");
	return;
    }

    for (; c->name; c++)
    {
	if (c->name == name)
	{
	    if (c->ncursor > 1)
	    {
		int n = (c->cur_cursor % c->ncursor);
		XDefineCursor(flx->display, win, c->cur[n]);;
		c->cur_cursor++;
		c->win = win;
		if (c->timeout_id == 0)
		    c->timeout_id = fl_add_timeout(c->timeout, animate_cursor, c);
	    }
	    else
	    {
		CurStruct *cur;
		cur = find_timeout(win);
		if (cur && cur->timeout_id)
		{
		    fl_remove_timeout(cur->timeout_id);
		    cur->timeout_id = 0;
		}
		XDefineCursor(flx->display, win, c->cur[0]);;
	    }
	    return;
	}
    }

    XDefineCursor(flx->display, win, fl_get_cursor_byname(name));
}

/*
 * to be used in XDefineCursor
 */
Cursor
fl_get_cursor_byname(int name)
{
    CurStruct *c;
    Cursor cur = 0;

    init_cursors();

    for (c = cursors; c->name; c++)
    {
	if (c->name == name)
	{
	    int n = c->cur_cursor % c->ncursor;
	    c->cur_cursor++;
	    return c->cur[n];
	}
    }

    /* take a wild shot. Since we can generate the default X cursor on the
       fly, we don't have to always save them to the cursor structure */
    if (name < XC_num_glyphs - 1)
    {
	static int nn;
	cur = XCreateFontCursor(flx->display, name);
	if (nn < 10)
	{
	    add_cursor(name, cur);
	    nn++;
	}
    }
    else
    {
	M_err("GetCursor", "Unknown cursor: %d\n", name);
	cur = fl_get_cursor_byname(FL_DEFAULT_CURSOR);	/* recursion */
    }

    return cur;
}

void
fl_set_cursor_color(int name, FL_COLOR fg, FL_COLOR bg)
{
    XColor xfg, xbg;
    int r, g, b;
    Cursor cur = fl_get_cursor_byname(name);

    fl_getmcolor(fg, &r, &g, &b);
    xfg.red = (r << 8) | 0xff;
    xfg.green = (g << 8) | 0xff;
    xfg.blue = (b << 8) | 0xff;

    fl_getmcolor(bg, &r, &g, &b);
    xbg.red = (r << 8) | 0xff;
    xbg.green = (g << 8) | 0xff;
    xbg.blue = (b << 8) | 0xff;

    XRecolorCursor(flx->display, cur, &xfg, &xbg);
}


int
fl_create_bitmap_cursor(const char *source, const char *mask,
			int w, int h, int hotx, int hoty)
{
    Cursor cur;

    init_cursors();
    cur = create_bitmap_cursor(source, mask, w, h, hotx, hoty);
    add_cursor(user_cur_name, cur);
    return user_cur_name++;
}

/* cursor animation */

int
fl_create_animated_cursor(int *cur_names, int timeout)
{
    int *n = cur_names, k = MAX_SEQ;
    CurStruct *c = 0;

    for (; *n >= 0 && --k >= 0; n++)
	c = add_cursor(user_cur_name, fl_get_cursor_byname(*n));

    if (c)
	c->timeout = timeout > 0 ? timeout : 20;

    return user_cur_name++;
}

static CurStruct *
find_timeout(Window win)
{
    CurStruct *c = cursors;
    for (; c->name; c++)
    {
	if (c->win == win && c->timeout_id)
	    return c;
    }
    return 0;
}
