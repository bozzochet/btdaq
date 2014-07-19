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
 * $Id: image_oquant.c,v 0.14 1999/01/16 07:29:24 zhao Exp $
 *
 * Copyright (c)  1998-2002 T.C. Zhao
 *
 *  A color quantizer based on octree algorithm. The algorithm
 *  is implemented and copyright by Steve Lamont (spl@ucsd.edu)
 *  Copyright (C) 1998, Steve Lamont and the National Center for Microscopy 
 *                      and Imaging Research
 *
 *  Contact Steve for the latest version of the quantization library.
 */

#include "forms.h"
#include "flimage.h"
#include "flimage_int.h"
#include "quantize.h"

void fl_select_octree_quantizer(void)
{
   flimage_quantize_rgb = fl_octree_quantize_rgb;
   flimage_quantize_packed  = fl_octree_quantize_packed;
}

int
fl_octree_quantize_rgb(unsigned char **red,
		       unsigned char **green,
		       unsigned char **blue, int w, int h,
		       int max_color,
		       unsigned short **ci, int *actual_color,
		       int *red_lut,
		       int *green_lut,
		       int *blue_lut, FL_IMAGE * im)
{
    int i, j;
    unsigned int *rlut, *glut, *blut;
    QuantizeDatabaseP database = QuantizeInitialize(max_color,
						    QuantizeStyleLeast);
    if (im)
    {
	im->completed = 0;
	im->visual_cue(im, "Starting OctreeQuantizer ...");
    }

    for (i = 0; i < h; i++)
	for (j = 0; j < w; j++)
	    QuantizeColor(database, red[i][j], green[i][j], blue[i][j]);

    QuantizeCreateLUT(database, actual_color,
		      &rlut, &glut, &blut);

    for (i = 0; i < *actual_color; i++)
    {
	red_lut[i] = rlut[i];
	green_lut[i] = glut[i];
	blue_lut[i] = blut[i];
    }

    free(rlut);
    free(glut);
    free(blut);

    for (i = 0; i < h; i++)
	for (j = 0; j < w; j++)
	    ci[i][j] = QuantizeFindIndex(database,
					 red[i][j], green[i][j], blue[i][j]);


    QuantizeDatabaseFree(database);

    if (im)
    {
	im->completed = im->total;
	im->visual_cue(im, "Done OctreeQuantizer");
    }
    return 0;
}

int
fl_octree_quantize_packed(unsigned int **packed, int w, int h,
		      int max_color, unsigned short **ci, int *actual_color,
			  int *red_lut, int *green_lut, int *blue_lut,
			  FL_IMAGE * im)
{
    int i, n = h * w;
    unsigned int *p;
    unsigned short *ind;
    unsigned int *rlut, *glut, *blut;

    QuantizeDatabaseP database = QuantizeInitialize(max_color,
						    QuantizeStyleLeast);
    if (im)
    {
	im->completed = 0;
	im->visual_cue(im, "Starting OctreeQuantizer ...");
    }

    for (p = packed[0], i = 0; i < n; i++, p++)
	QuantizeColor(database, FL_GETR(*p), FL_GETG(*p), FL_GETB(*p));

    QuantizeCreateLUT(database, actual_color, &rlut, &glut, &blut);
    for (i = 0; i < *actual_color; i++)
    {
	red_lut[i] = rlut[i];
	green_lut[i] = glut[i];
	blue_lut[i] = blut[i];
    }

    free(rlut);
    free(glut);
    free(blut);


    for (ind = ci[0], p = packed[0], i = 0; i < n; i++, p++, ind++)
	*ind = QuantizeFindIndex(database,
				 FL_GETR(*p), FL_GETG(*p), FL_GETB(*p));

    QuantizeDatabaseFree(database);

    if (im)
    {
	im->completed = im->total;
	im->visual_cue(im, "Done OctreeQuantizer");
    }
    return 0;
}
