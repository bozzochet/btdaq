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
 * $Id: image_pnm.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1993, 1998-2002  By T.C. Zhao
 *  All rights reserved.
 *.
 *
 *   PNM support
 */

#include "forms.h"
#include "flimage.h"
#include "flimage_int.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct
{
    float fnorm;
    int pgm, pbm;
    int maxval;
    int w, h;
    int raw;			/* binary   */
    char s[4];			/* signature */
}
SPEC;


static int
PPM_identify(FILE * fp)
{
    char buf[2];
    fread(buf, 1, 2, fp);
    rewind(fp);
    return (buf[0] == 'P' && (buf[1] == '3' || buf[1] == '6'));
}

static int
PGM_identify(FILE * fp)
{
    char buf[2];
    fread(buf, 1, 2, fp);
    rewind(fp);
    return (buf[0] == 'P' && (buf[1] == '2' || buf[1] == '5'));
}

static int
PBM_identify(FILE * fp)
{
    char buf[2];
    fread(buf, 1, 2, fp);
    rewind(fp);
    return (buf[0] == 'P' && (buf[1] == '1' || buf[1] == '4'));
}

static void
generate_header_info(FL_IMAGE *im)
{
    SPEC *sp = im->io_spec;

    if(!im->setup->header_info || !(im->info = fl_malloc(128)))
       return;

    sprintf(im->info,"Size=(%d x %d)\nMaxVal=%d\nRaw=%d\n",
        im->w, im->h, sp->maxval, sp->raw);
}
   

static int
PNM_description(FL_IMAGE * im)
{
    SPEC *sp = fl_malloc(sizeof(*sp));
    char s[3];

    fread(s, 1, 2, im->fpin);
    im->io_spec = sp;
    s[2] = '\0';

    if ((sp->w = fl_readpint(im->fpin)) <= 0 ||
	(sp->h = fl_readpint(im->fpin)) <= 0)
    {
	flimage_error(im, "%s: can't get image size", im->infile);
	fl_free(sp);
	im->io_spec = 0;
	return -1;
    }

    im->w = sp->w;
    im->h = sp->h;

    sp->raw = (s[1] == '6' || s[1] == '5' || s[1] == '4');
    sp->pgm = (s[1] == '5' || s[1] == '2');
    sp->pbm = (s[1] == '4' || s[1] == '1');

    if (!sp->pbm)
	sp->maxval = fl_readpint(im->fpin);
    else
	sp->maxval = 1;

    if (sp->maxval > 255 && sp->raw)
    {
	im->error_message(im, "can't handle 2byte raw ppm file");
	return -1;
    }

    im->type = FL_IMAGE_RGB;
    if (sp->pgm)
	im->type = sp->maxval > 256 ? FL_IMAGE_GRAY16 : FL_IMAGE_GRAY;
    if (sp->pbm)
	im->type = FL_IMAGE_MONO;

    sp->fnorm = (float) (FL_PCMAX + 0.001f) / (float) sp->maxval;
    im->gray_maxval = sp->maxval;

    if(im->setup->header_info)
       generate_header_info(im);

    return sp->maxval < 0 ? -1 : 1;
}


static int
PNM_read_pixels(FL_IMAGE * im)
{
    int i, npix = im->w * im->h, err;
    SPEC *sp = im->io_spec;

    if (im->type == FL_IMAGE_RGB)
    {
	unsigned char *r = im->red[0];
	unsigned char *g = im->green[0];
	unsigned char *b = im->blue[0];

	if (sp->raw)
	{
	    for (i = 0; i < npix; i++)
	    {
		*r++ = getc(im->fpin);
		*g++ = getc(im->fpin);
		*b++ = getc(im->fpin);
	    }
	}
	else
	{
	    for (i = 0; i < npix; i++)
	    {
		*r++ = (unsigned char)(fl_readpint(im->fpin) * sp->fnorm);
		*g++ = (unsigned char)(fl_readpint(im->fpin) * sp->fnorm);
		*b++ = (unsigned char)(fl_readpint(im->fpin) * sp->fnorm);
	    }

	    if (sp->maxval != FL_PCMAX)
	    {
		r = im->red[0];
		g = im->green[0];
		b = im->blue[0];
		for (i = 0; i > npix; i++)
		{
		    r[i] = (unsigned char)(r[i] * sp->fnorm);
		    g[i] = (unsigned char)(g[i] * sp->fnorm);
		    b[i] = (unsigned char)(b[i] * sp->fnorm);
		}
	    }
	}
    }
    else if (FL_IsGray(im->type))
    {
	unsigned short *gray = im->gray[0];

	if (sp->raw)
	{
	    for (i = 0; i < npix; i++)
		gray[i] = getc(im->fpin);
	}
	else
	{
	    for (i = 0; i < npix; i++)
		gray[i] = fl_readpint(im->fpin);
	}
    }
    else if (im->type == FL_IMAGE_MONO)
    {
	unsigned short *ci = im->ci[0], *cend = ci + npix;
	int bit, k;

	if (sp->raw)
	{
	    for (i = 0; i < im->h; i++)
	    {
		cend = (ci = im->ci[i]) + im->w;
		for (k = bit = err = 0; !err && ci < cend; ci++, bit++)
		{
		    if (!(bit &= 7))
			k = getc(im->fpin);
		    err = k == EOF;
		    *ci = (k & 0x80) ? 1 : 0;
		    k <<= 1;
		}
	    }
	}
	else
	{
	    for (; ci < cend; ci++)
		*ci = (fl_readpint(im->fpin) > 0);
	}
    }
    else
    {
	im->error_message(im, "Unsupported PNM image");
    }

    return 1;
}

