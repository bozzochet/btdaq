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
 * $Id: xyplot.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1997-2002  T.C. Zhao
 *  All rights reserved.
 *.
 *
 *
 * Class FL_XYPLOT. Simple 2D tabulated function plot.
 *
 *   Possible optimization: break update into DRAW_INSET, DRAW_POINT
 *   etc. so an update of inset does not result in complete redraw.
 *
 *  Need to re-think about the entire approach to overlay
 *
 */
#if defined(F_ID) || defined(DEBUG)
char *fl_id_xyp = "$Id: xyplot.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include "forms.h"
#include <math.h>
#include "pxyplot.h"

#define SPEC FL_XYPLOT_SPEC

#define XMAJOR        5
#define XMINOR        2
#define YMAJOR        1
#define YMINOR        2

#define XRound(s)  (s->xtic>0 && !s->axtic[0] && s->xmajor > 1)
#define XMINRound  XRound
#define XMAXRound  XRound

#define YRound(s)  (s->ytic>0 && !s->aytic[0] && s->ymajor > 1)
#define YMINRound  YRound
#define YMAXRound  YRound

static float gen_tic(float, float, int, int);
static float gen_logtic(float, float, float, int, int);
static void convert_coord(FL_OBJECT *, SPEC *);
static void find_xbounds(SPEC *);
static void find_ybounds(SPEC *);
static int allocate_spec(SPEC *, int);
static void add_xgrid(FL_OBJECT *), add_ygrid(FL_OBJECT *);
static void free_spec_dynamic_mem(SPEC *);
static void w2s_draw(FL_OBJECT *, double, double, float *, float *);
static void compute_key_position(FL_OBJECT *);
static void draw_inset(FL_OBJECT *);
static void fl_xyplot_gen_xtic(FL_OBJECT * ob);
static void fl_xyplot_gen_ytic(FL_OBJECT * ob);

#define Free(s)   do {if(s) { fl_free(s); s = 0;}} while(0)

#ifdef BAD_SPRINTF
#define SPRINTF(a,b,c)      ((int)sprintf(a,b,c),strlen(a))
#define SPRINTF4(a,b,c,d)   ((int)sprintf(a,b,c,d),strlen(a))
#else
#define SPRINTF              sprintf
#define SPRINTF4             sprintf
#endif

/* free data associated with overlay i */
static void
free_overlay_data(SPEC *sp, int id)
{
    if (sp->n[id])
    {
	Free(sp->x[id]);
	Free(sp->y[id]);
	sp->n[id] = 0;
    }

}

static void
free_inset_text(SPEC *sp, int i)
{
    Free(sp->text[i]);
}

static void
free_atic(char *atic[])
{
    for (; *atic; atic++)
    {
	fl_free(*atic);
	*atic = 0;
    }
}

static void
extend_screen_data(SPEC *sp, int n)
{
    if (n > sp->cur_nxp)
    {
	sp->xp--;
	sp->xp = fl_realloc(sp->xp, (n + 3) * sizeof(FL_POINT));
	sp->xp++;		/* Need one extra point for fill */
	sp->cur_nxp = n;
	sp->xpactive = fl_realloc(sp->xpactive, (n + 3) * sizeof(FL_POINT));
    }
}

static void
free_xyplot(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;

    fl_clear_xyplot(ob);

    /* working arrays */
    fl_free(sp->wx);
    fl_free(sp->wy);
    fl_free(sp->xpactive);
    sp->xpi--;
    fl_free(sp->xpi);
    sp->xp--;
    fl_free(sp->xp);

    /* various labels */
    Free(sp->xlabel);
    Free(sp->ylabel);
    Free(sp->title);
    Free(sp->text);
    sp->text = 0;

    free_atic(sp->axtic);
    free_atic(sp->aytic);

    Free(sp->xmargin1);
    Free(sp->xmargin2);
    Free(sp->ymargin1);
    Free(sp->ymargin2);

    free_spec_dynamic_mem(sp);
}

/*  Symbols. Center at (x,y) spanning a rectangle of (w,h) */
static void
draw_square(FL_OBJECT * ob, int Id, FL_POINT * p, int n, int w, int h)
{
    int w2 = w / 2, h2 = h / 2;
    FL_POINT *ps = p + n;
    for (; p < ps; p++)
	XDrawRectangle(flx->display, flx->win, flx->gc, p->x - w2, p->y - h2, w, h);
}

static void
draw_circle(FL_OBJECT * ob, int id, FL_POINT * p, int n, int w, int h)
{
    int w2 = w / 2, h2 = h / 2;
    FL_POINT *ps = p + n;
    for (; p < ps; p++)
	XDrawArc(flx->display, flx->win, flx->gc, p->x - w2, p->y - h2,
		 w, h, 0, 64 * 360);
}

#define SetSeg(n,xi,yi,xf,yf)   seg[n].x1=xi;seg[n].y1=yi;\
                                seg[n].x2=xf;seg[n].y2=yf
static void
draw_points(FL_OBJECT * ob, int id, FL_POINT * p, int n, int w, int h)
{
    FL_POINT *ps = p + n;
    XSegment seg[4];
    int w2 = w / 2, h2 = h / 2;

    for (; p < ps; p++)
    {
	SetSeg(0, p->x - w2, p->y, p->x + w2, p->y);
	SetSeg(1, p->x, p->y - h2, p->x, p->y + h2);
	SetSeg(2, p->x - w2, p->y - h2, p->x + w2, p->y + h2);
	SetSeg(3, p->x + w2, p->y - h2, p->x - w2, p->y + h2);
	XDrawSegments(flx->display, flx->win, flx->gc, seg, 4);
    }
}

int
fl_xyplot_interpolate(FL_OBJECT * ob, int id, int n1, int n2)
{
    SPEC *sp = ob->spec;
    int newn;
    float *x = sp->x[id], *y = sp->y[id];

    /* need to resize screen points */
    newn = ((x[n2 - 1] - x[n1]) / sp->grid[id]) + 1.01f;

    /* far exceeds screen resolution. warn */
    if (newn > 5000)
	M_err("Interpol", "interpolating %d points. Far exceeds screen res", newn);

#if (FL_DEBUG >= ML_DEBUG)
    M_warn("XyplotInterpolate", "ID=%d: ndeg=%d grid=%f",
	   id, sp->interpolate[id], sp->grid[id]);
#endif

    if (newn > sp->nxpi)
    {
	sp->xpi--;
	sp->xpi = fl_realloc(sp->xpi, (newn + 3) * sizeof(FL_POINT));
	sp->xpi++;		/* Need one extra point for fill */
	sp->nxpi = newn;
    }

    if (newn > sp->ninterpol)
    {
	sp->wx = fl_realloc(sp->wx, newn * sizeof(float));
	sp->wy = fl_realloc(sp->wy, newn * sizeof(float));
	if (!sp->wx || !sp->wy)
	{
	    if (sp->wx)
		fl_realloc(sp->wx, sizeof(float));
	    M_err("Interpol", "Can't allocate memory for %d points", newn);
	    return -1;
	}
	sp->ninterpol = newn;
    }

    if (fl_interpolate(x + n1, y + n1, n2 - n1,
		       sp->wx, sp->wy, sp->grid[id],
		       sp->interpolate[id]) != newn)
    {
	M_err("Interpolate", "An error has occured while Inerpolating");
	return -1;
    }
    return newn;
}

/* to avoid singularity or extreme scaling factors */
#define FMIN  (1.e-25)

/* this is faster than calling fl_xyplot_w2s N times. Also
 * we call this while drawing, that means we have to
 * use sp->bx, but sp->bxm as fl_xyplot_w2s uses
 */
static void
mapw2s(SPEC *sp, FL_POINT * p, int n1, int n2, float *x, float *y)
{
    int i, tmp;
    float t, lbase;

    if (sp->xscale == FL_LOG)
    {
	lbase = 1.0f / sp->lxbase;
	for (i = n1; i < n2; i++)
	{
	    t = x[i] > 0.0f ? x[i] : FMIN;
	    p[i - n1].x = (sp->ax * (float) log10(t) * lbase + sp->bx + 0.4f);
	}
    }
    else
    {
	for (i = n1; i < n2; i++)
	    p[i - n1].x = (sp->ax * x[i] + sp->bx + 0.4f);
    }

    if (sp->yscale == FL_LOG)
    {
	lbase = 1.0f / sp->lybase;
	for (i = n1; i < n2; i++)
	{
	    t = y[i] > 0.0f ? y[i] : FMIN;
	    p[i - n1].y = (sp->ay * (float) log10(t) * lbase + sp->by + 0.4f);
	}
    }
    else
    {
	for (i = n1; i < n2; i++)
	{
	    /* this can happen if y is zoomed */
	    if ((tmp = (sp->ay * y[i] + sp->by + 0.4f)) < 0)
		tmp = 0;
	    else if (tmp > 30000)
		tmp = 30000;
	    p[i - n1].y = tmp;
	}
    }
}

/* if non-autoscaling, some of the data might fall outside the range
   desired, get rid of them so actual data that get plotted are bound
   by (n1, n2)
 */
void
fl_xyplot_compute_data_bounds(FL_OBJECT * ob, int *n1, int *n2, int id)
{
    register SPEC *sp = ob->spec;
    register int i;
    register float *x = sp->x[id];
    register float xmin = FL_min(sp->xmin, sp->xmax);
    register float xmax = FL_max(sp->xmax, sp->xmin);

    /* special case for two points */
    if (sp->n[id] < 3)
    {
	*n1 = 0;
	*n2 = sp->n[id];
	return;
    }

    for (*n1 = -1, i = 0; i < sp->n[id] && *n1 < 0; i++)
    {
	if (x[i] >= xmin)
	    *n1 = i;
    }

    if (*n1 > 0)
	(*n1)--;
    if (*n1 < 0)
	*n1 = 0;

    for (*n2 = -1, i = sp->n[id]; --i >= 0 && *n2 < 0;)
    {
	if (x[i] <= xmax)
	    *n2 = i;
    }

    if (*n2 < 0)
	*n2 = (sp->n[id] > 1 ? sp->n[id] : 1);
    if (*n2 < sp->n[id])
	(*n2)++;
}

