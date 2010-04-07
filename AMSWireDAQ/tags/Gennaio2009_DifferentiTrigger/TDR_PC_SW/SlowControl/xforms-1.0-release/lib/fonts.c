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
 * $Id: fonts.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 * All font and string size query routines. There are rooms for speed ups.
 * For one, font switching can be reduced somewhat.
 *
 */
#if defined(F_ID) || defined(DEBUG)
char *fl_id_fnt = "$Id: fonts.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include <string.h>
#include <ctype.h>
#include "forms.h"
static XFontStruct *defaultfs;
static XFontStruct *fl_try_get_font_struct(int, int, int);
static char *get_fname(const char *str, int size);

/*
 * question marks indicate the sizes in tenth of a point. It will be
 * replaced on the fly by the font requesting routines. Depending on
 * the availability of the fonts and capabilities of the server, the
 * font may or may not be scalable.
 *
 * Resolution field is purposely left blank as the resolution reported
 * by the server is not reliable thus the program can't force the
 * resolution. This way the admins can set the proper font path.
 *
 * Order is important as it has to agree with FL_TIMES_STYLE etc
 * defined in Basic.h
 *
 */
static const char *fnts[FL_MAXFONTS] =
{
    "-*-helvetica-medium-r-*-*-*-?-*-*-p-*-*-*",
    "-*-helvetica-bold-r-*-*-*-?-*-*-p-*-*-*",
    "-*-helvetica-medium-o-*-*-*-?-*-*-p-*-*-*",
    "-*-helvetica-bold-o-no*-*-*-?-*-*-p-*-*-*",

    "-*-courier-medium-r-*-*-*-?-*-*-m-*-*-*",
    "-*-courier-bold-r-*-*-*-?-*-*-m-*-*-*",
    "-*-courier-medium-o-*-*-*-?-*-*-m-*-*-*",
    "-*-courier-bold-o-*-*-*-?-*-*-m-*-*-*",

    "-*-times-medium-r-*-*-*-?-*-*-p-*-*-*",
    "-*-times-bold-r-*-*-*-?-*-*-p-*-*-*",
    "-*-times-medium-i-*-*-*-?-*-*-p-*-*-*",
    "-*-times-bold-i-*-*-*-?-*-*-p-*-*-*",

 /* bitstream fonts can scale all */
    "-*-charter-medium-r-*-*-*-?-*-*-*-*-*-*",
    "-*-charter-bold-r-*-*-*-?-*-*-*-*-*-*",
    "-*-charter-medium-i-*-*-*-?-*-*-*-*-*-*",
    "-*-symbol-medium-r-*-*-*-?-*-*-*-*-*-*",
};

static FL_FONT fl_fonts[FL_MAXFONTS];
static const char *cv_fname(const char *);

#define DEFAULTF1  "fixed"
#define DEFAULTF2  "6x13"

/*
 * Global initialization routine. Must be called before any font
 * routine can be used. We can place fl_init_font in all font switching
 * and string size query routines so a seperate initialization is not
 * needed, and it has the added bonus that startup *may* be faster,
 * but we pay a function call overhead in every call to any of these
 * font related routines.
 */
void
fl_init_font(void)
{
    FL_FONT *flf = fl_fonts;
    const char *const *f = fnts;
    static int initialized;

    if (initialized)
	return;

    initialized = 1;

    /* if fl_set_font_name is called before fl_initialize, we need to keep
       the change */
    for (; *f; f++, flf++)
	if (!flf->fname[0])
	    strcpy(flf->fname, *f);

    /* load a default font */
    if (!defaultfs && !(defaultfs = XLoadQueryFont(flx->display, DEFAULTF1)))
	defaultfs = XLoadQueryFont(flx->display, DEFAULTF2);


    M_warn("InitFont", "Done");

    /* load a couple of fonts at normal size to prevent the caching code from
       using bad looking replacement if strange sizes are requested */

    fl_get_font_struct(FL_NORMAL_STYLE, FL_DEFAULT_SIZE);
    fl_get_font_struct(FL_BOLD_STYLE, FL_DEFAULT_SIZE);
    fl_get_font_struct(FL_FIXED_STYLE, FL_DEFAULT_SIZE);

}

