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
 * $Id: fldraw.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *  High level drawing routines. Uses the routines defined in xdraw.c
 *
 */

#if defined(F_ID) || defined(DEBUG)
char *fl_id_drw = "$Id: fldraw.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include "forms.h"


#if 0
#define MAXBOXTYPE 25
typedef void (*FL_DRAW_BOX) (int, FL_Coord, FL_Coord, FL_Coord, FL_Coord,
			     FL_COLOR, int);

static FL_DRAW_BOX **drawbox;

void
fl_add_boxtype(int style1, int style2, DrawBox draw)
{
    if (!drawbox)
	drawbox = fl_calloc(MAXBOXTYPE, sizeof(*drawbox));

    if (style1 < 0 || style2 >= MAXBOXTYPE)
    {
	M_err("AddBoxtype", "internal error");
	return
    }

    for (i = style1; i <= style2; i++)
	drawbox[i] = draw;
}

#endif

static void fl_foldertab_box(int, FL_Coord, FL_Coord, FL_Coord,
			     FL_Coord, FL_COLOR, int);

/*******************************************************************
 * Rectangle routines
 ****************************************************************{**/

/*  Make an arbitary rectangle have positive width and height */
void
fl_canonicalize_rect(FL_Coord * x, FL_Coord * y, FL_Coord * w, FL_Coord * h)
{
    if (*w < 0)
    {
	*w = -*w;
	*x -= *w;
    }

    if (*h < 0)
    {
	*h = -*h;
	*y -= *h;
    }
}


/* Draw a filled rectangle with a black boundary. Also Compensates
   for the inconsistency in Xlib
 */
int flrectboundcolor = FL_BLACK;
void
fl_rectbound(FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h, FL_COLOR col)
{
    fl_canonicalize_rect(&x, &y, &w, &h);

    /* 0 width has special meaning in Xlib */
    if (h < 2)
	h = 2;
    if (w < 2)
	w = 2;

    fl_rectangle(1, x + 1, y + 1, w - 1, h - 1, col);
    fl_rectangle(0, x, y, w, h, flrectboundcolor);
}

/****** End of rectangle routines ***********************}***/

#define RS     (FL_Coord)18

/*    0.0, 0.034074, 0.1339746, 0.292893, 0.5, 0.741181, 1.0 */
static float offset[] =
{
    0.0f, 0.07612f, 0.29289f, 0.61732f, 1.0f
};

#define RN     (sizeof(offset)/sizeof(offset[0]))

static int
compute_round_corners(FL_Coord x, FL_Coord y, FL_Coord w,
		      FL_Coord h, FL_POINT * point)
{
    FL_POINT *xp;
    int i, n;
    float rsx, rsy, xf, yf;

    rsx = rsy = 0.45f * FL_min(w, h);

    if (rsx > RS)
	rsx = rsy = RS;

    for (xp = point, n = i = 0; i < RN; i++, n++, xp++)
    {
	xf = (x + offset[RN - i - 1] * rsx);
	yf = (y + offset[i] * rsy);
	xp->x = (short)FL_nint(xf);
	xp->y = (short)FL_nint(yf);
    }

    for (i = 0; i < RN; i++, n++, xp++)
    {
	xf = (x + offset[i] * rsx);
	yf = (y + h - 1.0f - offset[RN - i - 1] * rsy);
	xp->x = (short)FL_nint(xf);
	xp->y = (short)FL_nint(yf);
    }

    for (i = 0; i < RN; i++, n++, xp++)
    {
	xf = (x + w - 1.0f - offset[RN - i - 1] * rsx);
	yf = (y + h - 1.0f - offset[i] * rsy);
	xp->x = (short) FL_nint(xf);
	xp->y = (short) FL_nint(yf);
    }

    for (i = 0; i < RN; i++, n++, xp++)
    {
	xf = (x + w - 1.0f - offset[i] * rsx);
	yf = (y + offset[RN - i - 1] * rsy);
	xp->x = (short) FL_nint(xf);
	xp->y = (short) FL_nint(yf);
    }

    return n;
}


void
fl_roundrectangle(int fill, FL_Coord x, FL_Coord y, FL_Coord w,
		  FL_Coord h, FL_COLOR col)
{
    FL_POINT point[5 * RN + 2];
    int n;

    n = compute_round_corners(x, y, w, h, point);
    fl_polygon(fill, point, n, col);
}

#define Shrink(x,y,w,h,d) do {x += (d); y += (d); \
			      w -= 2 * (d); h -= 2*(d);} while (0)