/* draw curve itself only */
static void
draw_curve_only(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    int i, n1, n2, noline, nplot, type, nxp, newn, cur_lw = 0;
    FL_XYPLOT_SYMBOL drawsymbol;
    FL_POINT *xp;
    float *x, *y;
    FL_COLOR col;
    int savelw = fl_get_linewidth(), savels = fl_get_linestyle();
    int key_xs, key_ys;

    fl_set_clipping(sp->xi, sp->yi, sp->xf - sp->xi + 1, sp->yf - sp->yi + 1);
    fl_set_text_clipping(sp->xi, sp->yi, sp->xf - sp->xi + 1, sp->yf - sp->yi + 1);

    if (sp->xgrid != FL_GRID_NONE && sp->xtic > 0)
	add_xgrid(ob);

    if (sp->ygrid != FL_GRID_NONE && sp->ytic > 0)
	add_ygrid(ob);

    compute_key_position(ob);
    fl_rect(sp->key_xs, sp->key_ys, sp->key_maxw, sp->key_maxh, sp->col[0]);

    key_xs = sp->key_xs + 2;
    key_ys = sp->key_ys + sp->key_ascend - sp->key_descend;

    for (nplot = 0; nplot <= sp->maxoverlay; nplot++)
    {
	if (sp->n[nplot] == 0)
	    continue;

	fl_color(col = sp->col[nplot]);
	drawsymbol = 0;
	noline = 0;

	/* if non-autoscaling, some of the data might fall outside the range
	   desired, get rid of them so actual data that get plotted are bound
	   by (n1, n2) */

	fl_xyplot_compute_data_bounds(ob, &n1, &n2, nplot);

	/* convert data */

	/* if interpolate, do it here */
	if (sp->interpolate[nplot] > 1 && (n2 - n1) > 3 &&
	    (newn = fl_xyplot_interpolate(ob, nplot, n1, n2)) >= 0)
	{
	    x = sp->wx;
	    y = sp->wy;
	    xp = sp->xpi;

	    mapw2s(sp, xp, 0, newn, x, y);

	    nxp = sp->nxpi = newn;

	    mapw2s(sp, sp->xp, n1, n2, sp->x[nplot], sp->y[nplot]);
	    sp->nxp = n2 - n1;
	    if ((sp->active || sp->inspect) && sp->iactive == nplot && !sp->update)
	    {
		memcpy(sp->xpactive, sp->xp, sp->nxp * sizeof(*xp));
		sp->n1 = n1;
	    }
	}
	else
	{
	    x = sp->x[nplot];
	    y = sp->y[nplot];
	    xp = sp->xp;
	    mapw2s(sp, xp, n1, n2, x, y);
	    nxp = sp->nxp = n2 - n1;
	    if ((sp->active || sp->inspect) && sp->iactive == nplot && !sp->update)
	    {
		memcpy(sp->xpactive, sp->xp, sp->nxp * sizeof(*xp));
		sp->n1 = n1;
	    }
	}

	type = (nplot ? sp->type[nplot] : ob->type);
	if (cur_lw != sp->thickness[nplot])
	{
	    cur_lw = sp->thickness[nplot];
	    fl_linewidth(cur_lw);
	}

	switch (type)
	{
	case FL_ACTIVE_XYPLOT:
	    drawsymbol = sp->mark_active ? draw_square : 0;
	    break;
	case FL_SQUARE_XYPLOT:
	    drawsymbol = draw_square;
	    break;
	case FL_CIRCLE_XYPLOT:
	    drawsymbol = draw_circle;
	    break;
	case FL_POINTS_XYPLOT:
	    noline = 1;
	case FL_LINEPOINTS_XYPLOT:
	    drawsymbol = sp->symbol[nplot] ? sp->symbol[nplot] : draw_points;
	    break;
	case FL_DOTTED_XYPLOT:
	    fl_linestyle(FL_DOT);
	    break;
	case FL_DOTDASHED_XYPLOT:
	    fl_linestyle(FL_DOTDASH);
	    break;
	case FL_LONGDASHED_XYPLOT:
	    fl_linestyle(FL_LONGDASH);
	    break;
	case FL_DASHED_XYPLOT:
	    fl_dashedlinestyle(0, 0);
	    fl_linestyle(LineOnOffDash);
	    break;
	case FL_FILL_XYPLOT:
	    xp[-1].x = xp[0].x;
	    xp[-1].y = sp->yf;
	    xp[nxp].x = xp[nxp - 1].x;
	    xp[nxp].y = sp->yf;
	    fl_polyf((xp - 1), nxp + 2, col);
	    noline = 1;
	    break;
	case FL_EMPTY_XYPLOT:
	    noline = 1;
	    drawsymbol = 0;
	    break;
	case FL_IMPULSE_XYPLOT:
	    noline = 1;
	    drawsymbol = 0;
	    for (i = 0; i < nxp; i++)
		fl_line(xp[i].x, sp->yf - 1, xp[i].x, xp[i].y, col);
	    break;
	case FL_NORMAL_XYPLOT:
	default:
	    break;
	}

	if (!noline)
	{
	    fl_lines(xp, nxp, col);
	}

	if (drawsymbol)
	{
	    xp = sp->xp;	/* always use original point for symbol */
	    nxp = sp->nxp;
	    drawsymbol(ob, nplot, xp, nxp, sp->ssize, sp->ssize);
	}

	/* do keys */

	if (sp->key[nplot])
	{
	    fl_linewidth(0);

	    if (!noline)
		fl_line(key_xs, key_ys, key_xs + 20, key_ys, col);

	    if (type == FL_IMPULSE_XYPLOT)
	    {
		fl_line(key_xs + 3, key_ys + 2, key_xs + 3, key_ys - 3, col);
		fl_line(key_xs + 7, key_ys + 2, key_xs + 7, key_ys - 3, col);
		fl_line(key_xs + 11, key_ys + 2, key_xs + 11, key_ys - 3, col);
		fl_line(key_xs + 15, key_ys + 2, key_xs + 15, key_ys - 3, col);
	    }
	    else if (sp->type[nplot] == FL_FILL_XYPLOT)
	    {
		fl_rectf(key_xs + 1, key_ys - 3, 19, 6, col);
	    }

	    if (drawsymbol)
	    {
		FL_POINT p[4];
		p[0].x = key_xs + 3;
		p[1].x = key_xs + 10;
		p[2].x = key_xs + 17;
		p[0].y = p[1].y = p[2].y = key_ys;
		drawsymbol(ob, nplot, p, 3, 4, 4);
	    }

	    fl_drw_text(FL_ALIGN_LEFT, key_xs + 20, key_ys, 0, 0, col,
			sp->key_lstyle, sp->key_lsize, sp->key[nplot]);

	    key_ys += sp->key_ascend + sp->key_descend * 0.9;
	}

	fl_linestyle(savels);
	fl_linewidth(cur_lw = savelw);
    }

    /* finally we do extra text */
    draw_inset(ob);

    fl_linestyle(savels);
    fl_linewidth(savelw);
    fl_unset_clipping();
    fl_unset_text_clipping();
}

void
fl_xyplot_nice_label(float tic, int minor, float f, char label[])
{
    float crit = tic * minor;

    label[0] = '\0';

    if (tic >= 1.0f && crit < 10.0f)
	sprintf(label, "%.1f", f);
    else if (tic > 1.0f && crit < 1000.0f)
	sprintf(label, "%.0f", f);
    else if (crit >= 0.40f && crit <= 999.0f)
	sprintf(label, "%.1f", f);
    else if (crit < 0.40f && crit > 0.01f)
	sprintf(label, "%.2f", f);
    else
	sprintf(label, "%g", f);
}

static void
fl_xyplot_gen_xtic(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    float tic = sp->xtic;
    float xmin, xmax = sp->xmax, xw;
    float mxmin, mxmax, val;
    int i, j;
    char *p;

    if (tic < 0)
	return;

    xmin = mxmin = FL_min(sp->xscmin, sp->xscmax);
    xmax = mxmax = FL_max(sp->xscmin, sp->xscmax);

    if (XRound(sp))
    {
	mxmin = floor(xmin / tic) * tic;
	mxmax = ceil(xmax / tic) * tic;
    }

    /* handle ticlable@location stuff */
    if (sp->axtic[0] && strchr(sp->axtic[0], '@'))
    {
	for (i = j = 0; i < sp->xmajor; i++)
	{
	    if ((p = strchr(sp->axtic[i], '@')))
		val = atof(p + 1);
	    else
		val = mxmin + (i * tic * sp->xminor);

	    if (val >= xmin && val <= xmax)
	    {
		sp->xtic_major[i] = sp->ax * val + sp->bx + 0.4f;
		sp->xmajor_val[i] = val;
		j++;
	    }
	    sp->num_xmajor = j;
	    sp->num_xminor = 1;
	}
	return;
    }

    if (sp->xscale != FL_LOG)
    {
	/* other minor tics */
	for (i = 0, xw = mxmin; xw <= mxmax; xw += tic)
	{
	    if (xw >= xmin && xw <= xmax)
		sp->xtic_minor[i++] = (sp->ax * xw + sp->bx + 0.4f);
	}
	sp->num_xminor = i;

	for (i = 0, xw = mxmin; xw <= mxmax; xw += tic * sp->xminor)
	{
	    if (xw >= xmin && xw <= xmax)
	    {
		sp->xtic_major[i] = (sp->ax * xw + sp->bx + 0.4f);
		sp->xmajor_val[i] = xw;
		i++;
	    }
	}
	sp->num_xmajor = i;
    }
    else
    {
	float minortic = tic / sp->xminor;

	/* minor */
	for (i = 0, xw = xmin; xw <= xmax; xw += minortic)
	    sp->xtic_minor[i++] = (sp->ax * xw + sp->bx + 0.4f);

	sp->num_xminor = i;

	for (i = 0, xw = xmin; xw <= xmax; xw += tic)
	{
	    sp->xtic_major[i] = (sp->ax * xw + sp->bx + 0.4f);
	    sp->xmajor_val[i] = xw;
	    i++;
	}

	sp->num_xmajor = i;
    }
}

static void
fl_xyplot_gen_ytic(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    float ymin, ymax, mymin, mymax, tic = sp->ytic, val;
    double yw;
    int i, n, j;
    char *p;

    if (tic < 0)
	return;

    mymin = ymin = FL_min(sp->yscmin, sp->yscmax);
    mymax = ymax = FL_max(sp->yscmin, sp->yscmax);

    if (YRound(sp))
    {
	mymin = floor(ymin / tic) * tic;
	mymax = ceil(ymax / tic) * tic;
    }

    /* handle ticlable@location stuff */
    if (sp->aytic[0] && strchr(sp->aytic[0], '@'))
    {
	for (i = j = 0; i < sp->ymajor; i++)
	{
	    if ((p = strchr(sp->aytic[i], '@')))
		val = atof(p + 1);
	    else
		val = mymin + i * tic * sp->yminor;

	    if (val >= ymin && val <= ymax)
	    {
		sp->ytic_major[i] = sp->ay * val + sp->by + 0.4f;
		sp->ymajor_val[i] = val;
		j++;
	    }
	    sp->num_ymajor = j;
	    sp->num_yminor = 1;
	}
	return;
    }

    if (sp->yscale != FL_LOG)
    {
	for (i = 0, yw = mymin; yw <= mymax; yw += tic)
	{
	    if (yw >= ymin && yw <= ymax)
	    {
		sp->ytic_minor[i++] = sp->ay * yw + sp->by + 0.4f;
	    }
	}

	sp->num_yminor = i;

	for (n = i = 0, yw = mymin; yw <= mymax; n++)
	{
	    if (yw >= ymin && yw <= ymax)
	    {
		sp->ytic_major[i] = (sp->ay * yw + sp->by + 0.4f);
		sp->ymajor_val[i] = yw;
		i++;
	    }
	    yw = mymin + ((n + 1) * tic * sp->yminor);
	}
	sp->num_ymajor = i;
    }
    else
    {
	double minortic = sp->ytic / sp->yminor;
	for (i = 0, yw = ymin; yw <= ymax; yw += minortic)
	    sp->ytic_minor[i++] = (sp->ay * yw + sp->by + 0.4f);
	sp->num_yminor = i;

	for (i = 0, yw = ymin; yw <= ymax; yw += sp->ytic)
	{
	    sp->ytic_major[i] = (sp->ay * yw + sp->by + 0.4f);
	    sp->ymajor_val[i] = yw;
	    i++;
	}
	sp->num_ymajor = i;
    }
}

