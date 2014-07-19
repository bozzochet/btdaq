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
 * $Id: ps_core.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1997-2002  by T.C. Zhao
 *  All rights reserved.
 *.
 *
 * Some primitive drawing routines is PS
 *
 *
 *  (x,y) (w,h) passed to all drawing functions are relative
 *  to PS coordinate system and the unit is pixel/point.
 */

#include "forms.h"
#include "flinternal.h"
#include "pflps.h"
#include <ctype.h>
#include <math.h>

FLPSInfo *flps;

FLPS_CONTROL *
flps_init(void)
{
    static FLPSInfo *local_flps;

    if (!local_flps)
    {
	local_flps = fl_calloc(1, sizeof(*flps));
	local_flps->ps_color = FLPS_COLOR;
	local_flps->orientation = FLPS_AUTO;
	local_flps->paper_w = 8.5;
	local_flps->paper_h = 11.0;
	local_flps->auto_fit = 1;
	local_flps->xscale = local_flps->yscale = 1.0f;
	/* driver will fill-in appropriate vals for minus */
	local_flps->drawbox = -1;
	local_flps->xdpi = local_flps->ydpi = fl_dpi;
	local_flps->pack = 1;
	/* cache */
	local_flps->cur_color = FL_NoColor;
	local_flps->cur_style = local_flps->cur_size = -1;
	local_flps->cur_lw = -1;
    }

    return (FLPS_CONTROL *) (flps = local_flps);
}

void
flps_switch_flps(FLPSInfo * info)
{
    flps = info;
}

void
flps_restore_flps(void)
{
    flps_init();
}

void
flps_reset_cache(void)
{
    flps_invalidate_font_cache();
    flps_invalidate_color_cache();
    flps_invalidate_symbol_cache();
    flps_invalidate_linewidth_cache();
}


/* write some arbitary text into the output stream */
void
flps_log(const char *s)
{
    fprintf(flps->fp, "%%%s", s);
}

#include <stdarg.h>

#define LEFT    '('
#define RIGHT   ')'

void
flps_output(const char *fmt,...)
{
    va_list args;
    char buf[2048], *q;
    int lastc = flps->lastc;

    /* flush */
    if (!fmt)
    {
	if (flps->pack && flps->len)
	{
	    putc('\n', flps->fp);
	    flps->len = 0;
	}
	return;
    }

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    if (!flps->pack)
	fprintf(flps->fp, "%s", buf);
    else
    {
	for (q = buf; *q; q++)
	{
	    if (*q == '\n')
		*q = ' ';

	    /* this is less than robust */
	    if (*q == LEFT)
		flps->literal = 1;
	    else if (*q == RIGHT)
		flps->literal = 0;

	    if (lastc == ' ' && *q == ' ' && !flps->literal)
		continue;

	    if (*q == ' ' && flps->len == 0)
		continue;

	    if (*q == ' ' && flps->len >= 70)
	    {
		putc('\n', flps->fp);
		flps->len = 0;
	    }
	    else
	    {
		lastc = *q;
		flps->len++;
		putc(*q, flps->fp);
	    }
	}
	flps->lastc = lastc;
    }
}

/* linewidth and reset linewidth must be used in pairs */
void
flps_linewidth(int lw)
{
    float flw;

    if (lw == flps->cur_lw)
	return;

    flps->last_lw = flps->cur_lw;
    flps->cur_lw = lw;

    flw = 0.9f * lw;
    if ((flps->final_xscale + flps->final_yscale) > 2)
	flw = 0.9f * lw;

    if (flw < 0.4f)
	flw = 0.4f;

    flps_output(" %.1f %.1f LW\n", flw, flw);
}

int
flps_get_linewidth(void)
{
    return flps->cur_lw;
}

void
flps_reset_linewidth(void)
{
    flps_linewidth(flps->last_lw);
    flps_log("resetline");
}

void
flps_invalidate_linewidth_cache(void)
{
    flps->cur_lw = -1;
}

static int ps_ls = FL_SOLID;