/*
 * In addition to get the font handle, we also make the font current
 * in default GC
 */
void
fl_set_font(int numb, int size)
{
    int dh;
    XCharStruct overall;
    XFontStruct *fs;

    fs = fl_get_font_struct(numb, size);

    /* cur_font is always the one in current GC */
    if (fl_state[fl_vmode].cur_fnt == fs)
    {
#if (FL_DEBUG >= ML_DEBUG)
	M_debug("SetFont", "current", fl_curfnt);
#endif
	return;
    }

    fl_state[fl_vmode].cur_fnt = flx->fs = fs;

#if 1
    /* basic font info */
    XTextExtents(flx->fs, "gbjQ", 4, &dh, &flx->fasc, &flx->fdesc, &overall);
#else
    /* this is theorectically correct, but looks bad */
    fl->fdesc = flx->fs->max_bounds.descent;
    fl->fasc = flx->fs->max_bounds.ascent;
#endif
    XSetFont(flx->display, flx->textgc, flx->fs->fid);
    flx->fheight = flx->fasc + flx->fdesc;

    if (fl_cntl.debug > 1)
    {
	unsigned long res = 0;
	if (XGetFontProperty(flx->fs, XA_RESOLUTION, &res))
	    M_info2("", "FontResolution: %lu", res);

    }
}

/*
 * Add a new font (indexed by n) or change an existing font.
 * preferably the font name constains a ? in the size position
 * so different sizes can be used.
 */
int
fl_set_font_name(int n, const char *name)
{
    FL_FONT *flf = fl_fonts + n;
    int i;

    if (n < 0 || n >= FL_MAXFONTS || !name)
    {
	M_warn("SetFont", "bad number(%d) or fontname", n);
	return -1;
    }

    if (flf->fname[0])
    {
	for (i = 0; i < flf->nsize; i++)
	    XFreeFont(flx->display, flf->fs[i]);
	flf->fname[0] = '\0';
    }

    flf->nsize = 0;
    strcpy(flf->fname, name);
    if (!flx->display)
	return 1;
    return (fl_try_get_font_struct(n, FL_DEFAULT_SIZE, 1) ? 0 : -1);
}


/*  list built-in fonts */
int
fl_enumerate_fonts(void (*output) (const char *s), int shortform)
{
    FL_FONT *flf = fl_fonts, *fe = flf + FL_MAXFONTS;
    int n = 0;

    for (; output && flf < fe; flf++)
	if (flf->fname[0])
	{
	    output(shortform ? cv_fname(flf->fname) : flf->fname);
	    n++;
	}
    return n;
}


/*
 * All font changes go through this routine. If with_fail is false,
 * this routine will not fail even if requested font can't be loaded.
 * A substitution will be made.
 */
