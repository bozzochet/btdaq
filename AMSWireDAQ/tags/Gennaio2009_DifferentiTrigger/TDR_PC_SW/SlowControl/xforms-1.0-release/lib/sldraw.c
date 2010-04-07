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
 * $Id: sldraw.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 *
 */
#if defined(F_ID) || defined(DEBUG)
char *fl_id_sldr = "$Id: sldraw.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include "forms.h"
#include "pslider.h"		/* for FL_MINKNOB */

#if 1
#define flinear(val, smin, smax, gmin, gmax)                    \
        ((smin) == (smax)) ? (gmax):                            \
	((gmin) + ((gmax) - (gmin)) * ((val) - (smin)) / ((smax) - (smin)))
#else
static float
flinear(float val, float smin, float smax, float gmin, float gmax) \
{
    return (smin == smax) ? gmax :
	(gmin + (gmax - gmin) * (val - smin) / (smax - smin));
}

#endif


#define SLB  FL_SCROLL_BAR


/* minimum knob size */
#define MINKNOB_SB    FL_MINKNOB_SB
#define MINKNOB_SL    FL_MINKNOB_SL


/* sliders that must have certain minimum value for the knob */
#define SLIDER_MIN(t) (t==FL_HOR_BROWSER_SLIDER   ||     \
                       t==FL_HOR_BROWSER_SLIDER2  ||     \
                       t==FL_HOR_THIN_SLIDER      ||     \
                       t==FL_VERT_BROWSER_SLIDER  ||     \
                       t==FL_VERT_BROWSER_SLIDER2 ||     \
                       t==FL_VERT_THIN_SLIDER)


#define IS_FLATBOX(t)  (t==FL_FRAME_BOX || t==FL_EMBOSSED_BOX || \
			t==FL_BORDER_BOX ||t==FL_ROUNDED_BOX)
#define IS_FLATORDOWN(t) (IS_FLATBOX(t) || t==FL_DOWN_BOX)

#define IS_FLATORUPBOX(t) (IS_FLATBOX(t) || t==FL_UP_BOX)