void
flps_linestyle(int n)
{
    if (ps_ls == n)
	return;

    switch (n)
    {
    case FL_DOT:
	flps_output("DT ");
	break;
    case FL_DOTDASH:
	flps_output("DTD ");
	break;
    case FL_DASH:
	flps_output("D ");
	break;
    case FL_LONGDASH:
	flps_output("LD ");
	break;
    case FL_SOLID:
    case FL_USERDASH:
    case FL_USERDOUBLEDASH:
    case -1:
	flps_output("SL ");
	ps_ls = FL_SOLID;
	break;
    default:
	fprintf(stderr, "Unknown dashstyle: %d\n", n);
	return;
    }

    ps_ls = n;
}

int
flps_get_linestyle(void)
{
    return ps_ls;
}

/*
 * Draw a line in abs coordinate system
 */
void
flps_line(int xi, int yi, int xf, int yf, long col)
{
    flps_color(col);
    flps_output("%d %d %d %d L S\n", xf, yf, xi, yi);
}

/* ghostview and some printers have problems with too many
 * points on the stack. We break the points into batches of
 * PSMAXP pairs
 */
#define PSMAXP    350

static void
small_flps_lines(FL_POINT * xp, int n, long col)
{
    FL_POINT *xps = xp + n;
    int cnt = 1;

    flps_color(col);
    for (; xp < xps; xp++, cnt++)
    {
	flps_output("%d %d ", (int) xp->x, (int) xp->y);
	if ((cnt % 6) == 0)
	    flps_output("\n");
    }

    if (n)
	flps_output("%d lines\n", n);
}

void
flps_lines(FL_POINT * xp, int n, long col)
{
    int k = n / PSMAXP, r = n % PSMAXP;
    int b;

    /* need to backup one point if broken up the batch */
    for (b = 0; b < k; b++)
	small_flps_lines(xp + b * PSMAXP - (b > 0), PSMAXP + (b > 0), col);

    if (r)
	small_flps_lines(xp + k * PSMAXP - (k > 0), r + (k > 0), col);
}


void
flps_poly(int fill, FL_POINT * xp, int n, long col)
{
    FL_POINT *xps = xp + n;
    int cnt = 1;

    flps_color(col);

    for (; xp < xps; xp++, cnt++)
    {
	flps_output("%d %d ", (int) xp->x, (int) xp->y);
	if ((cnt % 6) == 0)
	    flps_output("\n");
    }
    flps_output("%d P %c\n", n, "SF"[fill]);
}

void
flps_rectangle(int fill, int x, int y, int w, int h, long col)
{
    flps_color(col);
    flps_output("%d %d %d %d %d %d %d %d 4 P",
		x, y, x, y + h - 1, x + w - 1, y + h - 1, x + w - 1, y);
    flps_output(" %c\n", fill["SF"]);
}


void
flps_oval(int fill, int x, int y, int w, int h, long col)
{
    flps_pieslice(fill, x, y, w, h, 0.0f, 3600.0f, col);
}

void
flps_circ(int fill, int x, int y, int r, long col)
{
    flps_color(col);
    flps_output("newpath %d %d %d 0 360 arc %c\n", x, y, r, "SF"[fill]);
}

/* draw an circular arc, optionally filled. Angle t1 and t2 are in one-tenth of
 * a degree
 */
void
flps_arc(int fill, int x, int y, int r, int t1, int t2, long col)
{
    flps_color(col);
    flps_output("newpath %d %d %d %.1f %.1f arc %c\n",
		x, y, r, t1 * 0.1f, t2 * 0.1f, "SF"[fill]);
}

/* only fill does a pie slice */
void
flps_pieslice(int fill, int x, int y, int w, int h,
	      int t1, int t2, long col)
{
    float sx = 1.0f, sy = (float) h / w;

    flps_color(col);
    flps_output("gsave newpath %.1f %.1f translate %.1f %.1f scale\n",
		x + 0.5f * w, y + 0.5f * h, sx, sy);
    if (!fill)
	flps_output("0 0 %.1f %.1f %.1f arc S grestore\n",
		    w * 0.5f, t1 * 0.1f, t2 * 0.1f);
    else
	flps_output("0 0 M 0 0 %.1f %.1f %.1f arc C F grestore\n",
		    w * 0.5f, t1 * 0.1f, t2 * 0.1f);

    flps_invalidate_color_cache();
}

/**************************************************************
 * basic color routines
 **************************************************************/

#define VN(a)    a,#a
#define NV(a)    #a,a

/* slightly different from XForms' built-in due to the difference
 * between  printer and screen
 */

#define PREVIEW

