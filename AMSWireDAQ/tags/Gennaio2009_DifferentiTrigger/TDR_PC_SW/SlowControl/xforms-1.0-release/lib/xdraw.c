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
 * $Id: xdraw.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *
 *  Basic low level drawing routines in Xlib.
 *
 *  BUGS: All form window share a common GC and Colormap.
 *
 */

#if defined(F_ID) || defined(DEBUG)
char *fl_id_drw = "$Id: xdraw.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include "forms.h"

static int mono_dither(unsigned long);
static void set_current_gc(GC);
static GC dithered_gc;

/*******************************************************************
 *
 * Rectangle routines
 *
 ****************************************************************{**/

#if 0
static void
draw_rectangle(int fill, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
	       FL_COLOR col)
{
    (fill ? XFillRectangle : XDrawRectangle) (flx->display, flx->win,
		       flx->gc, (int) x, (int) y, (unsigned) w, (unsigned) h);
}

#else
#define draw_rectangle(f,x,y,w,h,c)    \
          ((f)?XFillRectangle:XDrawRectangle)(flx->display, flx->win,\
          flx->gc, (int)(x), (int)(y), (unsigned)(w), (unsigned)(h))
#endif

/* General rectangle routine */
void
fl_rectangle(int fill, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
	     FL_COLOR col)
{
    int bw = fl_dithered(fl_vmode) && mono_dither(col);
    GC gc = flx->gc;

    fl_canonicalize_rect(&x, &y, &w, &h);

    if (bw && fill)
    {
	set_current_gc(fl_whitegc);
	draw_rectangle(fill, x, y, w, h, col);
	set_current_gc(dithered_gc);
    }

    fl_color(bw ? FL_BLACK : col);
    draw_rectangle(fill, x, y, w, h, col);

    if (bw)
	set_current_gc(gc);
}


/****** End of rectangle routines ***********************}***/


/****************************************************************
 *
 * Polygon and polylines
 *
 ***********************************************************{****/


/* if fill == 0, xp must be 1 more than n */
void
fl_polygon(int fill, FL_POINT * xp, int n, unsigned long col)
{
    int bw = fl_dithered(fl_vmode) && mono_dither(col);
    GC gc = flx->gc;

    if (bw)
    {
	flx->gc = dithered_gc;
	fl_color(FL_WHITE);
	if (fill)
	    XFillPolygon(flx->display, flx->win, flx->gc, xp, n,
			 Nonconvex, CoordModeOrigin);
	else
	{
	    xp[n].x = xp[0].x;
	    xp[n].y = xp[0].y;
	    n++;
	    XDrawLines(flx->display, flx->win, flx->gc, xp, n, CoordModeOrigin);
	}
    }

    fl_color(bw ? FL_BLACK : col);

    if (fill)
	XFillPolygon(flx->display, flx->win, flx->gc, xp, n,
		     Nonconvex, CoordModeOrigin);
    else
    {
	xp[n].x = xp[0].x;
	xp[n].y = xp[0].y;
	n++;
	XDrawLines(flx->display, flx->win, flx->gc, xp, n, CoordModeOrigin);
    }

    if (bw)
        flx->gc = gc;
}


/****************** End of polygons *******************}********/

/****************************************************************
 *
 * Ellipse
 *
 **********************************************************{******/

void
fl_oval(int fill, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
	FL_COLOR col)
{
    int bw = fl_dithered(fl_vmode) && mono_dither(col);
    GC gc = flx->gc;

    if (bw)
    {
	set_current_gc(fl_whitegc);
	(fill ? XFillArc : XDrawArc)
	    (flx->display, flx->win, flx->gc, x, y, w, h, 0, 360 * 64);
	set_current_gc(dithered_gc);
    }

    fl_color(bw ? FL_BLACK : col);

    (fill ? XFillArc : XDrawArc)
	(flx->display, flx->win, flx->gc, x, y, w, h, 0, 360 * 64);

    if (bw)
	set_current_gc(gc);
}

void
fl_ovalbound(FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h, FL_COLOR col)
{
    fl_color(col);
    XFillArc(flx->display, flx->win, flx->gc, x, y, w, h, 0, 360 * 64);
    fl_color(FL_BLACK);
    XDrawArc(flx->display, flx->win, flx->gc, x, y, w - 1, h - 1, 0, 360 * 64);
}

/******* End of ellipse routines ****************}***********/

/****************************************************************
 *
 * Arcs
 ****************************************************************/

void
fl_ovalarc(int fill, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
	   int t0, int dt, FL_COLOR col)
{
    int mono = fl_dithered(fl_vmode) && mono_dither(col);

    if (w < 0 || h < 0)
    {
	M_err("PieSlice", "negative size w=%d h=%d\n", w, h);
	return;
    }

    if (mono)
    {
	set_current_gc(fl_whitegc);
	(fill ? XFillArc : XDrawArc)
	    (flx->display, flx->win, flx->gc, x, y, w, h,
	     (int) (t0 * 6.4), (int) (dt * 6.4));
	set_current_gc(dithered_gc);
    }

    fl_color(mono ? FL_BLACK : col);
    (fill ? XFillArc : XDrawArc)
	(flx->display, flx->win, flx->gc, x, y, w, h,
	 (int) (t0 * 6.4), (int) (dt * 6.4));

    if (mono)
	set_current_gc(fl_state[fl_vmode].gc[0]);
}