static void
add_xgrid(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    int yi, yf, xr, i;
    int ls = fl_get_linestyle();

    yi = sp->yi + 1;
    yf = sp->yf - 1;

    fl_linestyle(sp->grid_linestyle);

    for (i = 0; sp->xgrid == FL_GRID_MINOR && i < sp->num_xminor; i++)
    {
	xr = sp->xtic_minor[i];
	fl_line(xr, yi, xr, yf, ob->col2);
    }

    for (i = 0; i < sp->num_xmajor; i++)
    {
	xr = sp->xtic_major[i];
	fl_line(xr, yi, xr, yf, ob->col2);
    }

    fl_linestyle(ls);
}

static void
add_ygrid(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    int yr, i, xi, xf;
    int ls = fl_get_linestyle();

    xi = sp->xi + 1;
    xf = sp->xf - 1;

    fl_linestyle(sp->grid_linestyle);

    for (i = 0; sp->ygrid == FL_GRID_MINOR && i < sp->num_yminor; i++)
    {
	yr = sp->ytic_minor[i];
	fl_line(xi, yr, xf, yr, ob->col2);
    }

    for (i = 0; i < sp->num_ymajor; i++)
    {
	yr = sp->ytic_major[i];
	fl_line(xi, yr, xf, yr, ob->col2);
    }

    fl_linestyle(ls);

}

static void
add_xtics(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    float tic = sp->xtic;
    int xr, ticl = 6, yi, yf, i;
    char buf[80], *label;

    if (tic <= 0)
	return;

    yi = sp->yf + 1;
    yf = yi + ticl / 2;

    /* minor tics */
    for (i = 0; i < sp->num_xminor; i++)
    {
	xr = sp->xtic_minor[i];
	fl_line(xr, yi, xr, yf, ob->col2);
    }

    yi = sp->yf + 1;
    yf = yi + ticl;

    for (i = 0; i < sp->num_xmajor; i++)
    {
	xr = sp->xtic_major[i];
	fl_line(xr, yi, xr, yf, ob->col2);

	if (!sp->axtic[0])
	    fl_xyplot_nice_label(tic, sp->xminor,
				 sp->xmajor_val[i], label = buf);
	else
	{
	    char *p;
	    if ((p = strchr(sp->axtic[i], '@')))
	    {
		label = strcpy(buf, sp->axtic[i]);
		label[p - sp->axtic[i]] = '\0';
	    }
	    else
		label = sp->axtic[i];
	}

	fl_drw_text(FL_ALIGN_TOP, xr, sp->yf + ticl - 2, 0, 0,
		    ob->col2, sp->lstyle, sp->lsize, label);
    }
}

static void
add_logxtics(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    float tic = sp->xtic, xw;
    int xr, ticl = 6, i, yi, yf;
    char label[80];

    if (tic < 0)
	return;

    yi = sp->yf + 1;
    yf = yi + ticl / 2;

    for (i = 0; i < sp->num_xminor; i++)
    {
	xr = sp->xtic_minor[i];
	fl_line(xr, yi, xr, yf, ob->col2);
    }

    yi = sp->yf;
    yf = yi + ticl;

    for (i = 0; i < sp->num_xmajor; i++)
    {
	xr = sp->xtic_major[i];
	fl_line(xr, yi, xr, yf, ob->col2);

	xw = sp->xmajor_val[i];

	if (sp->xbase == 10.0)
	{
	    sprintf(label, "%g", pow(sp->xbase, xw));
	    fl_drw_text(FL_ALIGN_TOP, xr, sp->yf + ticl - 2, 0, 0,
			ob->col2, sp->lstyle, sp->lsize, label);
	}
	else
	{
	    int len1, len2, ll;

	    ll = SPRINTF(label, "%g", sp->xbase);

	    fl_drw_text(FL_ALIGN_TOP, xr - 3, yf - 2, 0, 0,
			ob->col2, sp->lstyle, sp->lsize, label);
	    len1 = fl_get_string_width(sp->lstyle, sp->lsize, label, ll);
	    ll = SPRINTF(label, "%d", (int) ceil(xw));
	    len2 = fl_get_string_width(sp->lstyle, sp->lsize, label, ll);
	    fl_drw_text(FL_ALIGN_TOP, xr - 3 + len1 / 2 + 1 + len2 / 2,
		      yf - 6, 0, 0, ob->col2, sp->lstyle, sp->lsize, label);
	}
    }
}

static void
add_logytics(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    float yw;
    int yr, ticl = 6, i;
    char label[80];

    if (sp->ytic <= 0)
	return;

    for (i = 0; i < sp->num_yminor; i++)
    {
	yr = sp->ytic_minor[i];
	fl_line(sp->xi, yr, sp->xi - ticl / 2, yr, ob->col2);
    }

    for (i = 0; i < sp->num_ymajor; i++)
    {
	yr = sp->ytic_major[i];
	fl_line(sp->xi - ticl, yr, sp->xi, yr, ob->col2);

	yw = sp->ymajor_val[i];

	if (sp->ybase == 10.0f)
	{
	    sprintf(label, "%g", pow(sp->ybase, yw));
	    fl_drw_text(FL_ALIGN_RIGHT, sp->xi - ticl + 2, yr,
			0, 0, ob->col2, sp->lstyle, sp->lsize, label);
	}
	else
	{
	    int len, ll;

	    ll = SPRINTF(label, "%d", (int) ceil(yw));
	    fl_drw_text(FL_ALIGN_RIGHT, sp->xi - ticl + 2, yr - 3,
			0, 0, ob->col2, sp->lstyle, sp->lsize, label);
	    len = fl_get_string_width(sp->lstyle, sp->lsize, label, ll);
	    sprintf(label, "%g", sp->ybase);
	    fl_drw_text(FL_ALIGN_RIGHT, sp->xi - ticl + 2 - len - 1,
		      yr + 1, 0, 0, ob->col2, sp->lstyle, sp->lsize, label);
	}
    }
}

static void
add_ytics(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    float tic = sp->ytic;
    int yr, ticl = 6, i;
    char buf[80], *label, *p;

    if (sp->ytic <= 0)
	return;

    for (i = 0; i < sp->num_yminor; i++)
    {
	yr = sp->ytic_minor[i];
	fl_line(sp->xi - 4, yr, sp->xi, yr, ob->col2);
    }

    for (i = 0; i < sp->num_ymajor; i++)
    {
	yr = sp->ytic_major[i];
	fl_line(sp->xi - ticl, yr, sp->xi, yr, ob->col2);

	if (!sp->aytic[0])
	    fl_xyplot_nice_label(tic, sp->yminor,
				 sp->ymajor_val[i], label = buf);
	else
	{
	    if ((p = strchr(sp->aytic[i], '@')))
	    {
		label = strcpy(buf, sp->aytic[i]);
		label[p - sp->aytic[i]] = '\0';
	    }
	    else
		label = sp->aytic[i];
	}

	fl_drw_text(FL_ALIGN_RIGHT, sp->xi - ticl + 2, yr,
		    0, 0, ob->col2, sp->lstyle, sp->lsize, label);
    }
}

static void
convert_coord(FL_OBJECT * ob, SPEC *sp)
{
    float extrax1, extray1;
    float extrax2, extray2;
    char buf[80], *label, *p;
    int j, ticl = 6, w = 0, tmpw = 0, ll;
    int fh = fl_get_string_height(sp->lstyle, sp->lsize, "1pj", 3, &j, &j);
    float halfh = 0.55f * fh;

    /* min. margins fixed margins */
    extrax1 = extray1 = extrax2 = extray2 = FL_abs(ob->bw) + 3.0f;

    /* figure out the plot region */
    if (sp->xtic > 0)
    {
	extray2 += ticl + fh + (sp->xscale == FL_LOG);

	if (!sp->axtic[0])
	    fl_xyplot_nice_label(sp->xtic, sp->xminor, sp->xmax, label = buf);
	else
	    label = sp->axtic[sp->xmajor - 1];

	w = fl_get_string_width(sp->lstyle, sp->lsize, label,
				strlen(label)) / 2;
	extrax2 += w + (sp->xscale == FL_LOG) * 2;

	/* don't need to compute label size if ytic is on */
	if (sp->ytic < 0)
	{
	    if (!sp->axtic[0])
		fl_xyplot_nice_label(sp->xtic, sp->xminor, sp->xmin,
				     label = buf);
	    else
		label = sp->axtic[0];

	    w = fl_get_string_width(sp->lstyle, sp->lsize, label,
				    strlen(label)) / 2;
	    extrax1 += w;
	}
    }

    sp->maxytic = 2;
    if (sp->ytic > 0)
    {
	label = buf;
	if (!sp->aytic[0])
	{
	    if (sp->yscale == FL_LOG)
	    {
		char *fmt = sp->ybase == 10 ? "%g-e%d" : "%g%d  ";

		ll = SPRINTF4(label, fmt, sp->ybase, (int) sp->yscmax);
		w = fl_get_string_width(sp->lstyle, sp->lsize, label, ll);
		ll = SPRINTF4(label, fmt, sp->ybase, (int) sp->yscmin);
		tmpw = fl_get_string_width(sp->lstyle, sp->lsize, label, ll);
	    }
	    else
	    {
		fl_xyplot_nice_label(sp->ytic, sp->yminor, sp->yscmax, label);
		w = fl_get_string_width(sp->lstyle, sp->lsize, label,
					strlen(label));
		fl_xyplot_nice_label(sp->ytic, sp->yminor, sp->yscmin, label);
		tmpw = fl_get_string_width(sp->lstyle, sp->lsize, label,
					   strlen(label));
	    }
	    w = (w > tmpw ? w : tmpw);
	}
	else
	{
	    if ((p = strchr(sp->aytic[0], '@')))
	    {
		label = strcpy(buf, sp->aytic[0]);
		label[p - sp->aytic[0]] = '\0';
	    }
	    else
		label = sp->aytic[0];

	    w = fl_get_string_width(sp->lstyle, sp->lsize, label,
				    strlen(label));
	}

	/* how much space to leave for ytics ylabels */
	extrax1 += ticl + 1;
	extrax1 += w;
	sp->maxytic = extrax1;
	extray1 += halfh + 1;
	if (sp->xtic < 0)
	    extray2 += halfh + 1;
    }


    if (sp->ylabel[0])
	extrax1 += 6 + fl_get_char_width(sp->lstyle, sp->lsize);

    if (sp->xlabel[0])
	extray2 += 2 + 1.1 * fh;

    if (sp->title[0])
	extray1 += 1 + (sp->ytic > 0 ? halfh : fh);

    /* if margin set, use the margin instead of the computed margin */
    if (sp->xmargin1)
    {
	extrax1 = extrax2 = FL_abs(ob->bw) + 1 + fl_get_linewidth();
	extrax1 += fl_get_string_width(sp->lstyle, sp->lsize, sp->xmargin1,
				       strlen(sp->xmargin1));
	extrax2 += fl_get_string_width(sp->lstyle, sp->lsize, sp->xmargin2,
				       strlen(sp->xmargin2));
    }

    if (sp->ymargin1)
    {
	extray1 = extray2 = FL_abs(ob->bw) + 1;
	extray1 += fl_get_string_width(sp->lstyle, sp->lsize, sp->ymargin1,
				       strlen(sp->ymargin1));
	extray2 += fl_get_string_width(sp->lstyle, sp->lsize, sp->ymargin2,
				       strlen(sp->ymargin2));
    }

    sp->xi = ob->x + extrax1;
    sp->yi = ob->y + extray1;
    sp->xf = (ob->x + ob->w - extrax2);
    sp->yf = (ob->y + ob->h - extray2);

    sp->ax = (sp->xf - sp->xi) / (sp->xscmax - sp->xscmin);
    sp->bx = sp->xi - sp->ax * sp->xscmin;
    sp->bxm = sp->bx - ob->x + sp->objx;

    sp->ay = (sp->yf - sp->yi) / (sp->yscmin - sp->yscmax);
    sp->by = sp->yi - sp->ay * sp->yscmax;
    sp->bym = sp->by - ob->y + sp->objy;

    fl_xyplot_gen_xtic(ob);
    fl_xyplot_gen_ytic(ob);
}