static FL_IMAP fl_imap[] =
{
    {NV(FL_BLACK), 0, 0, 0},
    {NV(FL_WHITE), 255, 255, 255},

    {NV(FL_COL1), 161, 161, 161},	/* default color, gray63 */
    {NV(FL_MCOL), 191, 191, 191},	/* used as magic, gray75 */
    {NV(FL_RIGHT_BCOL), 41, 41, 41},	/* right  gray16  */
    {NV(FL_BOTTOM_BCOL), 89, 89, 89},	/* bottom  gray35 */
    {NV(FL_LEFT_BCOL), 222, 222, 222},	/* left color  gray87 */

    {NV(FL_SLATEBLUE), 113, 113, 198},
    {NV(FL_INDIANRED), 198, 113, 113},

    {NV(FL_RED), 255, 0, 0},
    {NV(FL_BLUE), 0, 0, 255},
    {NV(FL_GREEN), 0, 255, 0},
    {NV(FL_YELLOW), 255, 255, 0},
    {NV(FL_MAGENTA), 255, 0, 255},
    {NV(FL_CYAN), 0, 255, 255},
    {NV(FL_TOMATO), 255, 99, 71},

    {NV(FL_INACTIVE), 110, 110, 110},	/* gray43       */
    {NV(FL_TOP_BCOL), 204, 204, 204},	/* top  gray80  */

    {NV(FL_PALEGREEN), 113, 198, 113},
    {NV(FL_DARKGOLD), 205, 149, 10},
    {NV(FL_ORCHID), 205, 105, 201},
    {NV(FL_DARKCYAN), 40, 170, 175},
    {NV(FL_DARKTOMATO), 139, 54, 38},
    {NV(FL_WHEAT), 255, 231, 155},
    {NV(FL_DARKORANGE), 255, 128, 0},
    {NV(FL_DEEPPINK), 255, 0, 128},
    {NV(FL_CHARTREUSE), 128, 255, 0},
    {NV(FL_DARKVIOLET), 128, 0, 255},
    {NV(FL_SPRINGGREEN), 0, 255, 128},
    {NV(FL_DODGERBLUE), 0, 128, 255},
    {NV(FL_DOGERBLUE), 0, 128, 255},
};

#define builtin   sizeof(fl_imap)/sizeof(fl_imap[0])

/*
 * do gamma adjustment
 */

void
flps_apply_gamma(float gamma)
{
    FL_IMAP *fm = fl_imap, *fs;
    float lastgamma = 1.0;

    if (FL_abs(gamma) < 1.0e-3)
    {
	fprintf(stderr, "fd2ps: Bad Gamma value %.2f\n", gamma);
	return;
    }

    for (fs = fm + builtin; fm < fs; fm++)
    {
	if (flps->verbose)
	    fprintf(stderr, "fm->r=%d\n", fm->r);
	fm->r = (0.4 + 255 * (pow((double) fm->r / 255.0, lastgamma / gamma)));
	fm->g = (0.4 + 255 * (pow((double) fm->g / 255.0, lastgamma / gamma)));
	fm->b = (0.4 + 255 * (pow((double) fm->b / 255.0, lastgamma / gamma)));
	if (flps->verbose)
	    fprintf(stderr, "fm->r=%d\n", fm->r);
    }

    lastgamma = gamma;
}


static void
flps_query_imap(long col, int *r, int *g, int *b)
{
    FL_IMAP *flmap = fl_imap, *flmape = flmap + builtin;

    for (; flmap < flmape; flmap++)
	if (col == flmap->index)
	{
	    *r = flmap->r;
	    *g = flmap->g;
	    *b = flmap->b;
	    return;
	}
}


#define  C2NC(c)             (1.0/255.0*c)
#define  rgb2gray(r,g,b)     (0.299f*(r)+0.587f*(g)+0.114f*(b))

#ifndef FL_PACK
#define FL_PACK(r,g,b)        ((b)<<16|(g)<<8|(r))
#define FL_UNPACK(p,r,g,b)    do { r = ((p)&0xff);      \
                                   g=((p)>>8)&0xff);    \
                                   b=((p)>>16) &0xff);  \
                              } while (0)
#endif


void
flps_invalidate_color_cache(void)
{
    flps->cur_color = FL_NoColor;
}