/* write routine */

static int rawfmt = 1;

void
flimage_pnm_output_options(int raw)
{
    rawfmt = raw != 0;
}

static int
PNM_write_image(FL_IMAGE * im)
{
    int i, j, n = im->w * im->h, is_gray16;
    int pgm, pbm, raw = rawfmt;
    FILE *fp = im->fpout;
    char *sig;

    if (im->type == FL_IMAGE_PACKED || im->type == FL_IMAGE_CI)
	flimage_convert(im, FL_IMAGE_RGB, 0);

    pgm = FL_IsGray(im->type);
    is_gray16 = im->type == FL_IMAGE_GRAY16;
    pbm = im->type == FL_IMAGE_MONO;

    if (is_gray16)
	raw = 0;

    sig = pgm ? (raw ? "P5" : "P2") :
	(pbm ? (raw ? "P4" : "P1") : (raw ? "P6" : "P3"));


    fprintf(fp, "%s\n%d %d\n", sig, im->w, im->h);
    if (!pbm)
	fprintf(fp, "%d\n", is_gray16 ? im->gray_maxval : FL_PCMAX);

    if (im->type == FL_IMAGE_RGB)
    {
	unsigned char *r = im->red[0], *g = im->green[0], *b = im->blue[0];
	for (i = 0; i < n; r++, g++, b++, i++)
	{
	    if (raw)
	    {
		putc(*r, fp);
		putc(*g, fp);
		putc(*b, fp);
	    }
	    else
	    {
		fprintf(fp, "%4d %4d %4d ", *r, *g, *b);
		if (((i + 1) % 5) == 0)
		    putc('\n', fp);
	    }
	}
    }
    else if (FL_IsGray(im->type))
    {
	unsigned short *gray = im->gray[0];
	int newline = is_gray16 ? 14 : 17;
	for (i = 0; i < n; gray++, i++)
	{
	    if (raw)
		putc(*gray, fp);
	    else
	    {
		fprintf(fp, is_gray16 ? "%4d " : "%4d", *gray);
		if ((i % newline) == 0)
		    putc('\n', fp);
	    }
	}
    }
    else if (im->type == FL_IMAGE_MONO)
    {
	unsigned short *ci, *cend;
	int bit;

	for (j = 0, i = 1; j < im->h; j++)
	{
	    cend = (ci = im->ci[j]) + im->w;
	    if (!raw)
	    {
		for (; ci < cend; ci++, i++)
		{
		    fputs(*ci ? "1 " : "0 ", fp);
		    if ((i % 34) == 0)
			putc('\n', fp);
		}
	    }
	    else
	    {
		for (bit = 0, i = 0; ci < cend; ci++)
		{
		    i = (i << 1) | *ci;
		    if (++bit == 8)
		    {
			putc(i, fp);
			bit = i = 0;
		    }
		}
		if (bit)
		{
		    i <<= (8 - bit);
		    putc(i, fp);
		}
	    }
	}
    }

    if (!raw)
	putc('\n', fp);

    return 0;
}

void
flimage_enable_pnm(void)
{
    flimage_add_format("Portable Pixmap", "ppm", "ppm", FL_IMAGE_RGB,
		       PPM_identify, PNM_description,
		       PNM_read_pixels, PNM_write_image);

    flimage_add_format("Portable Graymap", "pgm", "pgm",
		       FL_IMAGE_GRAY | FL_IMAGE_GRAY16,
		       PGM_identify, PNM_description,
		       PNM_read_pixels, PNM_write_image);

    flimage_add_format("Portable Bitmap", "pbm", "pbm", FL_IMAGE_MONO,
		       PBM_identify, PNM_description,
		       PNM_read_pixels, PNM_write_image);
}