static void
fl_rounded3dbox(int style, FL_Coord x, FL_Coord y, FL_Coord w,
		FL_Coord h, FL_COLOR col, int bw)
{
    FL_POINT point[5 * RN + 2];
    int lw = FL_abs(bw), cr = lw / 2;
    int n, olw;


    Shrink(x, y, w, h, cr);

    n = compute_round_corners(x, y, w, h, point);
    fl_polyf(point, n, col);

    olw = fl_get_linewidth();
    fl_linewidth(lw);

    /* draw the shadow */
    if (style == FL_ROUNDED3D_UPBOX)
    {
	fl_lines(point, 2 * RN, FL_LEFT_BCOL);
	fl_lines(point + 2 * RN - 3, RN + 1, FL_BOTTOM_BCOL);
	fl_lines(point + 3 * RN - 4, RN + 2, FL_RIGHT_BCOL);
	point[n] = point[0];
	fl_lines(point + n - 3, 4, FL_TOP_BCOL);
    }
    else
    {
	fl_lines(point, 2 * RN, FL_BOTTOM_BCOL);
	fl_lines(point + 2 * RN - 3, RN + 1, FL_TOP_BCOL);
	fl_lines(point + 3 * RN - 4, RN + 2, FL_LEFT_BCOL);
	point[n] = point[0];
	fl_lines(point + n - 3, 4, FL_BOTTOM_BCOL);
    }

    fl_linewidth(olw);

    if (bw > 0 && fl_dithered(fl_vmode))
    {
	n = compute_round_corners(x, y, w, h, point);
	fl_polygon(0, point, n, FL_BLACK);
    }
}

static void
fl_oval3dbox(int style, FL_Coord x, FL_Coord y, FL_Coord w,
	     FL_Coord h, FL_COLOR col, int bw)
{
    int absbw = FL_abs(bw), cr = absbw / 2, olw = fl_get_linewidth();
    int extra = 1 + (absbw > 3);
    int xx, yy, ww, hh;

    Shrink(x, y, w, h, cr);
    fl_pieslice(1, x, y, w, h, 0, 3600, col);

    xx = x + extra;
    yy = y + extra;
    ww = w - 2 * extra;
    hh = h - 2 * extra;

    fl_linewidth(absbw);

    if (style == FL_OVAL3D_UPBOX)
    {
	fl_pieslice(0, x, y, w, h, 450, 2250, FL_TOP_BCOL);
	fl_pieslice(0, x, y, w, h, 0, 450, FL_BOTTOM_BCOL);
	fl_pieslice(0, x, y, w, h, 2250, 3600, FL_BOTTOM_BCOL);
    }
    else if (style == FL_OVAL3D_FRAMEBOX)
    {
	fl_linewidth(0);

	fl_pieslice(0, x, y, w, h, 450, 2250, FL_BOTTOM_BCOL);
	fl_pieslice(0, xx, yy, ww, hh, 450, 2250, FL_LEFT_BCOL);

	fl_pieslice(0, xx, yy, ww, hh, 0, 450, FL_BOTTOM_BCOL);
	fl_pieslice(0, x, y, w, h, 0, 450, FL_LEFT_BCOL);

	fl_pieslice(0, xx, yy, ww, hh, 2250, 3600, FL_BOTTOM_BCOL);
	fl_pieslice(0, x, y, w, h, 2250, 3600, FL_LEFT_BCOL);
    }
    else if (style == FL_OVAL3D_EMBOSSEDBOX)
    {
	fl_linewidth(0);

	fl_pieslice(0, x, y, w, h, 450, 2250, FL_LEFT_BCOL);
	fl_pieslice(0, xx, yy, ww, hh, 450, 2250, FL_BOTTOM_BCOL);

	fl_pieslice(0, xx, yy, ww, hh, 0, 450, FL_LEFT_BCOL);
	fl_pieslice(0, x, y, w, h, 0, 450, FL_BOTTOM_BCOL);

	fl_pieslice(0, xx, yy, ww, hh, 2250, 3600, FL_LEFT_BCOL);
	fl_pieslice(0, x, y, w, h, 2250, 3600, FL_BOTTOM_BCOL);
    }
    else
    {
	fl_pieslice(0, x, y, w, h, 450, 2250, FL_BOTTOM_BCOL);
	fl_pieslice(0, x, y, w, h, 0, 450, FL_TOP_BCOL);
	fl_pieslice(0, x, y, w, h, 2250, 3600, FL_TOP_BCOL);
    }

    fl_linewidth(olw);

    if (fl_dithered(fl_vmode))
	fl_pieslice(0, x, y, w, h, 0, 3600, FL_BLACK);
}


#define TRAN(q) ((y + h - 1) - q)

#define AddVertex(v,xp,yp) do {v->x=xp; v->y=yp;v++;} while(0)

/* Draw a rectangular box. TODO: need to change primitive box
 * drawing using frame
 */