void
fl_calc_slider_size(int boxtype, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		    int sltype, float size, float val, SLB * slb,
		    int inv, FL_Coord bw)
{

    int absbw = FL_abs(bw);
    int fudge1 = IS_FLATORUPBOX(boxtype) ? 0 : ((bw == -2) || (bw >= 2));
    int fudge2 = IS_FLATORUPBOX(boxtype) ? 0 : (2 * (bw == -2) + (bw >= 2));

    slb->buttw = slb->butth = 0;

    if (sltype == FL_VERT_FILL_SLIDER)
    {
	slb->h = (inv ? (1 - val) : val) * (h - 2 * absbw);
	slb->y = inv ? (y + h - absbw - slb->h) : (y + absbw);
	slb->w = w - 2 * absbw;
	slb->x = x + absbw;
    }
    else if (sltype == FL_HOR_FILL_SLIDER)
    {
	slb->w = val * (w - 2 * absbw);
	slb->x = x + absbw;
	slb->h = h - 2 * absbw;
	slb->y = y + absbw;
    }
    else if (is_vslider(sltype))
    {
	slb->h = size * (h - 2 * absbw);

	if (SLIDER_MIN(sltype) && slb->h < MINKNOB_SB)
	    slb->h = MINKNOB_SB;
	else if (slb->h < 2 * absbw + MINKNOB_SL)
	    slb->h = 2 * absbw + MINKNOB_SL;

	if (sltype == FL_VERT_BROWSER_SLIDER2)
	{
	    slb->h = size * h;
	    slb->y = flinear(val, 0.0f, 1.0f, (float) y, (float) (y + h - slb->h));
	    slb->x = x + 1 + IS_FLATORDOWN(boxtype);
	    slb->w = w - 2 - 2 * IS_FLATORDOWN(boxtype);
	}
	else if (sltype == FL_VERT_THIN_SLIDER ||
		 sltype == FL_VERT_BASIC_SLIDER)
	{
	    slb->h = size * h;
	    slb->y = flinear(val, 0.0f, 1.0f, y, y + h - slb->h);
	    slb->w = w + fudge2;
	    slb->x = x - fudge1;
	}
	else
	{
	    slb->y = flinear(val, 0.0f, 1.0f, (float) (y + absbw),
			     (float) (y + h - absbw - slb->h));

	    slb->w = w - 2 * absbw;
	    slb->x = x + absbw;
	}
    }
    else if (is_hslider(sltype))
    {
	slb->w = size * (w - 2 * absbw);

	if (SLIDER_MIN(sltype) && slb->w < MINKNOB_SB)
	    slb->w = MINKNOB_SB;
	else if (slb->w < 2 * absbw + MINKNOB_SL)
	    slb->w = 2 * absbw + MINKNOB_SL;

	if (sltype == FL_HOR_BROWSER_SLIDER2)
	{
	    slb->w = size * w;
	    slb->x = flinear(val, 0.0f, 1.0f, x, x + w - slb->w);
	    slb->h = h - 2 * (1 + IS_FLATORDOWN(boxtype));
	    slb->y = y + 1 + IS_FLATORDOWN(boxtype);
	}
	else if (sltype == FL_HOR_THIN_SLIDER ||
		 sltype == FL_HOR_BASIC_SLIDER)
	{
	    slb->w = size * w;
	    slb->x = flinear(val, 0.0f, 1.0f, x, x + w - slb->w);
	    slb->h = h + fudge2;	/* - 2 * absbw; */
	    slb->y = y - fudge1;	/* + absbw; */
	}
	else
	{
	    slb->x = flinear(val, 0.0f, 1.0f,
			     (float) (x + absbw),
			     (float) (x + w - absbw - slb->w));
	    slb->h = h - 2 * absbw;
	    slb->y = y + absbw;
	}
    }
    else
    {
	M_err("CalcSize", "Bad slider type:%d", sltype);
	return;
    }
}

int
fl_slider_mouse_object(int boxtype, FL_Coord x, FL_Coord y,
		       FL_Coord w, FL_Coord h,
		       int sltype, float size, float val,
		       FL_Coord mx, FL_Coord my, int bw)
{
    SLB slb;

    fl_calc_slider_size(boxtype, x, y, w, h, sltype, size, val, &slb, 0, bw);

    if (mx >= (slb.x) && mx <= (slb.x + slb.w) &&
	my >= slb.y && my <= (slb.y + slb.h))
	return FL_SLIDER_KNOB;

    return FL_SLIDER_NONE;
}

/* val is normalized betweew 0 and 1 */
void
fl_drw_slider(int boxtype, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
	      FL_COLOR col1, FL_COLOR col2, int sltype,
	      double size, double val, char *str, int d,
	      int inv, FL_Coord bw)