void
fl_pieslice(int fill, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
	    int a1, int a2, FL_COLOR col)
{
    int delta = a2 - a1, bw = fl_dithered(fl_vmode) && mono_dither(col);
    GC gc = flx->gc;

    if (w < 0 || h < 0)
    {
	M_err("PieSlice", "negative size w=%d h=%d\n", w, h);
	return;
    }

    if (bw)
    {
	set_current_gc(fl_whitegc);
	(fill ? XFillArc : XDrawArc)
	    (flx->display, flx->win, flx->gc, x, y, w, h,
	     (int) (a1 * 6.4), (int) (delta * 6.4));
	set_current_gc(dithered_gc);
    }

    fl_color(bw ? FL_BLACK : col);
    (fill ? XFillArc : XDrawArc)
	(flx->display, flx->win, flx->gc, x, y, w, h,
	 (int) (a1 * 6.4), (int) (delta * 6.4));
    if (bw)
	set_current_gc(gc);
}

/*********************************************************************
 * Line segments
 *****************************************************************{***/

void
fl_lines(FL_POINT * xp, int n, FL_COLOR col)
{

    fl_color(col);
#if 0
    /* test */
    fl_context->ext_request_size = 10;
#endif

    /* we may need to break up the request into smaller pieces */
    if (fl_context->ext_request_size >= n)
	XDrawLines(flx->display, flx->win, flx->gc, xp, n, CoordModeOrigin);
    else
    {
	int req = fl_context->ext_request_size;
	int i, nchunks = (n + (n / req)) / req, left;
	FL_POINT *p = xp;

	for (i = 0; i < nchunks; i++, p += req - 1)
	    XDrawLines(flx->display, flx->win, flx->gc, p, req, CoordModeOrigin);

	left = ((xp + n) - p);

	if (left)
	{
	    if (left == 1)
	    {
		p--;
		left++;
	    }
	    XDrawLines(flx->display, flx->win, flx->gc, p, left, CoordModeOrigin);
	}
    }
}

/* simple line (x1,y1) (x2,y2) */

void
fl_line(FL_Coord xi, FL_Coord yi, FL_Coord xf, FL_Coord yf, FL_COLOR c)
{
    fl_color(c);
    XDrawLine(flx->display, flx->win, flx->gc, xi, yi, xf, yf);
}

/**************** End of line segments *******************}*********/

/* points */
void
fl_point(FL_Coord x, FL_Coord y, FL_COLOR c)
{
    fl_color(c);
    XDrawPoint(flx->display, flx->win, flx->gc, x, y);
}

void
fl_points(FL_POINT * p, int np, FL_COLOR c)
{
    fl_color(c);
    XDrawPoints(flx->display, flx->win, flx->gc, p, np, CoordModeOrigin);
}

/********************************************************************
 * Basic drawing attributes
 ****************************************************************{*/

static int lw = 0, ls = LineSolid, drmode = GXcopy;

void
fl_linewidth(int n)
{
    if (lw != n)
    {
	XGCValues gcvalue;
	unsigned long gcmask;

	gcmask = GCLineWidth;
	gcvalue.line_width = lw = n;
	XChangeGC(flx->display, flx->gc, gcmask, &gcvalue);
    }
}

int
fl_get_linewidth(void)
{
    return lw;
}

void
fl_xlinestyle(Display * d, GC gc, int n)
{
    static char dots[] = {2, 4};
    static char dotdash[] = {7, 3, 2, 3};
    static char ldash[] = {10, 4};

    if (ls != n)
    {
	XGCValues gcvalue;
	unsigned long gcmask;

	ls = n;

	gcmask = GCLineStyle;
	if (n == FL_DOT)
	    fl_xdashedlinestyle(d, gc, dots, 2);
	else if (n == FL_DOTDASH)
	    fl_xdashedlinestyle(d, gc, dotdash, 4);
	else if (n == FL_LONGDASH)
	    fl_xdashedlinestyle(d, gc, ldash, 2);
	if (n > LineDoubleDash)
	    n = LineOnOffDash;

	gcvalue.line_style = n;
	XChangeGC(d, gc, gcmask, &gcvalue);
    }

}

void
fl_linestyle(int n)
{
    fl_xlinestyle(flx->display, flx->gc, n);
}

int
fl_get_linestyle(void)
{
    return ls;
}

int
fl_get_drawmode(void)
{
    return drmode;
}

void
fl_drawmode(int request)
{
    if (drmode != request)
	XSetFunction(flx->display, flx->gc, drmode = request);
}