void
flps_color(long color)
{
    int r, g, b;

    if (color == FL_NoColor)
	return;

    if (flps->isRGBColor)
	FL_UNPACK(color, r, g, b);
    else
	flps_query_imap(color, &r, &g, &b);

    if (FL_PACK(r, g, b) != flps->cur_color)
	flps_rgbcolor(r, g, b);
}

void
flps_rgbcolor(int r, int g, int b)
{
    unsigned int packed = FL_PACK(r, g, b);

    if (flps->cur_color == packed)
	return;

    if (flps->ps_color == FLPS_COLOR && (r != g || r != b))
	flps_output("%.3g %.3g %.3g RGB ", C2NC(r), C2NC(g), C2NC(b));
    else if (flps->ps_color == FLPS_BW)
	flps_output("%d G ", (C2NC(rgb2gray(r, g, b)) > 0.62f));
    else
	flps_output("%.3g G ", C2NC(rgb2gray(r, g, b)));

    flps->cur_color = FL_PACK(r, g, b);
}

int
flps_get_gray255(long color)
{
    int r, g, b;

    if (flps->isRGBColor)
	FL_UNPACK(color, r, g, b);
    else
	flps_query_imap(color, &r, &g, &b);

    return (rgb2gray(r, g, b) + 0.1f);
}

int
flps_get_namedcolor(const char *s)
{
    FL_IMAP *flmap = fl_imap, *flmape = flmap + builtin;

    for (; s && flmap < flmape; flmap++)
	if (strcmp(s, flmap->name) == 0)
	    return flmap->index;
    /* a wild shot */
    return atoi(s);
}

/************** Handle built-in symbols. ****************{*/

typedef void (*PSdrawit) (int, int, int, int, int, long);

typedef struct
{
    const char *name;
    const char *psname;
    const char *draw;
    int otherdef;
    PSdrawit fdrawit;
    int abs_coordinate;		/* fdrawit uses abs coordinate system */
    int defined;		/* if def code emitted                */
}
PS_draw;

static void
draw_dnline(int x, int y, int w, int h, int angle, long col)
{
    float t = 0.20f, len;

    flps_output("%.2f %.2f LW ", 3.5 / (w + h), 3.5 / (w + h));
    if ((w + h) > 200)
	len = 0.99f;
    else if ((w + h) > 150)
	len = 0.98f;
    else
	len = 0.93f;

    flps_color(FL_RIGHT_BCOL);
    flps_output("-%.3f %.2f M %.3f %.2f LT S\n", len, t, len, t);
    flps_color(FL_LEFT_BCOL);
    flps_output("-%.3f -%.2f M %.3f -%.2f LT S\n", len, t, len, t);
}

static void
draw_upline(int x, int y, int w, int h, int angle, long col)
{
    float t = 0.033f;

    flps_color(FL_LEFT_BCOL);
    flps_output("%.3f %.3f LW ", 3.7f / (w + h), 3.7f / (w + h));
    flps_output("-0.9 %.3f M 0.9 %.3f LT S\n", t, t);
    flps_color(FL_RIGHT_BCOL);
    flps_output("0.9 -%.3f M 0.9 %.3f LT S ", t, t);
    flps_output("-0.9 -%.3f M 0.9 -%.3f LT S\n", t, t);
}

/* due to pixel precision problems, we have to draw uparrow, downarrow
 * and ripple line in absolute unit with the origin
 * at the center of the figure.
 */

static void
draw_uparrow(int x, int y, int w, int h, int angle, long col)
{
    float yc = y + h * 0.5f;
    float xc = x + w * 0.5f;
    float dx, dy;
    int d = 3 + (w + h) * 0.06f;

    x += d;
    y += d;
    w -= 2 * d;
    h -= 2 * d;
    dx = w / 2;
    dy = h / 2;

    if (angle == 90)
    {
	flps_line(xc, yc + dy, xc - dx, yc - dy, FL_LEFT_BCOL);
	flps_line(xc - dx, yc - dy, xc + dx, yc - dy, FL_BOTTOM_BCOL);
	flps_line(xc + dx, yc - dy, xc, yc + dy, FL_RIGHT_BCOL);
    }
    else if (angle == 180)
    {
	flps_line(xc - dx, yc, xc + dx, yc + dy, FL_TOP_BCOL);
	flps_line(xc + dx, yc + dy, xc + dx, yc - dy, FL_RIGHT_BCOL);
	flps_line(xc + dx, yc - dy, xc - dx, yc, FL_BOTTOM_BCOL);

    }
    else if (angle == 270)
    {
	flps_line(xc - dx, yc + dy, xc, yc - dy, FL_BOTTOM_BCOL);
	flps_line(xc, yc - dy, xc + dx, yc + dy, FL_RIGHT_BCOL);
	flps_line(xc + dx, yc + dy, xc - dx, yc + dy, FL_TOP_BCOL);

    }
    else
    {
	flps_line(x, yc - dy, x + w, yc, FL_BOTTOM_BCOL);
	flps_line(x, yc + dy, x + w, yc, FL_RIGHT_BCOL);
	flps_line(x, yc - dy, x, yc + dy, FL_LEFT_BCOL);
    }
}

