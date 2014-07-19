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
 * $Id: image_genesis.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1998-2002  By T.C. Zhao 
 *  All rights reserved.
 *.
 *
 *  but genesis format. Eventually dicom will
 *  be supported
 */

#include "forms.h"
#include "flimage.h"
#include "flimage_int.h"

#define IMG_MAGIC       0x494d4746

typedef struct
{
    int magic;
    int hdr_len;
    int width;
    int height;
    int depth;
    int compress;
    int windoww;
    int windowl;
}
SPEC;

static int
GENESIS_identify(FILE * fp)
{
    char buf[4];

    fread(buf, 1, 4, fp);
    rewind(fp);
    return (buf[0] == 'I' && buf[1] == 'M' && buf[2] == 'G' && buf[3] == 'F');
}

static int
GENESIS_description(FL_IMAGE * im)
{
    SPEC *sp = fl_calloc(1, sizeof(*sp));
    FILE *fp = im->fpin;

    im->io_spec = sp;
    im->spec_size = sizeof(*sp);

    rewind(fp);
    sp->magic = fl_fget4MSBF(fp);
    sp->hdr_len = fl_fget4MSBF(fp);
    sp->width = fl_fget4MSBF(fp);
    sp->height = fl_fget4MSBF(fp);
    sp->depth = fl_fget4MSBF(fp);
    sp->compress = fl_fget4MSBF(fp);
    sp->windoww = fl_fget4MSBF(fp);
    sp->windowl = fl_fget4MSBF(fp);

    if (sp->depth < 8 || sp->compress != 1)
	return -1;

    im->type = sp->depth == 8 ? FL_IMAGE_GRAY : FL_IMAGE_GRAY16;
    im->gray_maxval = (1 << sp->depth) - 1;
    im->w = sp->width;
    im->h = sp->height;

    return 0;
}

/* convert MSBF */
static void
convert_msbf(unsigned short *pixels, int npixels)
{
    register unsigned short *p = pixels, *ps = p + npixels;
    register unsigned char *c = (unsigned char *) p;

    for (; p < ps; p++, c += 2)
	*p = (c[0] << 8) | c[1];
}


static int
GENESIS_load(FL_IMAGE * im)
{
    FILE *fp = im->fpin;
    SPEC *sp = im->io_spec;

    fseek(fp, sp->hdr_len, SEEK_SET);
    fread(im->gray[0], sp->depth / 8, im->w * im->h, fp);
    convert_msbf(im->gray[0], im->w * im->h);

    return 0;
}

void
flimage_enable_genesis(void)
{
    flimage_add_format("GE Genesis", "genesis", "ge",
		       FL_IMAGE_GRAY16 | FL_IMAGE_GRAY,
		       GENESIS_identify,
		       GENESIS_description,
		       GENESIS_load,
		       0);
}