static XFontStruct *
fl_try_get_font_struct(int numb, int size, int with_fail)
{
    FL_FONT *flf = fl_fonts;
    XFontStruct *fs = 0;
    int n = 0, i;

    if (special_style(numb))
	numb %= FL_SHADOW_STYLE;

    flf = fl_fonts + numb;

    if (numb < 0 || numb >= FL_MAXFONTS || !flf->fname[0])
    {
	/* for fdesign: no connecton has fname empty and is legit */
	if (!fl_no_connection)
	    M_err("SetFont", "Bad FontStyle request %d: %s", numb, flf->fname);
	if (!fl_state[fl_vmode].cur_fnt)
	    M_warn("FontStruct", "bad font returned");
	return fl_state[fl_vmode].cur_fnt;
    }

    strcpy(fl_curfnt, get_fname(flf->fname, size));

    /* search for requested size */
    for (fs = 0, i = 0; i < flf->nsize; i++)
    {
	if (size == flf->size[i])
	{
	    fs = flf->fs[i];
#if (FL_DEBUG >= ML_DEBUG)
	    M_debug("SetFont", "Cache hit: %s", fl_cur_fontname);
#endif
	}
    }

    /* if requested not found or cache full, get the destination cache for
       this size */

    if (!fs && flf->nsize == FL_MAX_FONTSIZES)
    {
	XFreeFont(flx->display, flf->fs[FL_MAX_FONTSIZES - 1]);
	flf->nsize--;
    }


    /* font is not cached, try to load the font */
    if (!fs)
    {
	n = flf->nsize;
	flf->fs[n] = XLoadQueryFont(flx->display, fl_curfnt);

	if ((fs = flf->fs[n]))
	{
	    flf->size[n] = size;
	    flf->nsize++;
	}
    }

    if (!fs && with_fail)
	return 0;

    /* didn't get it. Try to find a substitute */

    if (!fs)
    {
	int mdiff = 1000, k = -1;

	M_warn("SetFont", "can't load %s", fl_curfnt);

	/* search for a replacement */
	for (i = 0; i < flf->nsize; i++)
	{
	    if (mdiff > FL_abs(size - flf->size[i]))
	    {
		mdiff = FL_abs(size - flf->size[i]);
		k = i;
	    }
	}

	fs = (k == -1) ? (flx->fs ? flx->fs : defaultfs) : flf->fs[k];

	/* if we did not get it this time, we won't get it next time either,
	   so replace it with whatever we found  */

	flf->size[flf->nsize] = size;
	flf->fs[flf->nsize] = fs;
	flf->nsize++;
    }

    /* here we are guranteed a valid font handle although there is no
       gurantee the font handle corresponds to the font requested */

    return fs;
}

XFontStruct *
fl_get_font_struct(int style, int size)
{
    return fl_try_get_font_struct(style, size, 0);
}


#if 0
/* Similar to enumerate_font, but also prints out the sizes cached
 * primarily for debugging
 */
void
fl_enumerate_loaded_font(void)
{
    FL_FONT *flf = fl_fonts, *fe = flf + FL_MAXFONTS;
    int i;

    for (; flf < fe; flf++)
    {
	if (flf->fname[0])
	    printf("\n%s: ", cv_fname(flf->fname));
	for (i = 0; flf->fname[0] && i < flf->nsize; i++)
	    printf("%d ", flf->size[i]);
    }
    printf("\n");
}

#endif


/*
 * Similar to fl_get_string_xxxGC except that there is no side effects.
 * Must not free the fontstruct as structure FL_FONT cache the
 * structure for possible future use.
 */
int
fl_get_string_width(int style, int size, const char *s, int len)
{
    XFontStruct *fs = fl_get_font_struct(style, size);
    return fl_no_connection ? (len * size) : XTextWidth(fs, (char *) s, len);
}

int
fl_get_string_widthTABfs(XFontStruct * fs, const char *s, int len)
{
    int w, tmpw, tab;
    const char *p, *q;

    if (fl_no_connection)
	return 12 * len;

    tab = fl_get_tabpixels(fs);

    for (w = 0, q = s; *q && (p = strchr(q, '\t')) && (p - s) < len; q = p + 1)
    {
	tmpw = XTextWidth(fs, q, p - q);
	w += tmpw;
	w = ((w / tab) + 1) * tab;
    }

    tmpw = XTextWidth(fs, q, len - (q - s));
    w += tmpw;
    return w;
}

int
fl_get_string_widthTAB(int style, int size, const char *s, int len)
{
    XFontStruct *fs = fl_get_font_struct(style, size);
    return fl_get_string_widthTABfs(fs, s, len);
}



int
fl_get_string_height(int style, int size, const char *s, int len,
		     int *asc, int *desc)
{
    XFontStruct *fs = fl_get_font_struct(style, size);
    XCharStruct overall;
    int dh, a, d;

    if (fl_no_connection)
	a = d = size / 2;
    else
	XTextExtents(fs, (char *) s, len, &dh, &a, &d, &overall);

    if (asc)
	*asc = a;
    if (desc)
	*desc = d;
    /* in case asc == desc */
    return (a + d);
}