/*
 * Draw a string with alignment given relative to a point.
 * Figure out the bounding box etc so symbols can be drawn
 */
static void
fl_drw_text_point(int lalign, int x, int y, FL_COLOR col,
		  int lstyle, int lsize, char *str)
{
    int align = lalign & ~FL_ALIGN_INSIDE;
    int bbox = 1.4f * lsize + 6;
    int xx = x, yy = y;

    switch (align)
    {
    case FL_ALIGN_CENTER:
	xx = x - bbox / 2;
	yy = y - bbox / 2;
	break;
    case FL_ALIGN_TOP:
	xx = x - bbox / 2;
	break;
    case FL_ALIGN_BOTTOM:
	xx = x - bbox / 2;
	yy = y - bbox;
	break;
    case FL_ALIGN_LEFT:
	yy = y - bbox / 2;
	break;
    case FL_ALIGN_RIGHT:
	xx = x - bbox;
	yy = y - bbox / 2;
	break;
    case FL_ALIGN_TOP_LEFT:
	xx = x - bbox;
	align = FL_ALIGN_TOP_RIGHT;
	break;
    case FL_ALIGN_TOP_RIGHT:
	align = FL_ALIGN_TOP_LEFT;
	break;
    case FL_ALIGN_BOTTOM_RIGHT:
	yy = y - bbox;
	align = FL_ALIGN_BOTTOM_LEFT;
	break;
    case FL_ALIGN_BOTTOM_LEFT:
	align = FL_ALIGN_BOTTOM_RIGHT;
	yy = y - bbox;
	xx = x - bbox;
	break;
    }

    fl_drw_text_beside(align, xx, yy, bbox, bbox, col, lstyle, lsize, str);
}

static void
draw_inset(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    int i;
    float tx, ty;

    for (i = 0; i < sp->maxoverlay; i++)
    {
	if (sp->text[i])
	{
	    w2s_draw(ob, sp->xt[i], sp->yt[i], &tx, &ty);
	    fl_drw_text_point(sp->talign[i], tx, ty, sp->tcol[i],
			      sp->lstyle, sp->lsize, sp->text[i]);
	}
    }
}


static void
compute_key_position(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    int w = 0, i, h, align;
    float fx, fy;

    h = fl_get_char_height(sp->key_lstyle, sp->key_lsize,
			   &sp->key_ascend, &sp->key_descend);

    /* find the max width */
    sp->key_maxw = sp->key_maxh = 0;
    for (i = 0; i < sp->maxoverlay && sp->n[i]; i++)
    {
	if (sp->key[i])
	{
	    w = fl_get_string_width(sp->key_lstyle, sp->key_lsize,
				    sp->key[i], strlen(sp->key[i]));
	    if (w > sp->key_maxw)
		sp->key_maxw = w;
	    sp->key_maxh += h;
	}
    }

    if (sp->key_maxw == 0)
	return;

    /* get alignment */
    w2s_draw(ob, sp->key_x, sp->key_y, &fx, &fy);
    sp->key_xs = fx;
    sp->key_ys = fy;

    sp->key_maxw += 32;
    sp->key_maxh += 1;

    align = sp->key_align;

    if (align == FL_ALIGN_TOP_LEFT)
	align = FL_ALIGN_TOP_RIGHT;
    else if (align == FL_ALIGN_TOP_RIGHT)
	align = FL_ALIGN_TOP_LEFT;
    else if (align == FL_ALIGN_BOTTOM_RIGHT)
	align = FL_ALIGN_BOTTOM_LEFT;
    else if (align == FL_ALIGN_BOTTOM_LEFT)
	align = FL_ALIGN_BOTTOM_RIGHT;

    fl_get_align_xy(align, sp->key_xs, sp->key_ys, 0, 0,
		sp->key_maxw, sp->key_maxh, 0, 0, &sp->key_xs, &sp->key_ys);

}

static void
add_border(SPEC *sp, FL_COLOR c)
{
    if (sp->xtic > 0 && sp->ytic > 0)
	fl_rect(sp->xi, sp->yi, sp->xf - sp->xi, sp->yf - sp->yi, c);
    else if (sp->xtic > 0)
	fl_line(sp->xi, sp->yf, sp->xf, sp->yf, c);
    else if (sp->ytic > 0)
	fl_line(sp->xi, sp->yi, sp->xi, sp->yf, c);
}

/*
 * (scxmin, scxmax) ultimately determines the plot area in world coord.
 */
static void
round_xminmax(SPEC *sp)
{
    if (sp->xscale != FL_LOG)
    {
	float xmin = sp->xmin, xmax = sp->xmax, tic = sp->xtic;
	float threshold = 0.5f * tic;

	if (XMINRound(sp))
	{
	    xmin = (xmin < xmax ? floor : ceil) (xmin / tic) * tic;
	    if (FL_abs(xmin - sp->xmin) > threshold)
		xmin = sp->xmin;
	}

	if (XMAXRound(sp))
	{
	    xmax = (xmin < xmax ? ceil : floor) (xmax / tic) * tic;
	    if (FL_abs(xmax - sp->xmax) > threshold)
		xmax = sp->xmax;
	}

	sp->xscmin = xmin;
	sp->xscmax = xmax;
    }
    else
    {
	float lmax = log10(sp->xmax) / sp->lxbase;
	float lmin = log10(sp->xmin) / sp->lxbase;

	if (XMINRound(sp))
	    lmin = (sp->xmin < sp->xmax ? floor : ceil)
		(log10(sp->xmin) / (sp->lxbase * sp->xtic)) * sp->xtic;
	if (XMAXRound(sp))
	    lmax = (sp->xmin < sp->xmax ? ceil : floor)
		(log10(sp->xmax) / (sp->lxbase * sp->xtic)) * sp->xtic;

	sp->xscmin = lmin;
	sp->xscmax = lmax;
    }
}

/*
 * do the same with y bounds
 */

static void
round_yminmax(SPEC *sp)
{

    if (sp->yscale != FL_LOG)
    {
	float ymin = sp->ymin, ymax = sp->ymax, tic = sp->ytic;
	float threshold = 0.7 * tic;

	if (YMINRound(sp))
	{
	    ymin = (ymin < ymax ? floor : ceil) (ymin / tic) * tic;
	    if (FL_abs(ymin - sp->ymin) > threshold)
		ymin = sp->ymin;
	}

	if (YMAXRound(sp))
	{
	    ymax = (ymin < ymax ? ceil : floor) (ymax / tic) * tic;
	    if (FL_abs(ymax - sp->ymax) > threshold)
		ymax = sp->ymax;
	}

	sp->yscmin = ymin;
	sp->yscmax = ymax;
    }
    else
    {
	float lmax = log10(sp->ymax) / sp->lybase;
	float lmin = log10(sp->ymin) / sp->lybase;

	if (YMINRound(sp))
	    lmin = (sp->ymin < sp->ymax ? floor : ceil)
		(log10(sp->ymin) / (sp->lybase * sp->ytic)) * sp->ytic;
	if (YMAXRound(sp))
	    lmax = (sp->xmin < sp->xmax ? ceil : floor)
		(log10(sp->ymax) / (sp->lybase * sp->ytic)) * sp->ytic;
	sp->yscmin = lmin;
	sp->yscmax = lmax;
    }
}


static void
draw_xyplot(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    FL_Coord bw = FL_abs(ob->bw);

    fl_drw_box(ob->boxtype, ob->x, ob->y, ob->w, ob->h, ob->col1, ob->bw);

    fl_drw_text_beside(ob->align, ob->x, ob->y, ob->w, ob->h,
		       ob->lcol, ob->lstyle, ob->lsize, ob->label);

    if (sp->n[0] <= 0 || !sp->x[0] || !sp->y[0])
	return;

    sp->xtic = sp->ytic = -1;
    sp->xscmin = sp->xmin;
    sp->xscmax = sp->xmax;
    sp->yscmin = sp->ymin;
    sp->yscmax = sp->ymax;

    if (sp->xmajor > 0)
    {
	if (sp->xscale == FL_LOG)
	    sp->xtic = gen_logtic(sp->xmin, sp->xmax, sp->xbase, sp->xmajor, sp->xminor);
	else
	    sp->xtic = gen_tic(sp->xmin, sp->xmax, sp->xmajor, sp->xminor);

	round_xminmax(sp);

    }

    if (sp->ymajor > 0)
    {
	if (sp->yscale == FL_LOG)
	    sp->ytic = gen_logtic(sp->ymin, sp->ymax, sp->ybase, sp->ymajor, sp->yminor);
	else
	    sp->ytic = gen_tic(sp->ymin, sp->ymax, sp->ymajor, sp->yminor);

	round_yminmax(sp);
    }


#if (FL_DEBUG >= ML_DEBUG)
    M_warn("drawXyplot", "xtic=%f(major=%d) ytic=%f (major=%d)",
	   sp->xtic, sp->xmajor, sp->ytic, sp->ymajor);
#endif

    convert_coord(ob, sp);
    add_border(sp, ob->col2);
    draw_curve_only(ob);

    fl_set_text_clipping(ob->x + bw, ob->y + bw, ob->w - 2 * bw, ob->h - 2 * bw);
    fl_set_clipping(ob->x + bw, ob->y + bw, ob->w - 2 * bw, ob->h - 2 * bw);

    /* do the tics and other things */

    /* title */
    fl_drw_text(FL_ALIGN_BOTTOM, (sp->xi + sp->xf) / 2,
		sp->yi + 1, 0, 0,
		ob->col2, sp->lstyle, sp->lsize, sp->title);

    (sp->xscale == FL_LOG ? add_logxtics : add_xtics) (ob);
    fl_drw_text(FL_ALIGN_BOTTOM, (sp->xi + sp->xf) / 2,
		ob->y + ob->h - bw,
		1, 1, ob->col2, sp->lstyle, sp->lsize, sp->xlabel);

    (sp->yscale == FL_LOG ? add_logytics : add_ytics) (ob);

    if (sp->ylabel && *sp->ylabel)
    {
	int cw = fl_get_char_width(sp->lstyle, sp->lsize), w;
	int ch = fl_get_char_height(sp->lstyle, sp->lsize, 0, 0);
	int jj, nc = strlen(sp->ylabel);
	char ss[2];

	for (ss[1] = '\0', jj = 0; jj < nc; jj++)
	{
	    ss[0] = sp->ylabel[jj];
	    w = fl_get_string_width(sp->lstyle, sp->lsize, ss, 1);
	    fl_drw_text(FL_ALIGN_RIGHT, sp->xi - sp->maxytic + 4 - (cw - w) / 2,
			(sp->yi + sp->yf) / 2 + (jj - nc / 2) * ch,
			1, 1, ob->col2, sp->lstyle, sp->lsize, ss);
	}
    }

    fl_unset_text_clipping();
    fl_unset_clipping();
    return;
}