void
fl_drw_box(int style, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
	   FL_COLOR c, int bw_in)
{
    FL_POINT vert[25], *fp;
    int border, B, dp = fl_dithered(fl_vmode), bw = bw_in;
    FL_Coord cx, cy, cw, ch;


    if (c == FL_NoColor)
	c = FL_COL1;

    if (style == FL_NO_BOX)
	return;


    if (!(border = bw > 0))
	bw = -bw;

    if (bw == 0 && style != FL_NO_BOX)
	style = FL_FLAT_BOX;

    B = border;

/* must guarante the width of rectangle > 0 */
    if ((w - 2 * bw) <= 0)
	bw = w / 2 - 1;
    if ((h - 2 * bw) <= 0)
	bw = h / 2 - 1;

    if (w <= 0 || h <= 0)
	return;

    switch (style)
    {
    case FL_NO_BOX:
	break;
    case FL_UP_BOX:

	fl_rectf(x + bw + B, y + bw + B, w - 2 * bw - 2 * B, h - 2 * bw - 2 * B, c);
	fl_rectf(x + B, y + B, w - 1 - B, bw, FL_TOP_BCOL);
	fl_rectf(x + B, y + h - bw - B, w - 1 - B, bw, FL_BOTTOM_BCOL);

	fp = vert;
	AddVertex(fp, x + w - B - bw, y + bw + B);
	AddVertex(fp, x + w - B - bw, y + h - B - bw);
	AddVertex(fp, x + w - B, y + h - B);
	AddVertex(fp, x + w - B, y + B);
	fl_polyf(vert, 4, FL_RIGHT_BCOL);

	fp = vert;

	/* left trapzoidal */
	AddVertex(fp, x + B, y + B);
	AddVertex(fp, x + B, y + h - B);
	AddVertex(fp, x + bw + B, y + h - bw - B);
	AddVertex(fp, x + bw + B, y + bw + B);
	fl_polyf(vert, 4, FL_LEFT_BCOL);

	if (border || fl_dithered(fl_vmode))
	    fl_rect(x, y, w - 1, h - 1, FL_RIGHT_BCOL);

	/* special hack for B&W */
	if (fl_dithered(fl_vmode))
	{
	    if (bw > 2)
	    {
		fp = vert;
		AddVertex(fp, x + B, y + B);
		AddVertex(fp, x + B + bw - 1, y + bw);
		AddVertex(fp, x + w - bw, y + bw);
		fl_lines(vert, 3, FL_BLACK);
		fp = vert;
		fl_simple_line(x + B + bw - 1, y + B + bw, x + B + bw - 1, y + h - bw, FL_BLACK);
	    }
	    else
	    {
		fl_rect(x, y, w - 1, h - 1, FL_BLACK);
	    }
	}
	break;

    case FL_DOWN_BOX:
	fl_rectf(x + bw, y + bw, w - 2 * bw, h - 2 * bw, c);
	fl_rectf(x, y + h - bw, w, bw - dp, FL_LEFT_BCOL);
	fl_rectf(x, y, w, bw, FL_BOTTOM_BCOL);

	/* right trapzoid */
	fp = vert;
	AddVertex(fp, x + w - bw, y + bw);
	AddVertex(fp, x + w - bw, y + h - bw);
	AddVertex(fp, x + w - dp, y + h);
	AddVertex(fp, x + w - dp, y);
	fl_polyf(vert, 4, FL_LEFT_BCOL);

	/* left trapzoid */
	fp = vert;
	AddVertex(fp, x, y);
	AddVertex(fp, x, y + h - 1);
	AddVertex(fp, x + bw, y + h - bw);
	AddVertex(fp, x + bw, y + bw);
	fl_polyf(vert, 4, FL_RIGHT_BCOL);

	/* special hack for B&W */
	if (fl_dithered(fl_vmode))
	{
	    fp = vert;
	    AddVertex(fp, x + B, y + h - 1);
	    AddVertex(fp, x + w - 1, y + h - 1);
	    AddVertex(fp, x + w - 1, y + B);
	    fl_lines(vert, 3, FL_BLACK);
	}
	break;
    case FL_FLAT_BOX:
	fl_rectf(x, y, w, h, c);
	break;
    case FL_BORDER_BOX:
	fl_rectbound(x, y, w - 1, h - 1, c);
	break;
    case FL_FRAME_BOX:
	B = (bw > 2 ? bw - 2 : 1);
	fl_drw_box(FL_DOWN_BOX, x, y, w, h, c, 1);
	x += B + 1;
	y += B + 1;
	w -= 2 * (B + 1);
	h -= 2 * (B + 1);
	fl_drw_frame(FL_UP_FRAME, x, y, w, h, c, -1);
	break;
    case FL_EMBOSSED_BOX:
	B = (bw > 2 ? bw - 2 : 1);
	fl_drw_box(FL_UP_BOX, x, y, w, h, c, -1);
	x += B + 1;
	y += B + 1;
	w -= 2 * (B + 1);
	h -= 2 * (B + 1);
	fl_drw_frame(FL_DOWN_FRAME, x, y, w, h, c, 1);
	break;
    case FL_ROUNDED_BOX:
	fl_roundrectf(x + 1, y + 1, w - 1, h - 1, c);
	fl_roundrect(x, y, w, h, FL_BLACK);
	break;
    case FL_ROUNDED3D_UPBOX:
    case FL_ROUNDED3D_DOWNBOX:
	fl_rounded3dbox(style, x, y, w, h, c, bw);
	break;
    case FL_SHADOW_BOX:
	fl_rectf(x + bw, y + h - bw, w - bw, bw, fl_shadow_col);
	fl_rectf(x + w - bw, y + bw, bw, h - bw, fl_shadow_col);
	fl_rectbound(x, y, w - bw, h - bw, c);
	break;
    case FL_RSHADOW_BOX:
	if (w > 70 && h > 70)
	    bw++;
	fl_get_clipping(&cx, &cy, &cw, &ch);

	/* draw the shadow.  draw it several times with clipping */
#if 0
	fl_set_additional_clipping(x + bw - 1, y + h - bw, w - bw + 1, bw);
	fl_roundrectf(x + bw, y + bw, w - bw, h - bw, fl_shadow_col);
	fl_set_additional_clipping(x + w - bw, y + bw, bw, h - bw);
	fl_roundrectf(x + bw, y + bw, w - bw, h - bw, fl_shadow_col);
	fl_set_additional_clipping(x + w + bw - RS - 2, y + h - RS, RS + 2, RS + 2);
#endif
	fl_roundrectf(x + bw, y + bw, w - bw, h - bw, fl_shadow_col);

	/* draw the box */
	fl_set_additional_clipping(x, y, w, h);
	fl_roundrectf(x + 1, y + 1, w - 1 - bw, h - 1 - bw, c);
	fl_roundrect(x, y, w - bw, h - bw, FL_BLACK);
	fl_set_clipping(cx, cy, cw, ch);
	break;
    case FL_RFLAT_BOX:
	fl_roundrectf(x, y, w, h, c);
	break;
    case FL_OVAL_BOX:
	fl_oval(1, x + 1, y + 1, w - 1, h - 1, c);
	fl_oval(0, x + 1, y + 1, w - 2, h - 2, FL_BLACK);
	break;
    case FL_OVAL3D_UPBOX:
    case FL_OVAL3D_DOWNBOX:
    case FL_OVAL3D_FRAMEBOX:
    case FL_OVAL3D_EMBOSSEDBOX:
	fl_oval3dbox(style, x, y, w, h, c, bw_in);
	break;
    case FL_TOPTAB_UPBOX:
    case FL_SELECTED_TOPTAB_UPBOX:
    case FL_BOTTOMTAB_UPBOX:
    case FL_SELECTED_BOTTOMTAB_UPBOX:
	fl_foldertab_box(style, x, y, w, h, c, bw_in);
	break;
    default:
	if ((style & FL_BROKEN_BOX))
	    fl_foldertab_box(style, x, y, w, h, c, bw_in);
	else
	    M_err("DrawBox", "Unkonwn boxtype:%d", style);
	break;
    }

}