void
fl_xdashedlinestyle(Display * d, GC gc, const char *dash, int ndash)
{
    static char default_dash[] = {4, 4};

    if (dash == 0)
    {
	dash = default_dash;
	ndash = 2;
    }

    XSetDashes(d, gc, 0, (char *) dash, ndash);
}

void
fl_dashedlinestyle(const char *dash, int ndash)
{
    static char default_dash[] = {4, 4};

    if (dash == 0)
    {
	dash = default_dash;
	ndash = 2;
    }

    XSetDashes(flx->display, flx->gc, 0, (char *) dash, ndash);
}


/************************************************************************
 * Clipping stuff
 ***********************************************************************/
/*
 *  Remember global clipping so unset_clipping will restore it. Most
 *  useful as part of event dispatching
 */
XRectangle fl_perm_xcr;
int fl_perm_clip;
static FL_RECT cur_clip;	/* not includng perm clip, probably should */

void
fl_set_perm_clipping(FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h)
{
    fl_perm_clip = 1;
    fl_perm_xcr.x = x;
    fl_perm_xcr.y = y;
    fl_perm_xcr.width = w;
    fl_perm_xcr.height = h;
}

void
fl_unset_perm_clipping(void)
{
    fl_perm_clip = 0;
}

void
fl_set_clipping(FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h)
{
    cur_clip.x = x;
    cur_clip.y = y;
    cur_clip.width = w;
    cur_clip.height = h;

    if(w && h )
       XSetClipRectangles(flx->display, flx->gc, 0, 0, &cur_clip, 1, Unsorted);
    else
       XSetClipMask(flx->display, flx->gc, None);
}

void
fl_set_text_clipping(FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h)
{
    fl_set_gc_clipping(flx->textgc, x, y, w, h);
}

void
fl_unset_text_clipping(void)
{
    fl_unset_gc_clipping(flx->textgc);
}

void
fl_get_clipping(FL_Coord * x, FL_Coord * y, FL_Coord * w, FL_Coord * h)
{
    *x = cur_clip.x;
    *y = cur_clip.y;
    *w = cur_clip.width;
    *h = cur_clip.height;
}

void
fl_set_additional_clipping(FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h)
{
    FL_RECT rect[2], *r;

    rect[0] = cur_clip;
    rect[1].x = x;
    rect[1].y = y;
    rect[1].width = w;
    rect[1].height = h;

    if ((r = (FL_RECT *) fl_union_rect(rect, rect + 1)) != 0)
    {
        XSetClipRectangles(flx->display, flx->gc, 0, 0, r, 1, Unsorted);
    }
}


void
fl_set_gc_clipping(GC gc, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h)
{
    XRectangle xrect;
    xrect.x = x;
    xrect.y = y;
    xrect.width = w;
    xrect.height = h;
    XSetClipRectangles(flx->display, gc, 0, 0, &xrect, 1, Unsorted);
}

void
fl_set_clippings(FL_RECT * xrect, int n)
{
    XSetClipRectangles(flx->display, flx->gc, 0, 0, xrect, n, Unsorted);
}


void
fl_unset_clipping(void)
{
    if (!fl_perm_clip)
    {
	XSetClipMask(flx->display, flx->gc, None);
	cur_clip.x = cur_clip.y = cur_clip.width = cur_clip.height = 0;
    }
    else
    {
	XSetClipRectangles(flx->display, flx->gc, 0, 0, &fl_perm_xcr, 1, Unsorted);
	cur_clip = fl_perm_xcr;
    }
}


void
fl_unset_gc_clipping(GC gc)
{
    if (!fl_perm_clip)
	XSetClipMask(flx->display, gc, None);
    else
	XSetClipRectangles(flx->display, gc, 0, 0, &fl_perm_xcr, 1, Unsorted);
}

static void
set_current_gc(GC gc)
{
    if (flx->gc != gc)
    {
	flx->gc = gc;
        flx->color = FL_NoColor;
    }
}

/*
 * manually dither non-gray scale colors by changing default GC. Grayscales
 * are typically used in buttons, boxes etc, better not to dither them
 */
static int
mono_dither(unsigned long col)
{
    int bwtrick = 0;

    switch (col)
    {
    case FL_RED:
    case FL_MAGENTA:
    case FL_SLATEBLUE:
    case FL_PALEGREEN:
    case FL_DARKGOLD:
    case FL_INACTIVE_COL:
	dithered_gc = fl_bwgc[1];
	bwtrick = 1;
	break;
    case FL_YELLOW:
    case FL_CYAN:
    case FL_INDIANRED:
    case FL_GREEN:
	dithered_gc = fl_bwgc[2];
	bwtrick = 1;
	break;
    case FL_BLUE:
	dithered_gc = fl_bwgc[0];
	bwtrick = 1;
	break;
    default:
	if (col >= FL_FREE_COL1)
	{
	    int r, g, b;
	    fl_get_icm_color(col, &r, &g, &b);
	    if ((bwtrick = (r > 70 && r <= 210)))
		dithered_gc = fl_bwgc[(r / 70) - 1];
	}
	break;
    }

    return bwtrick;
}