static void draw_curve_only(FL_OBJECT *);

/* For active xyplot only. point (ux,uy) is dragged to a new position.

 * Only need to erase the old drawing and update. draw_xyplot erases
 * everything by redrawing the box, can cause flicker in single buffer
 * mode.
 */
static void
update_xyplot(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    int n = sp->update - 1;
    float x, y;
    int active = sp->iactive;

    /* erase the old drawing */
    sp->col[0] = ob->col1;
    draw_curve_only(ob);

    /* update */
    sp->col[0] = ob->col2;
    sp->x[active][n] = sp->ux;
    sp->y[active][n] = sp->uy;

    /* due to possible double buffering, switch mapping */
    w2s_draw(ob, sp->ux, sp->uy, &x, &y);

    sp->xpactive[n - sp->n1].x = x;
    sp->xpactive[n - sp->n1].y = y;

    add_border(sp, ob->col2);
    draw_curve_only(ob);

}

/* find the data point the mouse falls on. Since log scale is
 * non-linear, can't do search in world coordinates given pixel-delta,
 * thus the xpactive keeps the active screen data.
 */
static int
find_data(FL_OBJECT * ob, int deltax, int deltay, int mx, int my, int *n)
{
    SPEC *sp = ob->spec;
    register int found, i, dist, mindist, done, newi;
    register int dx = 0;	/* to shut -Wall up */
    register int dy = 0;	/* same here */
    register FL_POINT *p = sp->xpactive;

    for (i = found = 0; i < sp->n[0] && !found; i++)
    {
	dx = p[i].x - mx;
	dy = p[i].y - my;
	found = (FL_abs(dx) < deltax && FL_abs(dy) < deltay);
    }

    /* it's possible that the first point we found is not the
       closest. Check for the closest using a linear distance */
    mindist = FL_abs(dx) + FL_abs(dy);
    newi = i;

    for ( done = !found; !done && i < sp->n[0]; i++)
    {
	dx = p[i].x - mx;
	dy = p[i].y - my;
        dist = FL_abs(dx) + FL_abs(dy);
        if(dist < mindist)
        {
            mindist = dist;
            newi = i+1; /* need to overshoot */
        }
        else
           done = 1;
    }

    /* n overshoots by 1 and we're dependent on that ! */
    *n = newi;
    return found;
}

static int
handle_mouse(FL_OBJECT * ob, FL_Coord mx, FL_Coord my)
{
    SPEC *sp = ob->spec;
    static FL_Coord lmx, lmy;
    int i;
    int deltax = (sp->ssize * (sp->inspect ? 2.5f : 1.5f) + 1);
    int deltay = deltax;
    float fmx, fmy;
    float xmin = FL_min(sp->xmin, sp->xmax), xmax = FL_max(sp->xmax, sp->xmin);
    float ymin = FL_min(sp->ymin, sp->ymax), ymax = FL_max(sp->ymax, sp->ymin);

    if (sp->n[0] == 0 || !sp->x || (!sp->active && !sp->inspect))
	return 0;

    if (lmx == mx && lmy == my)
    {
	if (sp->inside < 0)
	    fl_set_cursor(FL_ObjWin(ob), XC_tcross);
	return 0;
    }

    lmx = mx;
    lmy = my;

    mx -= ob->x;
    my -= ob->y;

    if (sp->inside <= 0)
    {
	int found = find_data(ob, deltax, deltay, mx, my, &i);
	/* found always overshoots one and we are dependent on that! */
	if ((sp->inside = (found ? i : 0)))
	    fl_set_cursor(FL_ObjWin(ob), XC_tcross);
	return 0;
    }

    if (sp->inspect)
    {
	int found = find_data(ob, deltax, deltay, mx, my, &i);
	if (!(sp->inside = (found ? i : 0)))
	    fl_reset_cursor(FL_ObjWin(ob));

	/* if delayed action, can't keep up with motion event */
#ifndef DELAYED_ACTION
	return (sp->inside && sp->how_return);
#else
	return 0;
#endif
    }

    fmx = (lmx - sp->bxm) / sp->ax;
    fmy = (lmy - sp->bym) / sp->ay;

    if (sp->xscale == FL_LOG)
	fmx = pow(sp->xbase, fmx);

    if (sp->yscale == FL_LOG)
	fmy = pow(sp->ybase, fmy);

    /* update data and redraw. Need to enforce the bounds */
    i = sp->inside - 1;

    if (fmx < xmin)
	fmx = xmin;
    else if (fmx > xmax)
	fmx = xmax;

    if (fmy < ymin)
	fmy = ymin;
    else if (fmy > ymax)
	fmy = ymax;

    /* fix the end points and don't allow crossings */
    if (i == 0 || i == sp->n[0] - 1)
	fmx = sp->x[0][i];
    else
    {
	/* here we need to leave some seperation. Otherwise too much error in
	   interpolation */

	if (fmx >= sp->x[0][i + 1])
	{
	    if (sp->xscale == FL_LOG)
		fmx = sp->x[0][i + 1] - (sp->x[0][i + 1] - sp->x[0][i]) / 100;
	    else
		fmx = sp->x[0][i + 1] - 1.0f / sp->ax;
	}
	else if (fmx <= sp->x[0][i - 1])
	{
	    if (sp->xscale == FL_LOG)
		fmx = sp->x[0][i - 1] + (sp->x[0][i] - sp->x[0][i - 1]) / 100;
	    else
		fmx = sp->x[0][i - 1] + 1.0f / sp->ax;
	}
    }

    sp->ux = fmx;
    sp->uy = fmy;

    /* suspend drawing of tics etc */
    sp->update = i + 1;

    fl_redraw_object(ob);

    sp->x[0][i] = fmx;
    sp->y[0][i] = fmy;

    return sp->how_return;
}

static int
handle_it(FL_OBJECT * ob, int event, FL_Coord mx, FL_Coord my, int key, void *ev)
{
    int ret = 0;
    SPEC *sp = ob->spec;

#if (FL_DEBUG >= ML_DEBUG)
    M_info2("HandleXYPlot", fl_event_name(event));
#endif

    sp->lsize = ob->lsize;
    sp->lstyle = ob->lstyle;
    sp->col[0] = ob->col2;

    switch (event)
    {
    case FL_DRAW:
	if (ob->flpixmap)
	{
	    sp->objx = ((FL_pixmap *) (ob->flpixmap))->x;
	    sp->objy = ((FL_pixmap *) (ob->flpixmap))->y;
	}
	else
	{
	    sp->objx = ob->x;
	    sp->objy = ob->y;
	}

	(sp->update ? update_xyplot : draw_xyplot) (ob);
	sp->update = 0;
	break;
    case FL_DRAWLABEL:
	fl_draw_object_label(ob);
	break;
    case FL_ENTER:
	sp->objx = ob->x;
	sp->objy = ob->y;
    case FL_LEAVE:
	break;
    case FL_PUSH:
    case FL_MOUSE:
	sp->objx = ob->x;
	sp->objy = ob->y;
	ret = handle_mouse(ob, mx, my);
	break;
    case FL_RELEASE:
	if (!sp->active && !sp->inspect)
	    return 0;
	/* mark the release by setting inside < 0 */
	if (sp->inside > 0)
	    sp->inside *= -1;
	fl_reset_cursor(FL_ObjWin(ob));
	ret = (!sp->how_return || sp->inspect) && sp->inside;
	break;
    case FL_FREEMEM:
	free_xyplot(ob);
	fl_free(sp);
	break;
    }

    if (ret && sp->inside == 0)
	fprintf(stderr, "Something is wrong\n");

    return ret;
}

#define REALLOC(a,n)   fl_realloc(a, (n) * sizeof(*a))
#define CALLOC(a,n)    fl_calloc((n), sizeof(*a))

static int
allocate_spec(SPEC *sp, int n)
{
    int i, i0, oldn = sp->maxoverlay, np1;

    if (n < sp->maxoverlay && sp->maxoverlay > FL_MAX_XYPLOTOVERLAY)
	return oldn;

    i0 = sp->maxoverlay + (sp->maxoverlay > 0);
    sp->maxoverlay = n;

    np1 = sp->maxoverlay + 1;

    if (sp->text)
    {
	sp->text = REALLOC(sp->text, np1);
	sp->xt = REALLOC(sp->xt, np1);
	sp->yt = REALLOC(sp->yt, np1);
	sp->x = REALLOC(sp->x, np1);
	sp->y = REALLOC(sp->y, np1);
	sp->grid = REALLOC(sp->grid, np1);
	sp->col = REALLOC(sp->col, np1);
	sp->tcol = REALLOC(sp->tcol, np1);
	sp->type = REALLOC(sp->type, np1);
	sp->n = REALLOC(sp->n, np1);
	sp->interpolate = REALLOC(sp->interpolate, np1);
	sp->talign = REALLOC(sp->talign, np1);
	sp->thickness = REALLOC(sp->thickness, np1);
	sp->key = REALLOC(sp->key, np1);
	sp->symbol = REALLOC(sp->symbol, np1);
    }
    else
    {
	sp->text = CALLOC(sp->text, np1);
	sp->xt = CALLOC(sp->xt, np1);
	sp->yt = CALLOC(sp->yt, np1);
	sp->x = CALLOC(sp->x, np1);
	sp->y = CALLOC(sp->y, np1);
	sp->grid = CALLOC(sp->grid, np1);
	sp->col = CALLOC(sp->col, np1);
	sp->tcol = CALLOC(sp->tcol, np1);
	sp->type = CALLOC(sp->type, np1);
	sp->n = CALLOC(sp->n, np1);
	sp->interpolate = CALLOC(sp->interpolate, np1);
	sp->talign = CALLOC(sp->talign, np1);
	sp->thickness = CALLOC(sp->thickness, np1);
	sp->key = CALLOC(sp->key, np1);
	sp->symbol = CALLOC(sp->symbol, np1);
    }

    for (i = i0; i <= sp->maxoverlay; i++)
    {
	sp->x[i] = sp->y[i] = 0;
	sp->text[i] = 0;
	sp->n[i] = 0;
	sp->interpolate[i] = 0;
	sp->type[i] = -1;
	sp->thickness[i] = 0;
	sp->key[i] = 0;
    }

    return oldn;
}

/* all mem that does not depend on the xy-data */
static void
free_spec_dynamic_mem(SPEC *sp)
{
    fl_free(sp->text);
    fl_free(sp->xt);
    fl_free(sp->yt);
    fl_free(sp->x);
    fl_free(sp->y);
    fl_free(sp->grid);
    fl_free(sp->col);
    fl_free(sp->tcol);
    fl_free(sp->type);
    fl_free(sp->n);
    fl_free(sp->interpolate);
    fl_free(sp->talign);
    fl_free(sp->thickness);
    fl_free(sp->key);
}