/*
 *     0
 *   1   3
 *     2
 * draw a box rotated 45 degrees
 */
void
fl_drw_checkbox(int type, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		FL_COLOR col, int bw)
{
    FL_POINT xpoint[6], *xp;
    FL_POINT allp[10];
    int halfh = h / 2, halfw = w / 2;

    w = 2 * halfw;
    h = 2 * halfh;

    /* generate all points */
    xp = allp;
    AddVertex(xp, x + halfw, y);
    AddVertex(xp, x, y + halfh);
    AddVertex(xp, x + halfw, y + h);
    AddVertex(xp, x + w, y + halfh);

    AddVertex(xp, x + halfw, y + bw);
    AddVertex(xp, x + bw, y + halfh);
    AddVertex(xp, x + halfw, y + h - bw);
    AddVertex(xp, x + w - bw, y + halfh);

    /* draw overall box */
    xp = allp + 4;
    fl_polyf(xp, 4, col);

    xp = xpoint;
    switch (type)
    {
    case FL_UP_BOX:
	AddVertex(xp, allp[0].x, allp[0].y);
	AddVertex(xp, allp[1].x, allp[1].y);
	AddVertex(xp, allp[5].x, allp[5].y);
	AddVertex(xp, allp[4].x, allp[4].y);
	fl_polyf(xp = xpoint, 4, FL_LEFT_BCOL);

	AddVertex(xp, allp[0].x, allp[0].y);
	AddVertex(xp, allp[4].x, allp[4].y);
	AddVertex(xp, allp[7].x, allp[7].y);
	AddVertex(xp, allp[3].x, allp[3].y);
	fl_polyf(xp = xpoint, 4, FL_TOP_BCOL);

	/* right bottom */
	AddVertex(xp, allp[6].x, allp[6].y);
	AddVertex(xp, allp[2].x, allp[2].y);
	AddVertex(xp, allp[3].x, allp[3].y);
	AddVertex(xp, allp[7].x, allp[7].y);
	fl_polyf(xp = xpoint, 4, FL_RIGHT_BCOL);

	AddVertex(xp, allp[1].x, allp[1].y);
	AddVertex(xp, allp[2].x, allp[2].y);
	AddVertex(xp, allp[6].x, allp[6].y);
	AddVertex(xp, allp[5].x, allp[5].y);
	fl_polyf(xp = xpoint, 4, FL_BOTTOM_BCOL);
	break;
    case FL_DOWN_BOX:
	AddVertex(xp, allp[0].x, allp[0].y);
	AddVertex(xp, allp[1].x, allp[1].y);
	AddVertex(xp, allp[5].x, allp[5].y);
	AddVertex(xp, allp[4].x, allp[4].y);
	fl_polyf(xp = xpoint, 4, FL_RIGHT_BCOL);

	AddVertex(xp, allp[0].x, allp[0].y);
	AddVertex(xp, allp[4].x, allp[4].y);
	AddVertex(xp, allp[7].x, allp[7].y);
	AddVertex(xp, allp[3].x, allp[3].y);
	fl_polyf(xp = xpoint, 4, FL_BOTTOM_BCOL);

	/* right bottom */
	AddVertex(xp, allp[6].x, allp[6].y);
	AddVertex(xp, allp[2].x, allp[2].y);
	AddVertex(xp, allp[3].x, allp[3].y);
	AddVertex(xp, allp[7].x, allp[7].y);
	fl_polyf(xp = xpoint, 4, FL_LEFT_BCOL);

	AddVertex(xp, allp[1].x, allp[1].y);
	AddVertex(xp, allp[2].x, allp[2].y);
	AddVertex(xp, allp[6].x, allp[6].y);
	AddVertex(xp, allp[5].x, allp[5].y);
	fl_polyf(xp = xpoint, 4, FL_TOP_BCOL);
	break;
    }

#if 1
    /* add a border. destructive as polyl uses the allp[5] */
    if (fl_dithered(fl_vmode) /* || tbw > 0 */ )
	fl_polyl(allp, 4, FL_BLACK);
#endif

}