static void
draw_dnarrow(int x, int y, int w, int h, int angle, long col)
{
    float yc = y + h * 0.5f;
    float xc = x + w * 0.5f;
    float dx, dy;
    int d = 3 + (w + h) * 0.06f;

    x += d;
    y += d;
    w -= 2 * d;
    h -= 2 * d;

    dx = w / 2;
    dy = h / 2;

    if (angle == 90)
    {
	flps_line(xc, yc + dy, xc - dx, yc - dy, FL_RIGHT_BCOL);
	flps_line(xc - dx, yc - dy, xc + dx, yc - dy, FL_TOP_BCOL);
	flps_line(xc + dx, yc - dy, xc, yc + dy, FL_TOP_BCOL);
    }
    else if (angle == 180)
    {
	flps_line(xc - dx, yc, xc + dx, yc + dy, FL_RIGHT_BCOL);
	flps_line(xc + dx, yc + dy, xc + dx, yc - dy, FL_LEFT_BCOL);
	flps_line(xc + dx, yc - dy, xc - dx, yc, FL_TOP_BCOL);

    }
    else if (angle == 270)
    {
	flps_line(xc - dx, yc + dy, xc, yc - dy, FL_RIGHT_BCOL);
	flps_line(xc, yc - dy, xc + dx, yc + dy, FL_LEFT_BCOL);
	flps_line(xc + dx, yc + dy, xc - dx, yc + dy, FL_BOTTOM_BCOL);

    }
    else
    {
	flps_line(xc - dx, yc - dy, xc - dx, yc + dy, FL_RIGHT_BCOL);
	flps_line(xc - dx, yc - dy, xc + dx, yc, FL_TOP_BCOL);
	flps_line(xc - dx, yc + dy, xc + dx, yc, FL_BOTTOM_BCOL);
    }
}

/*
 * need single pixel precision, draw in abs. coordiante system
 */
static void
draw_ripple_lines(int x, int y, int w, int h, int angle, long col)
{
    float ym = y + h / 2, ys;
    float xm = x + w / 2, xs;
    int i, mw = FL_BOUND_WIDTH;

    if (h < 14)
	return;

    xs = xm + 5;
    ys = ym + 5;

    if (angle == 90 || angle == 270)
    {
	for (i = 0; i < 3; i++)
	{
	    flps_line(xs, y + mw, xs, y + h - mw - 1, FL_RIGHT_BCOL);
	    xs -= 1;
	    flps_line(xs, y + mw, xs, y + h - mw - 1, FL_LEFT_BCOL);
	    xs -= 3;
	}
    }
    else
    {

	for (i = 0; i < 3; i++)
	{
	    flps_line(x + mw, ys, x + w - mw - 1, ys, FL_LEFT_BCOL);
	    ys -= 1;
	    flps_line(x + mw, ys, x + w - mw - 1, ys, FL_RIGHT_BCOL);
	    ys -= 3;
	}
    }
}

#define AddVertex(p,xp,yp) do{p->x = xp; p->y = yp; p++;} while(0)