{
    FL_Coord xsl, ysl, hsl, wsl;
    FL_Coord  absbw = FL_abs(bw), bw2, absbw2;
    int slbox;
    SLB slb;

    fl_calc_slider_size(boxtype, x, y, w, h, sltype, size, val,
			&slb, inv, bw);

    xsl = slb.x;
    ysl = slb.y;
    wsl = slb.w;
    hsl = slb.h;

    /* Draw the slider */

    if ((d & FL_SLIDER_BOX))
    {
	int btype = boxtype;
	int actual_bw = bw;

	if (sltype == FL_VERT_BROWSER_SLIDER2 ||
	    sltype == FL_HOR_BROWSER_SLIDER2)
	{
/*          btype = FL_UP_BOX; */
	    actual_bw = bw > 0 ? 1 : -1;
	}
	else if (sltype == FL_VERT_THIN_SLIDER ||
		 sltype == FL_VERT_BASIC_SLIDER)
	{
	    btype = FL_FLAT_BOX;
	    actual_bw = bw > 0 ? 1 : -1;
#if 0
	    col1 = FL_RED;
	    y -= 1;
	    h += 2;
	    x += 1;
	    w -= 2;
#endif
	}
	else if (sltype == FL_HOR_THIN_SLIDER ||
		 sltype == FL_HOR_BASIC_SLIDER)
	{
	    btype = FL_FLAT_BOX;
	    actual_bw = 1;
#if 0
	    col1 = FL_RED;
	    x -= 1;
	    w += 2;
	    y += absbw;
	    h -= 2 * absbw;
#endif
	}
	else
	{
	    btype = btype == FL_SHADOW_BOX ? FL_BORDER_BOX : btype;
	}

	fl_drw_box(btype, x, y, w, h, col1, actual_bw);
    }


    if (sltype == FL_VERT_NICE_SLIDER)
    {
	fl_drw_box(FL_FLAT_BOX, x + w / 2 - 2, y + absbw,
		   4, h - 2 * absbw, FL_BLACK, 0);
	fl_drw_box(FL_UP_BOX, xsl, ysl, wsl, hsl, col1,
		   IS_FLATBOX(boxtype) ? -1 : bw);
	fl_drw_box(FL_DOWN_BOX, xsl + 2, ysl + hsl / 2 - 2,
		   wsl - 4, 5, col2, 1);
    }
    else if (sltype == FL_HOR_NICE_SLIDER)
    {
	int yoff = hsl > 15 ? 3 : 2;

	fl_drw_box(FL_FLAT_BOX, x + absbw, y + h / 2 - 2,
		   w - 2 * absbw, 4, FL_BLACK, 0);
	fl_drw_box(FL_UP_BOX, xsl, ysl, wsl, hsl, col1,
		   IS_FLATBOX(boxtype) ? -1 : bw);
	fl_drw_box(FL_DOWN_BOX, xsl + wsl / 2 - 2, ysl + yoff,
		   5, hsl - 2 * yoff, col2, 1 /* absbw - 2 */ );
    }
    else
    {
	switch (boxtype)
	{
	case FL_UP_BOX:
	    slbox = FL_UP_BOX;
	    break;
	case FL_DOWN_BOX:
	    slbox = FL_UP_BOX;
	    break;
	case FL_FRAME_BOX:
	case FL_EMBOSSED_BOX:
	    slbox = boxtype;
	    break;
	case FL_ROUNDED_BOX:
	    slbox = FL_ROUNDED_BOX;
	    break;
	case FL_RFLAT_BOX:
	    slbox = FL_ROUNDED_BOX;
	    break;
	case FL_RSHADOW_BOX:
	    slbox = FL_ROUNDED_BOX;
	    break;
	default:
	    slbox = FL_BORDER_BOX;
	    break;
	}

	/* this is the height of the sliding bar */
	absbw2 = absbw >= 2 ? (absbw - 1) : (absbw - (bw < 0));
	if (absbw2 == 0)
	    absbw2 = 1;
	bw2 = bw > 0 ? absbw2 : (-absbw2);

	if (sltype == FL_VERT_THIN_SLIDER ||
	    sltype == FL_HOR_THIN_SLIDER ||
	    sltype == FL_VERT_BASIC_SLIDER ||
	    sltype == FL_HOR_BASIC_SLIDER)
	{
	    absbw2 = absbw - (absbw > 2) - (bw == 2);
	    if (absbw2 == 0)
		absbw2 = 1;
	    bw2 = bw > 0 ? absbw2 : (-absbw2);
	}

	if (sltype == FL_HOR_THIN_SLIDER)
	    sltype = FL_HOR_BROWSER_SLIDER2;
	if (sltype == FL_VERT_THIN_SLIDER)
	    sltype = FL_VERT_BROWSER_SLIDER2;

	if ((d & FL_SLIDER_KNOB))
	{

	    fl_drw_box(slbox, xsl, ysl, wsl, hsl, col2, bw2);

	    if (sltype == FL_VERT_BROWSER_SLIDER ||
		sltype == FL_VERT_BROWSER_SLIDER2)
	    {
		/* if soft edge, we can squeeze one more pixel */
		int extra = bw2 < 0;
		fl_drw_text(FL_ALIGN_CENTER,
			    xsl - extra,
			    ysl, wsl + 2 * extra,
			    hsl, 0,
			    FL_NORMAL_STYLE, FL_TINY_SIZE, "@RippleLines");
	    }
	    else if (sltype == FL_HOR_BROWSER_SLIDER ||
		     sltype == FL_HOR_BROWSER_SLIDER2)
	    {
		fl_drw_text(FL_ALIGN_CENTER, xsl - 1, ysl,
			    wsl, hsl, 0, 10, 1, "@2RippleLines");
	    }
	}

    }

    if (!str || !*str)
       return;

    /* Draw the label */
    fl_drw_text(FL_ALIGN_CENTER, xsl, ysl, wsl, hsl, 0, FL_NORMAL_STYLE,
		FL_TINY_FONT, str);
}