/* Draw a frame around a box. The frame is so drawn that it is just
 * outside of the box without any gap. A flat with the same size
 * as the frame just fit the inside the of the frame.
 */

void
fl_drw_frame(int style, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
	     FL_COLOR c, int bw)
{
    FL_POINT vert[25], *fp;
    int border, B, dp = fl_dithered(fl_vmode);

    if (w <= 0 || h <= 0)
	return;

    if (!(border = bw > 0))
	bw = -bw;

    B = border;

    switch (style)
    {
    case FL_UP_FRAME:
	/* must guarante the width of rectangle > 0 */
	if ((w - 2 * bw) <= 0)
	    bw = w / 2;
	if ((h - 2 * bw) <= 0)
	    bw = h / 2;

	x -= bw + B;
	y -= bw + B;
	w += 2 * (bw + B);
	h += 2 * (bw + B);

	fl_rectf(x + B, y + B, w - 1 - B, bw, FL_TOP_BCOL);
	fl_rectf(x + B, y + h - bw - B, w - 1 - B, bw, FL_BOTTOM_BCOL);

	fp = vert;
	AddVertex(fp, x + w - B - bw, y + bw + B);
	AddVertex(fp, x + w - B - bw, y + h - B - bw);
	AddVertex(fp, x + w - B, y + h - B);
	AddVertex(fp, x + w - B, y + B);
	fl_polyf(vert, 4, FL_RIGHT_BCOL);

	fp = vert;

	/* left trapzoidal */
	AddVertex(fp, x + B, y + B);
	AddVertex(fp, x + B, y + h - B);
	AddVertex(fp, x + bw + B, y + h - bw - B);
	AddVertex(fp, x + bw + B, y + bw + B);
	fl_polyf(vert, 4, FL_LEFT_BCOL);

	if (border || fl_dithered(fl_vmode))
	    fl_rect(x, y, w - 1, h - 1, FL_BLACK);

	/* special hack for B&W */
	if (fl_dithered(fl_vmode))
	{
	    if (bw > 2)
	    {
		fp = vert;
		AddVertex(fp, x + B, y + B);
		AddVertex(fp, x + B + bw - 1, y + bw);
		AddVertex(fp, x + w - bw, y + bw);
		fl_lines(vert, 3, FL_BLACK);
		fp = vert;
		fl_simple_line(x + B + bw - 1, y + B + bw, x + B + bw - 1,
			       y + h - bw, FL_BLACK);
	    }
	    else
	    {
		fl_rect(x, y, w - 1, h - 1, FL_BLACK);
	    }
	}
	break;

    case FL_DOWN_FRAME:
	x -= bw;
	y -= bw;
	w += 2 * bw;
	h += 2 * bw;
	/* top and bottom section */
	fl_rectf(x, y, w, bw, FL_BOTTOM_BCOL);	/* top */
	fl_rectf(x, y + h - bw, w, bw - dp, FL_LEFT_BCOL);	/* bottom */

	/* right trapzoid */
	fp = vert;
	AddVertex(fp, x + w - bw, y + bw);
	AddVertex(fp, x + w - bw, y + h - bw);
	AddVertex(fp, x + w - dp, y + h);
	AddVertex(fp, x + w - dp, y);
	fl_polyf(vert, 4, FL_LEFT_BCOL);

	/* left trapzoid */
	fp = vert;
	AddVertex(fp, x, y);
	AddVertex(fp, x, y + h - 1);
	AddVertex(fp, x + bw, y + h - bw);
	AddVertex(fp, x + bw, y + bw);
	fl_polyf(vert, 4, FL_RIGHT_BCOL);

	/* special hack for B&W */
	if (fl_dithered(fl_vmode))
	{
	    fp = vert;
	    AddVertex(fp, x + B, y + h - 1);
	    AddVertex(fp, x + w - 1, y + h - 1);
	    AddVertex(fp, x + w - 1, y + B);
	    fl_lines(vert, 3, FL_BLACK);
	}
	break;
    case FL_SHADOW_FRAME:
	if (w > 70 && h > 70)
	{
	    if ((bw += (w + h) / 140) > 5)
		bw = 5;
	}
	fl_rectf(x + bw, y + h, w, bw, fl_shadow_col);
	fl_rectf(x + w, y + bw, bw, h, fl_shadow_col);
	fl_rect(x - 1, y - 1, w + 1, h + 1, FL_BLACK);
	break;
    case FL_BORDER_FRAME:
	fl_rect(x - 1, y - 1, w + 1, h + 1, c);
	break;
    case FL_EMBOSSED_FRAME:
	B = (bw > 2 ? (bw - 2) : 1);
	fl_drw_frame(FL_UP_FRAME, x, y, w, h, 0, -1);
	fl_drw_frame(FL_DOWN_FRAME, x + B, y + B, w - 2 * B, h - 2 * B, 0, 1);
	break;
    case FL_ENGRAVED_FRAME:
	B = (bw > 2 ? (bw - 2) : 1);
	fl_drw_frame(FL_DOWN_FRAME, x, y, w, h, 0, 1);
	fl_drw_frame(FL_UP_FRAME, x + B, y + B, w - 2 * B, h - 2 * B, 0, -1);
	break;
    case FL_ROUNDED_FRAME:
	fl_roundrect(x - 1, y - 1, w + 2, h + 2, c);
	break;
    case FL_OVAL_FRAME:
	fl_oval(0, x - 1, y - 1, w + 2, h + 2, c);
	break;
    default:
	break;
    }
}

