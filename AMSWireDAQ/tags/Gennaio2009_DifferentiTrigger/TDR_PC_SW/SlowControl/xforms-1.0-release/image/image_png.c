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
 * $Id: image_png.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *   Copyright (c) 2001-2002 T.C. Zhao
 *
 *   TODO: png should be implemented in real code
 */

#include "forms.h"
#include "flimage.h"
#include "flimage_int.h"

static int
PNG_identify(FILE * fp)
{
    char buf[9];
    static unsigned char sig[9] =
    {137, 80, 78, 71, 13, 10, 26, 10};

    fread(buf, 1, 8, fp);
    buf[8] = '\0';
    return strcmp((char *) sig, buf) == 0;
}

static int
PNG_description(FL_IMAGE * im)
{
    int status;

    static char *cmds[] =
    {
	"pngtopnm %s > %s",
	0
    };

    status = flimage_description_via_filter(im, cmds, "reading png ...", 1);
    return status;
}

static int
PNG_load(FL_IMAGE * im)
{
    fprintf(stderr, "should never been here\n");
    return -1;
}

static int
PNG_dump(FL_IMAGE * im)
{
    char *cmds[] =
    {"pnmtopng %s  > %s", 0};
    char *formats[] =
    {"ppm", "pgm", "pbm", 0};

    return flimage_write_via_filter(im, cmds, formats, 1);
}

void
flimage_enable_png(void)
{
    flimage_add_format("Portable Network Graphics", "png", "png",
		       FL_IMAGE_RGB | FL_IMAGE_GRAY,
		       PNG_identify,
		       PNG_description,
		       PNG_load,
		       PNG_dump);
}
