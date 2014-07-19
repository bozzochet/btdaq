/*
 *
 * This file is part of XForms.
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
 * $Id: papers.c,v 0.36 1999/07/17 03:32:50 zhao beta $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1997-2000  by T.C. Zhao
 *  All rights reserved.
 *.
 *
 */

#include <stdio.h>
#include <string.h>
#include "fd2ps.h"

#ifdef __EMX__
#define strcasecmp stricmp
#endif

struct paper
{
    const char *name;
    const char *size;
    float w, h;			/* in inches */
};

static struct paper papers[] =
{
    {"Letter", "8.5x11in", 8.5, 11.0},
    {"Legal", "8.5x14in", 8.5, 14.0},
    {"A4", "210x295mm", 7.27, 11.61},	/* European  210x295mm */
    {"B4", "257x364mm", 10.1, 14.33},	/* Janpanese 275x364mm */
    {"B5", "18x20cm", 7.283, 10.630},
    {"B", "11x17in", 11.0, 17.0},	/* US tabloid */
    {"Note", "4x5in", 4.0, 5.0},
 /* sentinel */
    {0}
};

void
get_paper_size(const char *name, float *w, float *h)
{
    struct paper *p = papers;

    for (; p->name; p++)
	if (strcasecmp(name, p->name) == 0)
	{
	    *w = p->w;
	    *h = p->h;
	    return;
	}

    fprintf(stderr, "Unknown paper name %s - ignored\n", name);
}

void
list_papers(const char *prefix)
{
    struct paper *p = papers;

    for (; p->name; p++)
	fprintf(stderr, "%s%s\t(%s)\n", prefix, p->name, p->size);
}