static void
init_spec(FL_OBJECT * ob, SPEC *sp)
{
    allocate_spec(sp, FL_MAX_XYPLOTOVERLAY);

    sp->xscale = sp->yscale = FL_LINEAR;
    sp->xbase = sp->ybase = 10.0;
    sp->lxbase = sp->lybase = 1.0;
    sp->xautoscale = sp->yautoscale = 1;
    sp->xmajor = XMAJOR;
    sp->ymajor = YMAJOR;
    sp->xminor = XMINOR;
    sp->yminor = YMINOR;
    sp->ssize = 4;
    sp->lsize = ob->lsize;
    sp->lstyle = ob->lstyle;
    sp->grid_linestyle = FL_DOT;
    sp->wx = fl_malloc(sizeof(float));
    sp->wy = fl_malloc(sizeof(float));
    sp->how_return = FL_RETURN_END_CHANGED;

    sp->objx = ob->x;
    sp->objy = ob->y;
    sp->active = ob->active;
    sp->key_lsize = ob->lsize;
    sp->key_lstyle = ob->lstyle;
    sp->type[0] = ob->type;

    sp->nxpi = 1;
    sp->xpi = fl_malloc((sp->nxpi + 3) * sizeof(FL_POINT));
    sp->xpi++;

    sp->cur_nxp = 1;
    sp->xp = fl_malloc((sp->cur_nxp + 3) * sizeof(FL_POINT));
    sp->xp++;
    sp->xpactive = fl_malloc((sp->cur_nxp + 3) * sizeof(FL_POINT));

    sp->mark_active = 1;
}


FL_OBJECT *
fl_create_xyplot(int t, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h,
		 const char *l)
{
    FL_OBJECT *ob;
    SPEC *sp;

    ob = fl_make_object(FL_XYPLOT, t, x, y, w, h, l, handle_it);

    ob->boxtype = FL_XYPLOT_BOXTYPE;
    ob->active = (t == FL_ACTIVE_XYPLOT);
    ob->col2 = ob->lcol = FL_BLACK;
    ob->col1 = FL_COL1;

    ob->lsize = FL_TINY_FONT;
    ob->align = FL_XYPLOT_ALIGN;
    sp = ob->spec = fl_calloc(1, sizeof(SPEC));

    init_spec(ob, sp);

    if (!fl_xyplot_return)
	fl_xyplot_return = fl_set_xyplot_return;

    return ob;
}

void
fl_get_xyplot_data(FL_OBJECT * ob, float *x, float *y, int *n)
{
    SPEC *sp = ob->spec;

    *n = 0;
    if (sp->n[0] > 0)
    {
	memcpy(x, sp->x[0], sp->n[0] * sizeof(float));
	memcpy(y, sp->y[0], sp->n[0] * sizeof(float));
	*n = sp->n[0];
    }
}

void
fl_replace_xyplot_point(FL_OBJECT * ob, int i, double x, double y)
{
    SPEC *sp = ob->spec;

    if (i < 0 || i >= sp->n[0])
	return;

    if ((sp->x[0][i] != x) || (sp->y[0][i] != y))
    {
	sp->x[0][i] = x;
	sp->y[0][i] = y;
	fl_redraw_object(ob);
    }
}

void
fl_get_xyplot(FL_OBJECT * ob, float *x, float *y, int *i)
{
    SPEC *sp = ob->spec;

    *i = FL_abs(sp->inside) - 1;

    if (*i < 0 || *i >= sp->n[0])
    {
	*i = -1;
	return;
    }

    *x = sp->x[0][*i];
    *y = sp->y[0][*i];
}

int
fl_set_xyplot_maxoverlays(FL_OBJECT * ob, int maxover)
{
    return allocate_spec(ob->spec, maxover);
}

FL_OBJECT *
fl_add_xyplot(int t, FL_Coord x, FL_Coord y, FL_Coord w, FL_Coord h, const char *l)
{
    FL_OBJECT *ob = fl_create_xyplot(t, x, y, w, h, l);

    fl_add_object(fl_current_form, ob);

    /* active_xyplot looks a little better in double buffer mode */
    fl_set_object_dblbuffer(ob, ob->active);
    return ob;
}

void
fl_set_xyplot_return(FL_OBJECT * ob, int when)
{
    if (ob->type == FL_ACTIVE_XYPLOT)
	((SPEC *) ob->spec)->how_return = when;
}

void
fl_set_xyplot_symbolsize(FL_OBJECT * ob, int n)
{
    ((SPEC *) ob->spec)->ssize = n;
}


FL_XYPLOT_SYMBOL
fl_set_xyplot_symbol(FL_OBJECT * ob, int id, FL_XYPLOT_SYMBOL symbol)
{
    SPEC *sp = ob->spec;
    FL_XYPLOT_SYMBOL old = 0;
    int i;

    if (id > sp->maxoverlay)
    {
	M_err("xyplot_symbol", "ID %d is not in range (0,%d)",
	      id, sp->maxoverlay);
	return 0;
    }

    for (i = 0; i <= sp->maxoverlay; i++)
    {
	if (i == id || id < 0)
	{
	    old = sp->symbol[i];
	    if (sp->symbol[i] != symbol)
	    {
		sp->symbol[i] = symbol;
		fl_redraw_object(ob);
	    }
	}
    }

    return old;
}


void
fl_set_xyplot_inspect(FL_OBJECT * ob, int yes)
{
    SPEC *sp = ob->spec;

    if (sp->inspect != yes)
    {
	ob->active = sp->inspect = yes;
	if (ob->type == FL_ACTIVE_XYPLOT)
	    ob->active = 1;
	else
	{
	    /* work-around, need to get doublebuffer to get inspect work
	       right */
	    fl_set_object_dblbuffer(ob, sp->active || sp->inspect);
	    fl_redraw_object(ob);
	}
    }
}

void
fl_set_xyplot_xtics(FL_OBJECT * ob, int major, int minor)
{
    SPEC *sp = ob->spec;
    int ma = major ? major : XMAJOR;
    int mi = minor ? minor : XMINOR;

    if (major > MAX_MAJOR)
	major = MAX_MAJOR;

    if (minor > MAX_MINOR)
	minor = MAX_MINOR;

    if ((major * minor) >= MAX_TIC)
    {
	M_err("xtics", "major*minor should be less than %d", MAX_TIC);
	minor = 5;
	major = 10;
    }

    if (sp->xmajor != ma || sp->xminor != mi)
    {
	/* 0 restores the default */
	sp->xmajor = major ? major : XMAJOR;
	sp->xminor = minor ? minor : XMINOR;
	if (sp->axtic[0])
	    free_atic(sp->axtic);
	fl_redraw_object(ob);
    }
}

void
fl_set_xyplot_xgrid(FL_OBJECT * ob, int xgrid)
{
    SPEC *sp = ob->spec;

    if (sp->xgrid != xgrid)
    {
	sp->xgrid = xgrid;
	fl_redraw_object(ob);
    }
}

void
fl_set_xyplot_ygrid(FL_OBJECT * ob, int ygrid)
{
    SPEC *sp = ob->spec;

    if (sp->ygrid != ygrid)
    {
	sp->ygrid = ygrid;
	fl_redraw_object(ob);
    }
}

int
fl_set_xyplot_grid_linestyle(FL_OBJECT * ob, int style)
{
    SPEC *sp = ob->spec;
    int ostyle = sp->grid_linestyle;

    if (sp->grid_linestyle != style)
    {
	sp->grid_linestyle = style;
	fl_redraw_object(ob);
    }

    return ostyle;
}

void
fl_set_xyplot_xbounds(FL_OBJECT * ob, double xmin, double xmax)
{
    SPEC *sp = ob->spec;
    int xs = (xmax == xmin);

    if (sp->xautoscale != xs || sp->xmin != xmin || sp->xmax != xmax)
    {
	sp->xautoscale = (xmax == xmin);
	sp->xmax = xmax;
	sp->xmin = xmin;
	find_xbounds(sp);
#if (FL_DEBUG >= ML_DEBUG)
	M_warn("XYPlotxbounds", "auto: %d xmin=%f xmax=%f",
	       sp->xautoscale, sp->xmin, sp->xmax);
#endif
	fl_redraw_object(ob);
    }
}

void
fl_get_xyplot_xbounds(FL_OBJECT * ob, float *xmin, float *xmax)
{
    SPEC *sp = ob->spec;

    *xmin = sp->xmin;
    *xmax = sp->xmax;
}

void
fl_get_xyplot_ybounds(FL_OBJECT * ob, float *ymin, float *ymax)
{
    SPEC *sp = ob->spec;

    *ymin = sp->ymin;
    *ymax = sp->ymax;
}

void
fl_set_xyplot_ybounds(FL_OBJECT * ob, double ymin, double ymax)
{
    SPEC *sp = ob->spec;
    int ys = (ymax == ymin);

    if (sp->yautoscale != ys || sp->ymin != ymin || sp->ymax != ymax)
    {
	sp->yautoscale = (ymax == ymin);
	sp->ymax = ymax;
	sp->ymin = ymin;
	find_ybounds(sp);
	fl_redraw_object(ob);
    }
}


void
fl_set_xyplot_ytics(FL_OBJECT * ob, int major, int minor)
{
    SPEC *sp = ob->spec;
    int yma = major ? major : YMAJOR;
    int ymi = minor ? minor : YMINOR;

    if (major > MAX_MAJOR)
	major = MAX_MAJOR;

    if (minor > MAX_MINOR)
	minor = MAX_MINOR;

    if (sp->ymajor != yma || sp->yminor != ymi)
    {
	/* 0 restores the default */
	sp->ymajor = yma;
	sp->yminor = ymi;
	if (sp->aytic[0])
	    free_atic(sp->aytic);

	if (sp->ymajor < 0)
	    sp->ytic = -1;

	fl_redraw_object(ob);
    }
}

static void
get_min_max(float *x, int n, float *xmin, float *xmax)
{
    float *xs = x + n;

    if (!x || !n)
	return;

    *xmin = *xmax = *x++;
    for (; x < xs; x++)
    {
	if (*xmin > *x)
	    *xmin = *x;
	else if (*xmax < *x)
	    *xmax = *x;
    }
}

static void
find_xbounds(SPEC *sp)
{
    if (sp->xautoscale)
	get_min_max(sp->x[0], sp->n[0], &(sp->xmin), &(sp->xmax));

    if ((sp->xmax - sp->xmin) == 0.0)
    {
	sp->xmin -= 1.0f;
	sp->xmax += 1.0f;
    }
}

static void
find_ybounds(SPEC *sp)
{
    if (sp->yautoscale)
	get_min_max(sp->y[0], sp->n[0], &(sp->ymin), &(sp->ymax));

    if ((sp->ymax - sp->ymin) == 0.0f)
    {
	sp->ymin -= 1.0f;
	sp->ymax += 1.0f;
    }
}

/* overloading would'v been nice ... */
int
fl_set_xyplot_data_double(FL_OBJECT * ob, double *x, double *y, int n,
		   const char *title, const char *xlabel, const char *ylabel)
{
    SPEC *sp = ob->spec;
    int i;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_XYPLOT))
    {
	Bark("AddXyplotData", "%s not an xyplot", ob ? ob->label : "");
	return FL_BAD_OBJECT;
    }
