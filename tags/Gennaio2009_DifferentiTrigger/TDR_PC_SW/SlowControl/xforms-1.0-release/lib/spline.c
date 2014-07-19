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
 * $Id: spline.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1998-2002  T.C. Zhao
 *  All rights reserved.
 *.
 *
 * interpolate_spline interpolates a one-dimensional non-uniform
 * tabulated data onto a working grid, grid, using cubic splines.
 *
 */

#include <stdio.h>
#include "forms.h"

/* the input X should be monotonic increasing */
int
fl_spline_interpolate(const float *wx, const float *wy, int nin,
		      float *x, float *y, double grid)
{
    int i, j, k, jo, ih, im, nout;
    double sig, un, p, qn, h, a, b;
    static int nwork;
    static double *y2, *u;

    if (nin <= 3)
    {
	fputs("too few points in interpol\n", stderr);
	return -1;
    }

    if (nwork < nin)
    {
	if (y2)
	{
	    y2 = fl_realloc(y2, sizeof(*y2) * nin);
	    u = fl_realloc(u, sizeof(*u) * nin);
	}
	else
	{
	    y2 = fl_malloc(sizeof(*y2) * nin);
	    u = fl_malloc(sizeof(*u) * nin);
	}
	nwork = nin;
    }

    /* compute the second derivative */

    y2[0] = u[0] = 0.0;

    for (i = 1; i < nin - 1; i++)
    {
	sig = ((double) wx[i] - wx[i - 1]) / ((double) wx[i + 1] - wx[i - 1]);
	p = sig * y2[i - 1] + 2.0;
	y2[i] = (sig - 1.0) / p;
	u[i] = (((double) wy[i + 1] - wy[i]) / (wx[i + 1] - wx[i])) -
	    (((double) wy[i] - wy[i - 1]) / (wx[i] - wx[i - 1]));
	u[i] = (6.0 * u[i] / (wx[i + 1] - wx[i - 1]) - sig * u[i - 1]) / p;
    }

    qn = un = 0.0;
    y2[nin - 1] = (un - qn * u[nin - 2]) / (qn * y2[nin - 2] + 1.0);
    for (k = nin - 2; k >= 0; k--)
	y2[k] = y2[k] * y2[k + 1] + u[k];

    /* outputs */
    nout = (int) ((wx[nin - 1] - wx[0]) / grid + 1.01f);

    x[0] = wx[0];
    y[0] = wy[0];

    /* Start the main loop */
    for (jo = 0, i = 1; i < nout; i++)
    {
	/* better than x[i] = x[i-1] + grid; */
	x[i] = x[0] + (i * grid);

	/* center */
	j = jo;
	ih = nin;
	while ((ih - j) > 1)
	{
	    im = (ih + j) / 2;
	    if (x[i] > wx[im])
		j = im;
	    else
		ih = im;
	}

	jo = j;

	/* interpolate */
	h = wx[ih] - wx[j];
	a = (wx[ih] - x[i]) / h;
	b = (x[i] - wx[j]) / h;

	y[i] = (a * wy[j] + b * wy[ih] +
		((a * a * a - a) * y2[j] + (b * b * b - b) * y2[ih]) *
		(h * h) / 6.0);
    }

    x[nout - 1] = wx[nin - 1];
    y[nout - 1] = wy[nin - 1];

    return nout;
}

/* specialized for image processing */
int
fl_spline_int_interpolate(const int *wx, const int *wy, int nin,
			  int grid, int *y)
{
    int i, j, k, jo, ih, im, nout;
    double sig, un, p, qn, h, a, b, x;
    static int nwork;
    static double *y2, *u;

    if (nin <= 3)
    {
	fputs("too few points in interpol\n", stderr);
	return -1;
    }

    if (nwork < nin)
    {
	if (y2)
	{
	    y2 = fl_realloc(y2, sizeof(*y2) * nin);
	    u = fl_realloc(u, sizeof(*u) * nin);
	}
	else
	{
	    y2 = fl_malloc(sizeof(*y2) * nin);
	    u = fl_malloc(sizeof(*u) * nin);
	}
	nwork = nin;
    }

    y2[0] = u[0] = 0.0;

    for (i = 1; i < nin - 1; i++)
    {
	sig = (wx[i] - wx[i - 1]) / ((double) wx[i + 1] - wx[i - 1]);
	p = sig * y2[i - 1] + 2.0;
	y2[i] = (sig - 1.0) / p;
	u[i] = ((wy[i + 1] - wy[i]) / (double) (wx[i + 1] - wx[i])) -
	    ((wy[i] - wy[i - 1]) / (double) (wx[i] - wx[i - 1]));
	u[i] = (6.0 * u[i] / (wx[i + 1] - wx[i - 1]) - sig * u[i - 1]) / p;
    }

    qn = un = 0.0;
    y2[nin - 1] = (un - qn * u[nin - 2]) / (qn * y2[nin - 2] + 1.0);
    for (k = nin - 2; k >= 0; k--)
	y2[k] = y2[k] * y2[k + 1] + u[k];

    /* outputs */
    nout = (int) ((wx[nin - 1] - wx[0]) / grid + 1.01f);

    y[0] = wy[0];

    /* Start the main loop */
    for (jo = 0, i = 1; i < nout; i++)
    {
	x = wx[0] + (i * grid);

	/* center */
	j = jo;
	ih = nin - 1;
	while ((ih - j) > 1)
	{
	    im = (ih + j) / 2;
	    if (x > wx[im])
		j = im;
	    else
		ih = im;
	}

	jo = j;
	/* interpolate */
	h = wx[ih] - wx[j];
	a = (wx[ih] - x) / h;
	b = (x - wx[j]) / h;

	y[i] = (a * wy[j] + b * wy[ih] +
		((a * a * a - a) * y2[j] + (b * b * b - b) * y2[ih]) *
		(h * h) / 6.0) + 0.1;

	/* clamp */
	if (y[i] < 0)
	    y[i] = 0;
	else if (y[i] > FL_PCMAX)
	    y[i] = FL_PCMAX;
    }
    y[nout - 1] = wy[nin - 1];
    return nout;
}
