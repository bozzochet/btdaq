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
 *  $Id: rgb_db.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *  Copyright (c) 1999-2002 T.C. Zhao
 *
 *  search the rgb.txt database for a specific color
 */

#include "forms.h"
#include "flimage.h"
#include "flimage_int.h"

#include <ctype.h>

#define DEBUG 0

static char *rgbfile[] =
{
    "/usr/lib/X11/rgb.txt",	/* typical */
    "/usr/X11R6/lib/X11/rgb.txt", /* also typical */
    "/usr/local/lib/X11/rgb.txt",	/* try     */
#if defined(sun) || defined(__sun__)
    "/usr/openwin/lib/rgb.txt",	/* Sun     */
#endif
#ifdef __EMX__
    "/XFree86/lib/X11/rgb.txt",	/* OS2     */
#endif
#ifdef __VMS
    "SYS$MANAGER:DECW$RGB.DAT",	/* vax */
#endif
    0
};

typedef struct
{
    char name[32];
    unsigned short r, g, b;
}
RGBDB;

static RGBDB *rgb_db;
static int db_size, nentries;	/* total size and filled entries */
static int read_entry(FILE * fp, int *r, int *g, int *b, char name[]);
static char hexv[256];

int
fl_init_RGBdatabase(const char *f)
{
    FILE *fp = 0;
    char name[128], *lname;
    char *const *file = rgbfile;
    int r, g, b, lr, lg, lb, size = 700;
    RGBDB *db, *dbs;

    if (rgb_db)
	return 1;

    if (f)
	fp = fopen(f, "r");

    /* search default location if can't open input file */
    for (; !fp && *file; file++)
	fp = fopen(*file, "r");

    if (!fp)
    {
	M_err("InitColorLookup", "can't find the rgb color database");
	return -1;
    }

    /* now get the database */
    if (!(rgb_db = fl_malloc(sizeof(*rgb_db) * size)))
    {
	M_err("InitColorLookup", "Can't get memory");
	return -1;
    }


    for (r = 0; r < 10; r++)
	hexv[r + '0'] = r;

    for (r = 10; r <= 15; r++)
    {
	hexv[r - 10 + 'a'] = r;
	hexv[r - 10 + 'A'] = r;
    }


    /* now read it */
    db = rgb_db;
    dbs = rgb_db + size;
    lname = "";
    lr = lg = lb = -1;
    for (; read_entry(fp, &r, &g, &b, name) && db < dbs;)
    {
	db->r = r;
	db->g = g;
	db->b = b;

	/* unique the entry on the fly */
	if (r != lr || g != lg || b != lb || strcasecmp(name, lname))
	{
	    lname = strcpy(db->name, name);
	    nentries++;
#if DEBUG
	    fprintf(stderr,"(%3d %3d %3d) %s\n", db->r, db->g, db->b, db->name);
#endif
	    if (db == dbs - 1)
	    {
		size += size / 2;
		rgb_db = fl_realloc(rgb_db, sizeof(*rgb_db) * size);
		dbs = rgb_db + size;
	    }
	    db++;
	    lr = r;
	    lg = g;
	    lb = b;
	}
    }

    fclose(fp);
    db_size = size;

#if DEBUG
    fprintf(stderr, " TotalEntries: %d of %d filled\n", nentries, db_size);
#endif

    return (nentries > 100) ? 1 : -1;
}