#endif

    free_overlay_data(sp, 0);

    Free(sp->xlabel);
    Free(sp->ylabel);
    Free(sp->title);

    sp->xlabel = fl_strdup(xlabel ? xlabel : "");
    sp->ylabel = fl_strdup(ylabel ? ylabel : "");
    sp->title = fl_strdup(title ? title : "");

    sp->x[0] = fl_malloc(n * sizeof(double));
    sp->y[0] = fl_malloc(n * sizeof(double));

    if (!sp->x[0] || !sp->y[0])
    {
	M_err("SetXYplotData", "Can't allocate memory");
	return FL_ALLOC;
    }

    extend_screen_data(sp, n);
    for ( i = 0; i < n ; i++)
    {
       sp->x[0][i] = x[i];
       sp->y[0][i] = y[i];
    }

    sp->n[0] = n;

    find_xbounds(sp);
    find_ybounds(sp);

    fl_redraw_object(ob);
    return FL_OK;
}


int
fl_set_xyplot_data(FL_OBJECT * ob, float *x, float *y, int n,
		   const char *title, const char *xlabel, const char *ylabel)
{
    SPEC *sp = ob->spec;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_XYPLOT))
    {
	Bark("AddXyplotData", "%s not an xyplot", ob ? ob->label : "");
	return FL_BAD_OBJECT;
    }
#endif

    free_overlay_data(sp, 0);

    Free(sp->xlabel);
    Free(sp->ylabel);
    Free(sp->title);

    sp->xlabel = fl_strdup(xlabel ? xlabel : "");
    sp->ylabel = fl_strdup(ylabel ? ylabel : "");
    sp->title = fl_strdup(title ? title : "");

    sp->x[0] = fl_malloc(n * sizeof(float));
    sp->y[0] = fl_malloc(n * sizeof(float));

    if (!sp->x[0] || !sp->y[0])
    {
	M_err("SetXYplotData", "Can't allocate memory");
	return FL_ALLOC;
    }

    extend_screen_data(sp, n);

    memcpy(sp->x[0], x, n * sizeof(float));
    memcpy(sp->y[0], y, n * sizeof(float));
    sp->n[0] = n;

    find_xbounds(sp);
    find_ybounds(sp);

    fl_redraw_object(ob);

    return FL_OK;
}

/* insert a point after n */
void
fl_insert_xyplot_data(FL_OBJECT * ob, int id, int n,
		      double x, double y)
{
    SPEC *sp = ob->spec;
    float *xx, *yy;

    if (id < 0 || id > sp->maxoverlay)
	return;

    if (n < -1)
	n = -1;
    else if (n >= sp->n[id])
	n = sp->n[id] - 1;

    n = n + 1;
    sp->n[id] += 1;

    if (n == sp->n[id] - 1)
    {
	sp->x[id] = fl_realloc(sp->x[id], sp->n[id] * sizeof(*sp->x[id]));
	sp->y[id] = fl_realloc(sp->y[id], sp->n[id] * sizeof(*sp->y[id]));
	sp->x[id][n] = x;
	sp->y[id][n] = y;
    }
    else
    {

	xx = fl_malloc(sp->n[id] * sizeof(*xx));
	yy = fl_malloc(sp->n[id] * sizeof(*yy));
	if (n)
	{
	    memcpy(xx, sp->x[id], n * sizeof(*xx));
	    memcpy(yy, sp->y[id], n * sizeof(*yy));
	}
	xx[n] = x;
	yy[n] = y;
	memcpy(xx + n + 1, sp->x[id] + n, sizeof(*xx) * (sp->n[id] - n - 1));
	memcpy(yy + n + 1, sp->y[id] + n, sizeof(*yy) * (sp->n[id] - n - 1));
	fl_free(sp->x[id]);
	fl_free(sp->y[id]);
	sp->x[id] = xx;
	sp->y[id] = yy;
    }

    extend_screen_data(sp, sp->n[id]);

    fl_redraw_object(ob);
}

/*
 */
void
fl_add_xyplot_overlay(FL_OBJECT * ob, int id,
		      float *x, float *y, int n, FL_COLOR col)
{

    SPEC *sp;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_XYPLOT))
    {
	Bark("XYPlotOverlay", "%s not XYPLOT class", ob ? ob->label : "");
	return;
    }
#endif

    sp = ob->spec;

    if (id < 1 || id > sp->maxoverlay)
    {
	M_err("add_xyplot_overlay", "ID %d is not in range (1,%d)",
	      id, sp->maxoverlay);
	return;
    }

    /* free old data */
    free_overlay_data(sp, id);

    /* copy data */
    sp->x[id] = fl_malloc(n * sizeof(float));
    sp->y[id] = fl_malloc(n * sizeof(float));
    memcpy(sp->x[id], x, n * sizeof(float));
    memcpy(sp->y[id], y, n * sizeof(float));
    sp->n[id] = n;

    /* extend screen points if needed. */
    extend_screen_data(sp, n);

    sp->col[id] = col;

    /* set default only for the first time */
    if (sp->type[id] == -1)
	sp->type[id] = ob->type;

    fl_redraw_object(ob);
}

#define MAXP  1024		/* this is the initial space */

static int
load_data(const char *f, float **xx, float **yy)
{
    int n, err;
    FILE *fp;
    char buf[128];
    float *x, *y;
    int maxp = MAXP, ncomment = 0;

    if (!f || !(fp = fopen(f, "r")))
    {
	Bark("XYplotFile", "can't open datafile %s", f ? f : "null");
	return 0;
    }

    x = *xx = fl_malloc(maxp * sizeof(float));
    y = *yy = fl_malloc(maxp * sizeof(float));

    /* comment chars are ; # ! and seperators are spaces and tabs  */
    n = err = 0;
    while (fgets(buf, sizeof(buf), fp) && n < maxp && !err)
    {
	if (buf[0] == '!' || buf[0] == '#' || buf[0] == ';' || buf[0] == '\n')
	{
	    ncomment++;
	    continue;
	}

	if (!(err = (sscanf(buf, "%f%*[ \t,]%f", x + n, y + n) != 2)))
	{
	    n++;
	    if (n >= maxp - 1)
	    {
		maxp *= 2;
		x = *xx = fl_realloc(x, maxp * sizeof(float));
		y = *yy = fl_realloc(y, maxp * sizeof(float));
	    }
	}
    }

    fclose(fp);

    if (err)
	M_err("XyplotFile", "An error occured at line %d", n + ncomment);

    if (n == 0)
    {
	fl_free(*xx);
	fl_free(*yy);
    }

    return n;
}

int
fl_add_xyplot_overlay_file(FL_OBJECT * ob, int id, const char *f, FL_COLOR c)
{
    float *x, *y;
    int n;

    if ((n = load_data(f, &x, &y)) > 0)
    {
	fl_add_xyplot_overlay(ob, id, x, y, n, c);
	fl_free(x);
	fl_free(y);
    }
    return n;
}

void
fl_set_xyplot_overlay_type(FL_OBJECT * ob, int id, int type)
{
    SPEC *sp;

    if (id < 0 || !ob || id > (sp = ob->spec)->maxoverlay)
	return;

    sp = ob->spec;
    if (sp->type[id] != type)
    {
	sp->type[id] = type;
	fl_redraw_object(ob);
    }
}

int
fl_get_xyplot_numdata(FL_OBJECT * ob, int id)
{
    SPEC *sp;

    if (id < 0 || !ob || id > (sp = ob->spec)->maxoverlay)
	return 0;
    return sp->n[id];
}

void
fl_delete_xyplot_overlay(FL_OBJECT * ob, int id)
{
    SPEC *sp = ob->spec;

    if (id > 0 && id <= sp->maxoverlay && sp->n[id])
    {
	free_overlay_data(sp, id);
	sp->type[id] = -1;
	fl_redraw_object(ob);
    }

}

void
fl_get_xyplot_overlay_data(FL_OBJECT * ob, int id, float *x, float *y,
			   int *n)
{
    SPEC *sp = ob->spec;

    if (id >= 0 && id <= sp->maxoverlay && sp->n[id])
    {
	memcpy(x, sp->x[id], sp->n[id] * sizeof(float));
	memcpy(y, sp->y[id], sp->n[id] * sizeof(float));
	*n = sp->n[id];
    }
}

void
fl_get_xyplot_data_pointer(FL_OBJECT * ob, int id, float **x, float **y, int *n)
{
    SPEC *sp = ob->spec;

    *n = 0;
    if (id >= 0 && id <= sp->maxoverlay && sp->n[id])
    {
	*x = sp->x[id];
	*y = sp->y[id];
	*n = sp->n[id];
    }
}

void
fl_set_xyplot_linewidth(FL_OBJECT * ob, int id, int lw)
{
    SPEC *sp = ob->spec;

    if (id >= 0 && id <= sp->maxoverlay && lw != sp->thickness[id])
    {
	sp->thickness[id] = lw;
	fl_redraw_object(ob);
    }
}

int
fl_set_xyplot_file(FL_OBJECT * ob, const char *f, const char *title,
		   const char *xl, const char *yl)
{
    float *x, *y;
    int n;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_XYPLOT))
    {
	Bark("AddXyplotDataFile", "%s not an xyplot", ob ? ob->label : "");
	return 0;
    }
#endif

    if ((n = load_data(f, &x, &y)) > 0)
    {
	fl_set_xyplot_data(ob, x, y, n, title, xl, yl);
	fl_free(x);
	fl_free(y);
    }
    return n;
}

void
fl_add_xyplot_text(FL_OBJECT * ob, double x, double y, const char *text,
		   int al, FL_COLOR col)
{
    SPEC *sp;
    int i;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_XYPLOT))
    {
	Bark("AddXyplotText", "%s not an xyplot", ob ? ob->label : "");
	return;
    }
#endif

    sp = ob->spec;

    /* find an appropriate slot for this */
    for (i = 0; sp->text[i] && i < sp->maxoverlay; i++)
	;
    if (i < sp->maxoverlay)
    {
	sp->text[i] = fl_strdup(text);
	sp->xt[i] = x;
	sp->yt[i] = y;
	sp->talign[i] = al;
	sp->tcol[i] = col;
	fl_redraw_object(ob);
    }
}

void
fl_delete_xyplot_text(FL_OBJECT * ob, const char *text)
{
    SPEC *sp;
    int i;

#if (FL_DEBUG >= ML_ERR)
    if (!IsValidClass(ob, FL_XYPLOT))
    {
	Bark("RemoveXYplotText", "%s not an xyplot", ob ? ob->label : "");
	return;
    }
#endif

    for (sp = ob->spec, i = 0; i < sp->maxoverlay; i++)
	if (sp->text[i] && strcmp(sp->text[i], text) == 0)
	{
	    fl_free(sp->text[i]);
	    sp->text[i] = 0;
	    fl_redraw_object(ob);
	}
}