int
fl_get_char_height(int style, int size, int *asc, int *desc)
{
    return fl_get_string_height(style, size, "gbjQ", 4, asc, desc);
}

int
fl_get_char_width(int style, int size)
{
    XFontStruct *fs = fl_get_font_struct(style, size);
    return fs->max_bounds.width;
}


void
fl_get_string_dimension(int fntstyle, int fntsize, const char *s, int len,
			int *width, int *height)
{
    const char *p, *q;
    int w, h, maxw = 0, maxh = 0;

    h = fl_get_string_height(fntstyle, fntsize, "qjb", 3, 0, 0);

    for (q = s; *q && (p = strchr(q, '\n')); q = p + 1)
    {
	w = fl_get_string_width(fntstyle, fntsize, q, p - q);
	if (maxw < w)
	    maxw = w;
	maxh += h;
    }

    w = fl_get_string_width(fntstyle, fntsize, q, len - (q - s));
    if (maxw < w)
	maxw = w;
    maxh += h;

    *width = maxw;
    *height = maxh;
}

/*
 * Tab handling. Currently only one tab
 */

#define  MaxTabs   5
static char *tabstop[MaxTabs] = { "aaaaaaaa", 0};
static int tabstopNchar[MaxTabs] = {7};

void
fl_set_tabstop(const char *s)
{
    static int set;

    if (s)
    {
	if (set)
	    fl_free(tabstop[0]);
	tabstop[0] = fl_strdup(s);
	tabstopNchar[0] = strlen(tabstop[0]);
	set = 1;
    }
}

#if 0
int 
fl_set_tabstops(int n, const char *s)
{
    if (n < 0 || n >= MaxTabs || !s)
        return -1;

    if (tabstop[n] != 0)
         fl_free(tabstop[n]);
    
    tabstop[n] = fl_strdup(s);
    tabstopNchar[n] = strlen(tabstop[n]);
    return 0;
}
#endif

int
fl_get_tabpixels(XFontStruct * fs)
{
    return XTextWidth(fs, tabstop[0], tabstopNchar[0]) + XTextWidth(fs, " ", 1);
}


/*
 * Convert X font names to more conventional names by striping the
 * auxilary info.
 */
#define is_garb(c)       (!isalpha( (int) c) && !isdigit( ( int ) c))
static const char *
cv_fname(const char *f)
{
    static char fname[127];
    char *q, *p;

    /* remove all the garbages from head */
    for (q = strcpy(fname, f); *q && is_garb(*q); q++)
	;

    /* remove all the garbage from the end, starting from ? */
    if ((p = strchr(fname, '?')))
	*--p = '\0';

    /* remove all remaining garbages */
    for (p = fname + strlen(fname) - 1; p > q && is_garb(*p); p--)
	;
    *++p = '\0';
    return q;
}

/* Given a fontname and a size (in point), get the complete name */
static char *
get_fname(const char *str, int size)
{
    static char fname[80];
    char *p, sz[15], tail[80];

    strcpy(fname, str);

    if ((p = strchr(fname, '?')))
    {
	sprintf(sz, "%d", size * 10);
	strcpy(tail, p + 1);
	*p = '\0';
	strcat(strcat(fname, sz), tail);
    }

    return fname;
}

/**** Some compatibility stuff */

int fl_fdesc_(void)
{
    return flx->fdesc;
}

int fl_fheight_(void)
{
   return flx->fheight;
}

GC fl_gc_(void)
{
   return flx->gc;
}

GC fl_textgc_(void)
{
   return flx->textgc;
}


Window fl_cur_win_(void)
{
   return flx->win;
}

XFontStruct* fl_cur_fs_(void)
{
   return flx->fs;
}

Display* fl_display_(void)
{
     return flx->display;
}