/********* Some convience functions, sort of GL in X   ******{*****/

#define MAX_BUF_POINT  128
static FL_POINT xpbuf[MAX_BUF_POINT];
static int npt;
static FL_COLOR pcol;

void
fl_add_vertex(FL_Coord x, FL_Coord y)
{
#if FL_CoordIsFloat
    xpbuf[npt].x = FL_nint(x);
    xpbuf[npt++].y = FL_nint(y);
#else
    xpbuf[npt].x = x;
    xpbuf[npt++].y = y;
#endif
}

void
fl_add_float_vertex(float x, float y)
{
    xpbuf[npt].x = FL_nint(x);
    xpbuf[npt++].y = FL_nint(y);
}

void
fl_reset_vertex(void)
{
    npt = 0;
    pcol = flx->color;
}

void
fl_endline(void)
{
    if (npt >= MAX_BUF_POINT)
	Bark("DoLine", "Vertices Out of bounds");
    fl_lines(xpbuf, npt, flx->color);
}

void
fl_endclosedline(void)
{
    if (npt >= MAX_BUF_POINT)
	Bark("DoPolyLine", "Vertices Out of bounds");
    fl_polyl(xpbuf, npt, pcol);
}

void
fl_endpolygon(void)
{
    if (npt >= MAX_BUF_POINT)
	Bark("DoPoly", "Vertices Out of bounds");
    fl_polyf(xpbuf, npt, flx->color);
}

#define vv2(x1,y1,x2,y2) AddVertex(fp,x1,y1); AddVertex(fp,x2,y2);
#define vv3(x1,y1,x2,y2,x3,y3) vv2(x1,y1,x2,y2); AddVertex(fp,x3,y3);

static int Corner = 3;
int
fl_set_default_tabfolder_corner(int n)
{
    int old = Corner;

    Corner = FL_abs(n);
    if (Corner > 10)
	Corner = 10;

    return old;
}