static void
draw_bararrowhead(int x, int y, int w, int h, int angle, long col)
{
    float xc = x + 0.5f * w, yc = y + 0.5f * h;
    int d = 3 + (w + h) * 0.06f;
    float dx, dy, mar, dbar, xl;
    FL_POINT point[5], *p;

    x += d;
    y += d;
    w -= 2 * d;
    h -= 2 * d;
    dx = w / 2;
    dy = h / 2;

    dbar = dx * 0.4f;
    mar = 0.2f * dx;

    flps_output("gsave %.1f %.1f translate %d rotate\n", xc, yc, angle);

    xl = -dx + 1.1 * mar;
    p = point;
    AddVertex(p, xl, -dy);
    AddVertex(p, xl + dbar, -dy);
    AddVertex(p, xl + dbar, dy);
    AddVertex(p, xl, dy);
    flps_poly(1, point, 4, col);
    flps_poly(0, point, 4, FL_RIGHT_BCOL);

    p = point;
    AddVertex(p, -mar, -dy);
    AddVertex(p, -mar + dx, 0);
    AddVertex(p, -mar, dy);
    flps_poly(1, point, 3, col);
    flps_poly(0, point, 3, FL_RIGHT_BCOL);

    flps_output("grestore\n");
}

/* all PS drawings have a size of 2 pixels and centered at origin */
static PS_draw psdraw[] =
{
    {"returnarrow", "symreturnarrow",
     "-0.8 0.0 -0.1 0.7 -0.1 0.05 0.6 0.05 0.6 0.7 0.7 0.7 0.7 -0.05\n"
     "-0.1 -0.05 -0.1 -0.7 9 P"
    },

    {"->", "symrarrow",
     "-0.8 -0.4 -0.8 0.4 0 0.4 0 0.8 0.8 0 0 -0.8 0 -0.4 7 P"
    },
    {"<-", "180 rotate", 0, -1},

    {">", "symrarrowh", "-0.2 0.7 M 0.5 0.0 LT -0.2 -0.7 LT C"},
    {"<", "180 rotate", 0, -1},

    {"<->", "symdarrow",
     "-0.25 0.4 0.25 0.4 0.25 0.8 0.85 0 0.25 -0.8 0.25 -0.4 -0.25 -0.4\n"
     "-0.25 -0.8 -0.85 0 -0.25 0.8 10 P"
    },

    {"plus", "symplus",
   "-0.9 -0.13 -0.9 0.13 -0.13 0.13 -0.13 0.9 0.13 0.9 0.13 0.13 0.9 0.13\n"
     "0.9 -0.13 0.13 -0.13 0.13 -0.9 -0.13 -0.9 -0.13 -0.13 12 P"
    },

    {">>", "symdarrowh",
   "0.15 0.7 0.85 0 0.15 -0.7 0.15 -0.001 -0.55 -0.7 -0.55 0.7 0.15 0.001\n"
     "7 P"
    },
    {"<<", "180 rotate", 0, -1},

    {"arrow", "symthinarrow",
     "-0.9 0.02 M 0.65 0.02 LT 0.65 0.15 LT 0.9 0 LT 0.65 -0.15 LT\n"
     "0.65 -0.02 LT -0.9 -0.02 LT C"
    },

    {"circle", "symcircle", "0 0 0.77 0 360 arc"},

    {"square", "symsquare",
     "-0.77 -0.77 M -0.77 0.77 LT 0.77 0.77 LT 0.77 -0.77 LT C"
    },

    {"line", "symline",
     "-0.97 0.01 M  0.97 0.01 LT 0.97 -0.01 LT -0.08 -0.01 LT C",
    },

    {"->|", "symarrowbar",
     "-0.75 0.35 -0.1 0.35 -0.1 0.75 0.6 0.001 0.6 0.75 0.75 0.75\n"
" 0.75 -0.75 0.6 -0.75 0.6 -0.001 -0.1 -0.75 -0.1 -0.35 -0.75 -0.35 12 P\n",
    },

    {"|<-", "180 rotate", 0, -1},

    {">|", "symarrowheadbar",	/* bar 0.25 */
     "-0.60 0.7 0.22 0.001 0.22 0.7 0.47 0.7 0.47 -0.7 0.22 -0.7\n"
     " 0.22 -0.001 -0.60 -0.7 8 P",
    },

    {"|<", "180 rotate", 0, -1},

    {"|>", "symbararrowhead", 0, -1, draw_bararrowhead, 1},
    {"DnLine", "symdnline", 0, -1, draw_dnline},
    {"dnline", "symdnline", 0, -1, draw_dnline},
    {"UpLine", "symupline", 0, -1, draw_upline},
    {"upline", "symupline", 0, -1, draw_upline},
    {"UpArrow", "symuparrow", 0, -1, draw_uparrow, 1},
    {"DnArrow", "symdnarrow", 0, -1, draw_dnarrow, 1},
    {"=", "symRippleLines", 0, -1, draw_ripple_lines, 1},

 /* aliases */
    {"RippleLines", "symRippleLines", 0, -1, draw_ripple_lines, 1},

    {"+", "symplus",
   "-0.9 -0.13 -0.9 0.13 -0.13 0.13 -0.13 0.9 0.13 0.9 0.13 0.13 0.9 0.13\n"
     "0.9 -0.13 0.13 -0.13 0.13 -0.9 -0.13 -0.9 -0.13 -0.13 12 P"
    },

    {"-->", "symthinarrow",
     "-0.9 0.02 M 0.65 0.02 LT 0.65 0.15 LT 0.9 0 LT 0.65 -0.15 LT\n"
     "0.65 -0.02 LT -0.9 -0.02 LT C"
    }
};

