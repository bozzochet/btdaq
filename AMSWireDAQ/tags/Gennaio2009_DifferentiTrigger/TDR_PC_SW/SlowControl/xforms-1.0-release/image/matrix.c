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
 * $Id: matrix.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1998-2002  T.C. Zhao
 *  All rights reserved.
 *.
 *
 */
#include "forms.h"
#include "flimage.h"
#include "flimage_int.h"

void *
fl_get_matrix(int nrows, int ncols, unsigned int esize)
{
    char **mat;
    int i, stride = ncols * esize;

    if (!(mat = fl_malloc((nrows + 1) * sizeof(void *))))
	  return 0;

    mat[0] = (char *) FL_GET_MATRIX;
    if (!(mat[1] = fl_calloc(nrows * ncols, esize)))
    {
	fl_free(mat);
	return 0;
    }

    for (i = 2; i <= nrows; i++)
	mat[i] = mat[i - 1] + stride;

    return mat + 1;
}

/* given a piece of memory, make a matrix out of it */
void *
fl_make_matrix(int nrows, int ncols, unsigned int esize, void *mem)
{
    char **mat = fl_malloc((nrows + 1) * sizeof(void *));
    int i, stride = ncols * esize;

    if (mat)
    {
	mat[0] = (char *) FL_MAKE_MATRIX;
	for (mat[1] = mem, i = 2; i <= nrows; i++)
	    mat[i] = mat[i - 1] + stride;
    }
    return mat + 1;
}

void
fl_free_matrix(void *p)
{
    char **matrix = p;

    if (!p)
	return;

    if (matrix[-1] && matrix[0])
    {
	if (matrix[-1] == (char *) FL_GET_MATRIX)
	    fl_free(matrix[0]);
	matrix[0] = 0;
	matrix[-1] = (char *) FL_FREE_MATRIX;
	fl_free(matrix - 1);
    }
}

#if 0
void *
fl_get_3d(int d1, int d2, int d3, unsigned es)
{
    char **m;
    int i;

    /* get one more and mark the end with null */

    m = malloc(sizeof(*m) * (d1 + 1));
    for (i = 0; i < d1; i++)
	m[i] = fl_get_matrix(d2, d3, es);
    m[d1] = 0;

    return m;
}

void
fl_free_3d(void *p)
{
    char **m = p;

    for (; m && *m; m++)
    {
	fl_free_matrix(*m);
	*m = 0;
    }
}
#endif