static void
fl_foldertab_box(int style, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		 FL_COLOR c, int bw)
{
    int ctr, right, bott;
    FL_POINT vert[10], *fp;
    int border;
    int absbw = FL_abs(bw), i;
    int C = Corner;
    int isbroken = (style & FL_BROKEN_BOX);

    if (!(border = bw > 0))
	bw = -bw;


    style &= ~FL_BROKEN_BOX;

    /* for foldertab box, actual h is pre-enlarged by absbw pixels so the
       label is draw centered. Here we recover the pixels */

    if (style == FL_BOTTOMTAB_UPBOX || style == FL_SELECTED_BOTTOMTAB_UPBOX)
	h += absbw;
    else
	h -= absbw;

/* must guarante the width of rectangle > 0 */
    if ((w - 2 * bw) <= 0)
	bw = w / 2 - 1;
    if ((h - 2 * bw) <= 0)
	bw = h / 2 - 1;

    ctr = absbw / 2;
    Shrink(x, y, w, h, ctr);

    right = x + w - 1;
    bott = y + h - 1;
    fp = vert;

    switch (style)
    {
    case FL_TOPTAB_UPBOX:
	vv3(x, y + h - (ctr == 0), x, y + C - 1, x + C - 1, y);
	vv2(right - C, y, x + C - 1, y);
	vv3(right - C + 1, y, right, y + C - 1, right, y + h - (ctr == 0));
	fl_polyf(vert, 8, c);
	fl_set_linewidth(absbw);
	fl_lines(vert, 3, FL_LEFT_BCOL);
	fl_lines(vert + 3, 2, FL_TOP_BCOL);
	if (!isbroken)
	    fl_lines(vert + 5, 3, FL_BOTTOM_BCOL);
	else
	{
	    int yc = (h - (ctr == 0)) / 2, yc2 = yc / 2, yc4 = yc / 4;
	    fl_line(right, y, right, y + 4, FL_INACTIVE_COL);
	    fl_line(right, y + 4, right - 3, y + yc4, FL_INACTIVE_COL);
	    fl_line(right - 3, y + yc4, right - 2, y + yc2, FL_INACTIVE_COL);
	    fl_line(right - 2, y + yc2, right, y + yc, FL_INACTIVE_COL);
	    fl_line(right, y + yc, right, y + yc + 2, FL_INACTIVE_COL);
	    fl_line(right, y + yc + 2, right + 1, y + yc + yc4, FL_INACTIVE_COL);
	    fl_line(right + 1, y + yc + yc4, right - 2, y + yc + yc, FL_INACTIVE_COL);
	}

	fl_set_linewidth(0);

	if (border || fl_dithered(fl_vmode))
	{
	    for (i = 0; i < 8; i++)
	    {
		if (vert[i].x > (x + w / 2))
		    vert[i].x += ctr;
		else
		    vert[i].x -= ctr + 1;

		if (vert[i].y < (y + h / 2))
		    vert[i].y -= ctr + 1;
	    }
	    fl_lines(vert, 8, FL_RIGHT_BCOL);
	}
	break;
    case FL_SELECTED_TOPTAB_UPBOX:
	vv3(x, bott + absbw + 3, x, y + C - 1, x + C - 1, y);
	vv2(right - C, y, x + C - 1, y);
	vv3(right - C + 1, y, right, y + C - 1, right, bott + absbw + 3);
	fl_polyf(vert, 8, c);
	fl_set_linewidth(absbw);
	fl_lines(vert, 3, FL_LEFT_BCOL);
	fl_lines(vert + 3, 2, FL_TOP_BCOL);
	fl_lines(vert + 5, 3, FL_BOTTOM_BCOL);
	fl_set_linewidth(0);

	if (border || fl_dithered(fl_vmode))
	{
	    for (i = 0; i < 8; i++)
	    {
		if (vert[i].x > (x + w / 2))
		    vert[i].x += ctr;
		else
		    vert[i].x -= ctr + 1;

		if (vert[i].y < (y + h / 2))
		    vert[i].y -= ctr + 1;
	    }
	    vert[0].y -= absbw + 1;
	    vert[7].y -= absbw + 1;
	    fl_lines(vert, 8, FL_RIGHT_BCOL);
	}
	break;
    case FL_BOTTOMTAB_UPBOX:
	vv3(x, y + (ctr == 0), x, bott - C + 1, x + C - 1, bott);
	vv2(x + C - 1, bott, right - C, bott);
	vv3(right - C, bott, right, bott - C, right, y + (ctr == 0));

	fl_polyf(vert, 8, c);
	fl_linewidth(absbw);
	fl_lines(vert, 3, FL_TOP_BCOL);
	fl_lines(vert + 3, 2, FL_BOTTOM_BCOL);
	fl_lines(vert + 5, 3, FL_RIGHT_BCOL);
	fl_linewidth(0);

	if (border || fl_dithered(fl_vmode))
	{
	    for (i = 0; i < 8; i++)
	    {
		if (vert[i].x > (x + w / 2))
		    vert[i].x += ctr;
		else
		    vert[i].x -= ctr + 1;
		if (vert[i].y > (y + h / 2))
		    vert[i].y += ctr;
	    }
	    fl_lines(vert, 8, FL_RIGHT_BCOL);
	}

	break;
    case FL_SELECTED_BOTTOMTAB_UPBOX:
	vv3(x, y - absbw - 1, x, bott - C + 1, x + C - 1, bott);
	vv2(x + C - 1, bott, right - C, bott);
	vv3(right - C, bott, right, bott - C + 1, right, y - absbw - 1);

	fl_polyf(vert, 8, c);
	fl_linewidth(absbw);
	fl_lines(vert, 3, FL_TOP_BCOL);
	fl_lines(vert + 3, 2, FL_BOTTOM_BCOL);
	fl_lines(vert + 5, 3, FL_RIGHT_BCOL);
	fl_linewidth(0);

	if (border || fl_dithered(fl_vmode))
	{
	    for (i = 0; i < 8; i++)
	    {
		if (vert[i].x > (x + w / 2))
		    vert[i].x += ctr;
		else
		    vert[i].x -= ctr + 1;
		if (vert[i].y > (y + h / 2))
		    vert[i].y += ctr;
	    }

	    fl_lines(vert, 8, FL_RIGHT_BCOL);
	}
	break;
    default:
	break;
    }
}

/*

 * draw triangluar box
 *
 */