static void
define_symbol(PS_draw * p)
{
    if (!p->defined && !p->fdrawit)
    {
	if (p->otherdef)
	    define_symbol(p + p->otherdef);
	else
	    flps_output("/%s {%s} BD\n", p->psname, p->draw);
	p->defined = 1;
    }
}

/* invalidate symbols cache */
void
flps_invalidate_symbol_cache(void)
{
    PS_draw *p = psdraw, *ps = psdraw + sizeof(psdraw) / sizeof(psdraw[0]);
    for (; p < ps; p++)
	p->defined = 0;
}

static PS_draw *
find(const char *s)
{
    PS_draw *p = psdraw, *ps = psdraw + sizeof(psdraw) / sizeof(psdraw[0]);

    for (; p < ps; p++)
	if (strcmp(s, p->name) == 0)
	{
	    define_symbol(p);
	    return p;
	}
    return 0;
}

#define swapit(t,a,b) do {t t_; t_ = a; a = b; b = t_;} while(0)

int
flps_draw_symbol(const char *label, int x, int y, int w, int h, long col)
{
    int pos, shift, equalscale = 0;
    short defr[] =
    {
	0, 225, 270, 315, 180, 0, 0, 135, 90, 45
    };
    PS_draw *s;
    int rotated = 0;
    int sw = w, sh = h;
    int delta = 0;

    if (!label || label[0] != '@')
	return 0;

    if (flps->verbose)
	fprintf(flps->fp, "%%Symbol %s: %d %d %d %d\n", label + 1, x, y, w, h);

    x += 1.2;
    y += 1.2;
    w -= 2.4;
    h -= 2.4;

    /* check special operators */

    pos = 1;
    while ((label[pos] == '-' && isdigit( ( int ) label[pos + 1])) ||
	   (label[pos] == '+' && isdigit( ( int ) label[pos + 1])) ||
	   label[pos] == '#')
    {
	switch (label[pos])
	{
	case '+':
	    delta = '0' - label[++pos];
	    break;
	case '-':
	    delta = label[++pos] - '0';
	    break;
	case '#':
	    equalscale = 1;
	    break;
	}
	pos++;
    }

    shift = pos;

    if (label[pos] >= '1' && label[pos] <= '9')
    {
	rotated = defr[label[pos] - '0'];
	shift = pos + 1;
    }
    else if (label[pos] == '0')
    {
	rotated = 100 * (label[pos + 1] - '0') + 10 *
	    (label[pos + 2] - '0') + 1 * (label[pos + 3] - '0');
	shift = pos + 4;
    }

    /* short hand with @4 etc */
    if (!(s = (label[shift] ? find(label + shift) : psdraw)))
    {
	fprintf(stderr, "Bad label %s\n", label + shift);
	if (flps->verbose)
	    fprintf(flps->fp, "%% unknown symbol %s. Not drawn\n", label);
	return 0;
    }

    if (equalscale)
	sw = sh = FL_min(w, h);

    if (delta)
    {
	if (s->abs_coordinate)
	{
	    x += delta;
	    y += delta;
	}
	sw -= 2 * delta;
	sh -= 2 * delta;
    }

    if (sw <= 5)
	sw = 5;
    if (sh <= 5)
	sw = 5;

    if (!s->abs_coordinate)
    {
	flps_output("gsave\n%.1f %.1f translate %.1f %.1f scale %d rotate ",
		 x + 0.5f * w, y + 0.5f * h, 0.5f * sw, 0.5f * sh, rotated);
	flps_output("%.3f %.3f LW\n", 3.0f / (sw + sh), 3.0f / (sh + sw));
    }

    if (s->fdrawit)
    {
	s->fdrawit(x, y, w, h, rotated, col);
	if (!s->abs_coordinate)
	    flps_output("grestore\n");
	flps_invalidate_color_cache();
	return 1;
    }

    if (s->otherdef)
    {
	flps_output("%s ", s->psname);
	s = s + s->otherdef;
    }

    flps_color(col);
    flps_output("%s F ", s->psname);
    flps_color(FL_BLACK);
    flps_output("%s S\n", s->psname);
    flps_output("grestore\n");

    flps_reset_cache();
    return 1;
}