void
fl_set_xyplot_interpolate(FL_OBJECT * ob, int id, int deg, double grid)
{
    int intpl;
    SPEC *sp;

    if (id < 0 || !ob || id > ((SPEC *) (ob->spec))->maxoverlay)
	return;

    if (deg >= 2 && grid <= 0.0f)
	return;

    intpl = (deg <= 1) ? 0 : ((deg < 2 || deg > 7) ? 2 : deg);

    sp = ob->spec;
    if (sp->interpolate[id] != intpl)
    {
	sp->interpolate[id] = intpl;
	sp->grid[id] = grid;
	fl_redraw_object(ob);
    }

}

void
fl_set_xyplot_fontsize(FL_OBJECT * ob, int size)
{
    if (ob->lsize != size)
    {
	ob->lsize = size;
	fl_redraw_object(ob);
    }
}

void
fl_set_xyplot_fontstyle(FL_OBJECT * ob, int style)
{
    if (ob->lstyle != style)
    {
	ob->lstyle = style;
	fl_redraw_object(ob);
    }
}

#define ADVANCE  0.1f		/* shoulde not be greater than 0.25 */
#define FACTOR   1.9f		/* max major expansion              */

static float trunc_f(float, int);

static float
gen_tic(float tmin, float tmax, int major, int minor)
{
    float r_range, l_range, n_range;
    float tic;
    int ipow, digit;

    /* handle special case: Min, MAX and one tic */
    if (major == 1 && minor == 2)
    {
	tic = FL_abs(tmax - tmin) * 0.5f;
	return tic;
    }

    r_range = FL_abs(tmax - tmin);
    l_range = (float) log10(r_range);
    ipow = (int) ((l_range > 0.0) ? (int) l_range : (int) l_range - 1);

    /* normalized range is between 0 and 10 */
    n_range = pow(10.0, (l_range - ipow));
    n_range += ADVANCE;

    if (n_range <= major && n_range >= major * 0.5f)
	tic = 1.0f;
    else
	tic = n_range / major;

    digit = (tic < 1.0f);
    tic = trunc_f(tic, digit);
    tic /= (float) minor;
    tic = trunc_f(tic, 1);
    tic *= pow(10.0, (double) ipow);

    /* Final check */
    n_range = (r_range / (tic * minor * major));
    if (n_range > FACTOR)
    {
	ipow = (n_range / FACTOR);
	if (ipow >= 1)
	    tic *= 2.0f * ipow;
	else
	    tic *= 2.0f * n_range / FACTOR;
	tic = trunc_f(tic, 1);
    }

    return tic;
}

static float
gen_logtic(float tmin, float tmax, float base, int major, int minor)
{
    float tic, lbase = log10(base), tmp;

    if (tmin <= 0.0f || tmax <= 0.0f)
    {
	M_err("GenTics", "range must be greater than 0 for logscale");
	return -1;
    }

    if (major == 1 && minor == 2)
    {
	tic = floor(log10(tmax) / lbase + 0.0001f);
	if (tic < 1.0f)
	    tic = 1.0f;
	return tic;
    }

    tmp = log10(tmax) - log10(tmin);
    tic = floor(FL_abs(tmp) / lbase) / major + 0.01f;
    tic = floor(tic);
    if (tic < 1.0f)
	tic = 1.0f;
    return tic;
}

/* truncate a floating point number to requested significant digits */
static float
trunc_f(float f, int digit)
{
    int ipow, itmp, c;
    float expon, tmp;

    if (fabs(f) < 1.e-20)
	return 0.0f;

    if (digit < 0)
	digit = 1;

    if (!digit)
    {
	if (f > 0.5f)
	    return 1.0f;
	else if (f < 0.5f)
	    return 0.0f;
	else
	{
	    itmp = f;
	    return (float) itmp;
	}
    }

    c = digit - 1;
    expon = log10(f);
    ipow = (expon > 0.0f) ? (int) expon : (int) expon - 1;

    tmp = pow(10.0, expon - (double) ipow);
    tmp += ADVANCE;
    tmp *= pow(10.0, (double) c);
    itmp = tmp;
    tmp = (float) itmp;
    tmp *= pow(10.0, (double) (ipow - c));
    return tmp;
}


void
fl_get_xyplot_xmapping(FL_OBJECT * ob, float *a, float *b)
{
    *a = ((SPEC *) ob->spec)->ax;
    *b = ((SPEC *) ob->spec)->bxm;
}

void
fl_get_xyplot_ymapping(FL_OBJECT * ob, float *a, float *b)
{
    *a = ((SPEC *) ob->spec)->ay;
    *b = ((SPEC *) ob->spec)->bym;
}

void
fl_xyplot_s2w(FL_OBJECT * ob, double sx, double sy, float *wx, float *wy)
{
    SPEC *sp = ob->spec;

    *wx = (sx - sp->bxm) / sp->ax;
    *wy = (sy - sp->bym) / sp->ay;
    if (sp->xscale == FL_LOG)
	*wx = pow(sp->xbase, *wx);
    if (sp->yscale == FL_LOG)
	*wy = pow(sp->ybase, *wy);
}

/* draw means we only call this at draw time, thus
 * should be ax, bx, not ax, bxm
 */
static void
w2s_draw(FL_OBJECT * ob, double wx, double wy, float *sx, float *sy)
{
    SPEC *sp = ob->spec;
    float sbx = sp->bxm, sby = sp->bym;

    sp->bxm = sp->bx;
    sp->bym = sp->by;
    fl_xyplot_w2s(ob, wx, wy, sx, sy);
    sp->bxm = sbx;
    sp->bym = sby;
}

/* really should be ints for the screen coordinates */
void
fl_xyplot_w2s(FL_OBJECT * ob, double wx, double wy, float *sx, float *sy)
{
    SPEC *sp = ob->spec;

    if (sp->xscale == FL_LOG)
	*sx = (log10(wx) / sp->lxbase * sp->ax + sp->bxm + 0.5f);
    else
	*sx = (wx * sp->ax + sp->bxm + 0.5f);

    if (sp->yscale == FL_LOG)
	*sy = (log10(wy) / sp->lybase * sp->ay + sp->bym + 0.5f);
    else
	*sy = (wy * sp->ay + sp->bym + 0.5f);
}

void
fl_set_xyplot_xscale(FL_OBJECT * ob, int scale, double base)
{
    SPEC *sp = ob->spec;

    if (scale == FL_LOG && (base <= 0.0 || base == 1.0))
    {
	M_err("XyplotScale", "bad log base %g specified", base);
	return;
    }

    if (sp->xscale != scale || sp->xbase != base)
    {
	sp->xscale = scale;
	if (scale == FL_LOG)
	{
	    sp->xbase = base;
	    sp->lxbase = log10(base);
	}
	fl_redraw_object(ob);
    }
}

void
fl_set_xyplot_yscale(FL_OBJECT * ob, int scale, double base)
{
    SPEC *sp = ob->spec;

    if (scale == FL_LOG && (base <= 0.0 || base == 1.0))
    {
	M_err("XyplotScale", "bad log base %g specified", base);
	return;
    }

    if (sp->yscale != scale || sp->ybase != base)
    {
	sp->yscale = scale;
	if (scale == FL_LOG)
	{
	    sp->ybase = base;
	    sp->lybase = log10(base);
	}
	fl_redraw_object(ob);
    }
}

void
fl_set_xyplot_fixed_xaxis(FL_OBJECT * ob, const char *lm, const char *rm)
{
    SPEC *sp = ob->spec;

    Free(sp->xmargin1);
    Free(sp->xmargin2);
    sp->xmargin1 = lm ? fl_strdup(lm) : 0;
    sp->xmargin2 = rm ? fl_strdup(rm) : 0;

    if (sp->xmargin2 && !sp->xmargin1)
	sp->xmargin1 = fl_strdup("");
    if (sp->xmargin1 && !sp->xmargin2)
	sp->xmargin2 = fl_strdup("");
}

void
fl_set_xyplot_fixed_yaxis(FL_OBJECT * ob, const char *bm, const char *tm)
{
    SPEC *sp = ob->spec;

    Free(sp->ymargin1);
    Free(sp->ymargin1);
    sp->ymargin1 = tm ? fl_strdup(tm) : 0;
    sp->ymargin2 = bm ? fl_strdup(bm) : 0;

    if (sp->ymargin2 && !sp->ymargin1)
	sp->ymargin1 = fl_strdup("");
    if (sp->ymargin1 && !sp->ymargin2)
	sp->ymargin2 = fl_strdup("");
}


void
fl_set_xyplot_alphaxtics(FL_OBJECT * ob, const char *m, const char *s)
{
    SPEC *sp = ob->spec;
    char *tmps, *item;
    int n;

    tmps = fl_strdup(m ? m : "");
    for (n = 0, item = strtok(tmps, "|"); item; item = strtok((char *) 0, "|"))
	sp->axtic[n++] = fl_strdup(item);

    sp->axtic[n] = 0;
    sp->xmajor = n;
    sp->xminor = 1;
    fl_free(tmps);
    fl_redraw_object(ob);
}

void
fl_set_xyplot_alphaytics(FL_OBJECT * ob, const char *m, const char *s)
{
    SPEC *sp = ob->spec;
    char *tmps, *item;
    int n;

    tmps = fl_strdup(m ? m : "");
    for (n = 0, item = strtok(tmps, "|"); item; item = strtok((char *) 0, "|"))
	sp->aytic[n++] = fl_strdup(item);
    sp->aytic[n] = 0;
    sp->ymajor = n;
    sp->yminor = 1;
    fl_free(tmps);
    fl_redraw_object(ob);
}

/* free all data and inset text, alphanumerical labels and other stuff */
void
fl_clear_xyplot(FL_OBJECT * ob)
{
    int i;
    SPEC *sp = ob->spec;


    for (i = 0; i <= sp->maxoverlay; i++)
    {
	free_overlay_data(ob->spec, i);
	free_inset_text(sp, i);
    }

    fl_redraw_object(ob);
}

void
fl_set_xyplot_key(FL_OBJECT * ob, int id, const char *key)
{
    SPEC *sp = ob->spec;

    if (id >= 0 && id < sp->maxoverlay)
    {
	Free(sp->key[id]);
	if (key && *key)
	    sp->key[id] = fl_strdup(key);
    }
}

void
fl_set_xyplot_key_position(FL_OBJECT * ob, float x, float y, int align)
{
    SPEC *sp = ob->spec;

    sp->key_x = x;
    sp->key_y = y;
    sp->key_align = align & ~FL_ALIGN_INSIDE;
    fl_redraw_object(ob);
}

void
fl_set_xyplot_keys(FL_OBJECT * ob, char **keys, float x, float y, int align)
{
    int i;
    SPEC *sp = ob->spec;

    for (i = 0; i < sp->maxoverlay && keys[i]; i++)
	fl_set_xyplot_key(ob, i, keys[i]);

    fl_set_xyplot_key_position(ob, x, y, align);
}

void
fl_set_xyplot_key_font(FL_OBJECT * ob, int style, int size)
{
    SPEC *sp = ob->spec;

    if (sp->key_lstyle != style || sp->key_lsize != size)
    {
	sp->key_lstyle = style;
	sp->key_lsize = size;
	fl_redraw_object(ob);
    }
}

int
fl_set_xyplot_mark_active(FL_OBJECT * ob, int y)
{
    SPEC *sp = ob->spec;
    int old = sp->mark_active;

    if (old != y)
    {
	sp->mark_active = y;
	fl_redraw_object(ob);
    }

    return old;
}