void
fl_drw_tbox(int style, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
	    FL_COLOR c, int bw_in)
{
    FL_POINT vert[5], *fp;
    int dp = fl_dithered(fl_vmode), bw = bw_in;
    int xc = x + w / 2, yc = y + h / 2;
    int halfbw = bw / 2;

    if (c == FL_NoColor)
	c = FL_COL1;

    if ( bw < 0)
	bw = -bw;

    if (bw == 0 && style != FL_NO_BOX)
	style = FL_FLAT_BOX;

    fp = vert;
    switch (style)
    {
    case FL_TRIANGLE_UPBOX8:
	AddVertex(fp, xc, y + bw);
	AddVertex(fp, x + bw, y + h - bw);
	AddVertex(fp, x + w - bw, y + h - bw);
	fl_polyf(vert, 3, c);
	fl_linewidth(bw);
	Shrink(x, y, w, h, halfbw);
	xc = x + w / 2;
	fl_line(xc, y, x, y + h - 1, FL_LEFT_BCOL);
	fl_line(x, y + h - 1, x + w - 1, y + h - 1, FL_BOTTOM_BCOL);
	fl_line(xc, y, x + w - 1, y + h - 1, FL_BOTTOM_BCOL);
	fl_linewidth(0);
	break;
    case FL_TRIANGLE_DOWNBOX8:
	AddVertex(fp, xc, y + bw);
	AddVertex(fp, x + bw, y + h - bw);
	AddVertex(fp, x + w - bw, y + h - bw);
	fl_polyf(vert, 3, c);
	fl_linewidth(bw);
	Shrink(x, y, w, h, halfbw);
	xc = x + w / 2;
	fl_line(xc, y, x, y + h - 1, FL_BOTTOM_BCOL);
	fl_line(x, y + h - 1, x + w - 1, y + h - 1, FL_TOP_BCOL);
	fl_line(xc, y, x + w - 1, y + h - 1, FL_LEFT_BCOL);
	fl_linewidth(0);
	break;
    case FL_TRIANGLE_UPBOX2:
	AddVertex(fp, xc, y + h - bw);
	AddVertex(fp, x + bw, y + bw);
	AddVertex(fp, x + w - bw, y + bw);
	fl_polyf(vert, 3, c);
	fl_linewidth(bw);
	Shrink(x, y, w, h, halfbw);
	xc = x + w / 2;
	fl_line(xc, y + h - 1, x, y, FL_LEFT_BCOL);
	fl_line(x, y, x + w - 1, y, FL_TOP_BCOL);
	fl_line(x + w - 1, y, xc, y + h - 1, FL_RIGHT_BCOL);
	fl_linewidth(0);
	break;
    case FL_TRIANGLE_DOWNBOX2:
	AddVertex(fp, xc, y + h - bw);
	AddVertex(fp, x + bw, y + bw);
	AddVertex(fp, x + w - bw, y + bw);
	fl_polyf(vert, 3, c);
	fl_linewidth(bw);
	Shrink(x, y, w, h, halfbw);
	xc = x + w / 2;
	fl_line(xc, y + h - 1, x, y, FL_BOTTOM_BCOL);
	fl_line(x, y, x + w - 1, y, FL_BOTTOM_BCOL);
	fl_line(x + w - 1, y, xc, y + h - 1, FL_TOP_BCOL);
	fl_linewidth(0);
	break;
    case FL_TRIANGLE_UPBOX4:
	AddVertex(fp, x + bw, yc);
	AddVertex(fp, x + w - bw, y + bw);
	AddVertex(fp, x + w - bw, y + h - bw);
	fl_polyf(vert, 3, c);
	fl_linewidth(bw);
	Shrink(x, y, w, h, halfbw);
	yc = y + h / 2;
	fl_line(x, yc, x + w - 1, y, FL_TOP_BCOL);
	fl_line(x + w - 1, y, x + w - 1, y + h - 1, FL_RIGHT_BCOL);
	fl_line(x + w - 1, y + h - 1, x, yc, FL_BOTTOM_BCOL);
	fl_linewidth(0);
	break;
    case FL_TRIANGLE_DOWNBOX4:
	AddVertex(fp, x + bw, yc);
	AddVertex(fp, x + w - bw, y + bw);
	AddVertex(fp, x + w - bw, y + h - bw);
	fl_polyf(vert, 3, c);
	fl_linewidth(bw);
	Shrink(x, y, w, h, halfbw);
	yc = y + h / 2;
	fl_line(x, yc, x + w - 1, y, FL_BOTTOM_BCOL);
	fl_line(x + w - 1, y, x + w - 1, y + h - 1, FL_LEFT_BCOL);
	fl_line(x + w - 1, y + h - 1, x, yc, FL_LEFT_BCOL);
	fl_linewidth(0);
	break;
    case FL_TRIANGLE_UPBOX6:
	AddVertex(fp, x + bw, y + bw);
	AddVertex(fp, x + w - bw, yc);
	AddVertex(fp, x + bw, y + h - bw);
	fl_polyf(vert, 3, c);
	fl_linewidth(bw);
	Shrink(x, y, w, h, halfbw);
	yc = y + h / 2;
	fl_line(x, y, x + w - 1, yc, FL_RIGHT_BCOL);
	fl_line(x + w - 1, yc, x, y + h - 1, FL_BOTTOM_BCOL);
	fl_line(x, y + h - 1, x, y, FL_LEFT_BCOL);
	fl_linewidth(0);
	break;
    case FL_TRIANGLE_DOWNBOX6:
	AddVertex(fp, x + bw, y + bw);
	AddVertex(fp, x + w - bw, yc);
	AddVertex(fp, x + bw, y + h - bw);
	fl_polyf(vert, 3, c);
	fl_linewidth(bw);
	Shrink(x, y, w, h, halfbw);
	yc = y + h / 2;
	fl_line(x, y, x + w - 1, yc, FL_LEFT_BCOL);
	fl_line(x + w - 1, yc, x, y + h - 1, FL_TOP_BCOL);
	fl_line(x, y + h - 1, x, y, FL_RIGHT_BCOL);
	fl_linewidth(0);
	break;
    }

    /* handle black and white */

    if (dp)
	fl_polyl(vert, 3, FL_BLACK);
}


void
fl_drw_broken_box(int style, int x, int y, int w, int h,
		  FL_COLOR c, int bw)
{
    fl_drw_box(FL_UP_BOX, x, y, w, h, c, bw);

    fl_line(x + w - 1, y, x + w - 6, y + 6, FL_RIGHT_BCOL);
}

int
fl_boxtype2frametype(int btype)
{
    if (btype <= FL_EMBOSSED_BOX)
	return btype;
    else if (btype == FL_FLAT_BOX)
        return FL_NO_FRAME;
    else
	return FL_DOWN_FRAME;
}