/********************** end of symbols *************************}*/
void
flps_emit_prolog(void)
{
    /* basic graphics defines */
    flps_output("/BD {bind def} def\n");
    flps_output("/M {moveto} BD /LT {lineto} BD\n");
    flps_output("/RLT {rlineto} BD\n");
    flps_output("/L {M LT} BD  /C  {closepath} BD\n");
    flps_output("/LW {setlinewidth} BD\n");
    flps_output("/S {stroke} BD /F {fill} BD\n");
    flps_output("/G {setgray} BD /RGB {setrgbcolor} BD\n");
    /* x y x y N */
    flps_output("/P  {3 1 roll M 2 exch 1 exch {pop LT} for C} BD\n");
    flps_output("/lines {3 1 roll M 2 exch 1 exch {pop LT} for S} BD\n");
    flps_output("/DTD {[5 4 1 4] 0 setdash} BD\n");	/* dot-dash */
    flps_output("/DT {[1 3] 0 setdash} BD\n");	/* dot   */
    flps_output("/LD {[8 5] 0 setdash} BD\n");	/* long dash */
    flps_output("/D {[4 4] 0 setdash} BD\n");	/* dash */
    flps_output("/SL {[] 0 setdash} BD\n");	/* solid */
    flps_output("/NP {newpath} BD\n");	/* solid */

    flps_output("/SX %.2g def /SY %.2g def\n",
		flps->final_xscale, flps->final_yscale);
    /* basic font support */
    flps_output("/point {SX SY add 2 div div} BD\n");
    flps_output("/SetFont {findfont exch scalefont setfont} BD\n");
    flps_output("/Lshow {show} BD\n");
    flps_output("/Cshow {dup stringwidth pop -2 div 0 rmoveto show} BD\n");
    flps_output("/Rshow {dup stringwidth pop neg 0 rmoveto show} BD\n");
    flps_output("/CP {currentpoint} BD /SW {stringwidth} BD\n");
}

void
flps_emit_header(const char *title, int n, int xi, int yi, int xf, int yf)
{
    if (flps->eps)
	fprintf(flps->fp, "%%!PS-Adobe-3.0 EPSF-2.0\n");
    else
	fprintf(flps->fp, "%%!PS-Adobe-1.0\n");

    fprintf(flps->fp, "%%%%Title: %s\n", title);
    fprintf(flps->fp, "%%%%For: %s\n", fl_whoami());
    fprintf(flps->fp, "%%%%CreateDate: %s\n", fl_now());
    fprintf(flps->fp, "%%%%Creator: xforms V%d.%d "
	    "Copyright (c) 1997-1999 T.C. Zhao and M. Overmars\n",
	    FL_VERSION, FL_REVISION);
    fprintf(flps->fp, "%%%%Pages: %d\n", n);
    fprintf(flps->fp, "%%%%BoundingBox: %d %d %d %d\n", xi, yi, xf, yf);
    fprintf(flps->fp, "%%%%Orientation: %s\n",
	    flps->landscape ? "Landscape" : "Portrait");
    fprintf(flps->fp, "%%%%EndComments\n");

    fprintf(flps->fp, "%% PaperSize: %.1fx%.1fin\n",
	    flps->paper_w, flps->paper_h);
}

void
flps_set_clipping(int x, int y, int w, int h)
{
#if 0				/* rectclip is a level 2 feature */
    flps_output("gsave NP %d %d %d %d rectclip\n", x, y, w, h);
#else
    flps_output("gsave NP %d %d M %d 0 RLT 0 %d RLT %d neg 0 RLT CP clip NP\n",
		x, y, w, h, w);
#endif /* PACK */
}

void
flps_unset_clipping(void)
{
    flps_output("grestore\n");
    flps_reset_cache();
}