/***
  Routine to determine the value indicated by the mouse position.
  The routine return the following possible value
	-2  do a page down (mouse click was below sliding part)
	-1  do a line down (mouse click in down button if available (not yet))
	 0  simple adapt value (mouse click in sliding part)
	 1  line up
	 2  page up
  In all cases, in newval the new value is return as if 0 was returned
  (i.e., you can ignore the returned value and simple set the slider
  to the new value if you don't want page down's etc.).
***/

int
fl_get_pos_in_slider(FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		     int sltype,	/* Slider type */
		     float size,	/* Slider size */
		     FL_Coord xpos, FL_Coord ypos,	/* Mouse position */
		     float oldval,	/* The old value */
		     float *newval	/* The new value */
)
{
    float v = 0, tmp;
    int ret = 0;

    if (sltype == FL_HOR_THIN_SLIDER || sltype == FL_HOR_BASIC_SLIDER)
	sltype = FL_HOR_BROWSER_SLIDER2;
    if (sltype == FL_VERT_THIN_SLIDER || sltype == FL_VERT_BASIC_SLIDER)
	sltype = FL_VERT_BROWSER_SLIDER2;

    xpos -= x + FL_SLIDER_BW2;
    ypos -= y + FL_SLIDER_BW2;
    h -= 2.0f * FL_SLIDER_BW2;
    w -= 2.0f * FL_SLIDER_BW2;

    if (sltype == FL_VERT_FILL_SLIDER)
    {
	v = flinear((float) ypos, 0.0f, (float) h, 0.0f, 1.);
    }
    else if (sltype == FL_HOR_FILL_SLIDER)
    {
	v = flinear((float) xpos, 0.0f, (float) w, 0.0f, 1.);
    }
    else if (sltype == FL_VERT_SLIDER ||
	     sltype == FL_VERT_NICE_SLIDER ||
	     sltype == FL_VERT_BROWSER_SLIDER ||
	     sltype == FL_VERT_BROWSER_SLIDER2)
    {
	FL_Coord hsl = size * h;
	tmp = hsl * 0.5f;

	if (sltype == FL_VERT_BROWSER_SLIDER && hsl < MINKNOB_SB)
	    hsl = MINKNOB_SB;

	v = flinear((float) ypos, tmp, h - tmp, 0.0f, 1.0f);

	if (ypos < oldval * (h - hsl))
	    ret = 2;
	else if (ypos > (oldval * (h - hsl) + hsl))
	    ret = -2;

    }
    else if (is_hslider(sltype))
    {
	tmp = size * w;
	v = flinear(xpos, 0.5f * tmp, w - 0.5f * tmp, 0.0f, 1.0f);
	if (xpos < oldval * (w - tmp))
	    ret = -2;
	else if (xpos > (oldval * (w - tmp) + tmp))
	    ret = 2;
    }
    else
    {
	M_err("", "bad slider type");
    }

    if (v < 0.0f)
	v = 0.0f;
    if (v > 1.0f)
	v = 1.0f;

    if (shiftkey_down(fl_keymask))
	*newval = oldval + (v - oldval) * FL_SLIDER_FINE;
    else
	*newval = v;

    return ret;
}