int
fl_lookup_RGBcolor(const char *req_name, int *r, int *g, int *b)
{
    int match, i;
    RGBDB *db;
    char name[128], *p;

    if (!req_name || !*req_name)
    {
	*r = *g = *b = 0;
	return -1;
    }

    p = (char *) req_name;

    /* squeeze out all spaces */
    for (i = 0; *p && i < 127; p++)
    {
	if (!isspace( ( int ) *p))
	    name[i++] = *p;
    }
    name[i] = '\0';

    if (!rgb_db)
	fl_init_RGBdatabase(0);

    if (!rgb_db)
    {
	M_err("LookupColor", "database uninitialized");
	*r = *g = *b = 0;
	return -1;
    }

    /* search for special cases: numerical and None */
    if (strcasecmp(name, "None") == 0)
    {
	*r = *g = *b = -1;
    }
    else if (*name == '#')
    {
	unsigned char *q = (unsigned char *) name;
	if ((match = strlen(name)) == 7)	/* #rrggbb */
	{
	    *r = (hexv[q[1]] << 4) | hexv[q[2]];
	    *g = (hexv[q[3]] << 4) | hexv[q[4]];
	    *b = (hexv[q[5]] << 4) | hexv[q[6]];
	}
	else if (match == 13)	/* #rrrrggggbbbb */
	{
	    *r = (hexv[q[1]] << 12) | (hexv[q[2]] << 8) |
		(hexv[q[3]] << 4) | hexv[q[4]];
	    *g = (hexv[q[5]] << 12) | (hexv[q[6]] << 8) |
		(hexv[q[7]] << 4) | hexv[q[8]];
	    *b = (hexv[q[9]] << 12) | (hexv[q[10]] << 8) |
		(hexv[q[11]] << 4) | hexv[q[12]];

	    *r = (*r * 255L) / 65535L;
	    *g = (*g * 255L) / 65535L;
	    *b = (*b * 255L) / 65535L;
	}
	else if (match == 10)	/* #rrrgggbbb */
	{
	    *r = (hexv[q[1]] << 8) | (hexv[q[2]] << 4) | (hexv[q[3]]);
	    *g = (hexv[q[4]] << 8) | (hexv[q[5]] << 4) | (hexv[q[6]]);
	    *b = (hexv[q[7]] << 8) | (hexv[q[8]] << 4) | (hexv[q[9]]);

	    *r = (*r * 255L) / 4095;
	    *g = (*g * 255L) / 4095;
	    *b = (*b * 255L) / 4095;
	}
	else if (match == 4)
	{
	    /* this is not conformant to the spec but probably better */
	    *r = (hexv[q[1]] * 255) / 15;
	    *g = (hexv[q[2]] * 255) / 15;
	    *b = (hexv[q[3]] * 255) / 15;
	}
	else if (match == 3)
	{
	    *r = *g = *b = 0;
	}
	else
	{
	    M_err("LookupColor", "can't handle color %s", name);
	    return -1;
	}
    }
    else if (strstr(name, "rgb:"))
    {
	unsigned char *q = (unsigned char *) strchr(name, ':');
	if ((match = strlen((char *)q)) == 6)
	{
	    /* :r/g/b */
	    *r = (hexv[q[1]] * 255) / 15;
	    *g = (hexv[q[3]] * 255) / 15;
	    *b = (hexv[q[5]] * 255) / 15;
	}
	else if (match == 9)
	{
	    /* :rr/gg/bb */
	    *r = (hexv[q[1]] << 4) | hexv[q[2]];
	    *g = (hexv[q[4]] << 4) | hexv[q[5]];
	    *b = (hexv[q[7]] << 4) | hexv[q[8]];
	}
	else if (match == 12)
	{
	    /* :rrr/ggg/bbb */
	    *r = (hexv[q[1]] << 8) | (hexv[q[2]] << 4) | (hexv[q[3]]);
	    *g = (hexv[q[5]] << 8) | (hexv[q[6]] << 4) | (hexv[q[7]]);
	    *b = (hexv[q[9]] << 8) | (hexv[q[10]] << 4) | (hexv[q[11]]);

	    *r = (*r * 255L) / 4095;
	    *g = (*g * 255L) / 4095;
	    *b = (*b * 255L) / 4095;

	}
	else if (match == 15)
	{
	    *r = (hexv[q[1]] << 12) | (hexv[q[2]] << 8) |
		(hexv[q[3]] << 4) | hexv[q[4]];
	    *g = (hexv[q[6]] << 12) | (hexv[q[7]] << 8) |
		(hexv[q[8]] << 4) | hexv[q[9]];
	    *b = (hexv[q[11]] << 12) | (hexv[q[12]] << 8) |
		(hexv[q[13]] << 4) | hexv[q[14]];

	    *r = (*r * 255L) / 65535L;
	    *g = (*g * 255L) / 65535L;
	    *b = (*b * 255L) / 65535L;
	}
	else
	{
	    M_err("LookupColor", "can't handle color %s\n", name);
	    return -1;
	}
    }
    else
    {
	/* search the pre-defined colorname database */
	for (match = 0, db = rgb_db; !match && db < rgb_db + db_size; db++)
	{
	    if (strcasecmp(name, db->name) == 0)
	    {
		match = 1;
		*r = db->r;
		*g = db->g;
		*b = db->b;
	    }
	}

	if (!match)
	{
	    M_warn("LookupColor", "colorname (%s) not found\n", name);
	    return -1;
	}
    }
    return 0;
}


static int
read_entry(FILE * fp, int *r, int *g, int *b, char name[])
{
    int n;
    char buf[256], *p;

    if (!fgets(buf, sizeof(buf), fp))
	return 0;

    if (buf[0] == '!')
	fgets(buf, sizeof(buf), fp);

    if (sscanf(buf, " %d %d %d %n", r, g, b, &n) < 3)
	return 0;

    p = buf + n;

    /* squeeze out all spaces */
    while (*p)
    {
	if (*p != ' ' && *p != '\n')
	    *name++ = *p;
	p++;
    }
    *name = 0;

    return (feof(fp) || ferror(fp)) ? 0 : 1;
}
