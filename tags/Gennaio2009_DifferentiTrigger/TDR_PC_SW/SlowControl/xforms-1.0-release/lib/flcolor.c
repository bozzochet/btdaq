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
 * $Id: flcolor.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 * Colormap management.
 *
 *  XForm will by default use the visual that has the most depth
 *  and share colormap if possible. The default behavior can be
 *  overridden by resource class Visual and Depth.
 *
 *  Pixel is addressed via two indirections. First all colors
 *  are addressed using the FL defined symbolic names, FL_RED, etc
 *  which are then translated via a secondary lookup table to
 *  get the true pixel values as known by the server.
 *
 */

#if defined(F_ID) || defined(DEBUG)
char *fl_id_col = "$Id: flcolor.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include "forms.h"

/******************* Local variables ************************/

static unsigned long *lut;
static long max_server_cols;	/* max cols in current visual       */
static long cols_in_default_visual;
static long predefined_cols;	/* min(max_server_col, built_in_col) */
static FL_COLOR rgb2pixel(unsigned, unsigned, unsigned);
static int allow_leakage;
static FL_COLOR lastmapped;	/* so fl_textcolor can refresh its cache */

/*
   typedef struct
   {
   const char *name;
   FL_COLOR long index;
   unsigned short r, g, b, a;
   int grayval;
   } FL_IMAP;
 */

/* this needs to be changed to a lookup table */
#ifndef FL_RGB2GRAY
#define FL_RGB2GRAY(r,g,b)  ((78*(r)+150*(g)+28*(b))>>8)
#endif
#define NV(a)    #a,a

/*
 * Default colormap entry for FORMS, subject to gamma corrections.
 *
 * The map entries are listed in the order of importance based
 * on which color we need most in case the Xserver does not have
 * enough depths to get them all.
 */

static FL_IMAP fl_imap[FL_MAX_COLS] =
{
    {NV(FL_BLACK), 0, 0, 0},
    {NV(FL_WHITE), 255, 255, 255},

    {NV(FL_COL1), 161, 161, 161},	/* default color, gray63 */
    {NV(FL_BOTTOM_BCOL), 89, 89, 89},	/* bottom  gray35 */
    {NV(FL_RIGHT_BCOL), 41, 41, 41},	/* right  gray16  */
    {NV(FL_MCOL), 191, 191, 191},	/* used as magic, gray75 */
    {NV(FL_LEFT_BCOL), 222, 222, 222},	/* left color  gray87 */

    {NV(FL_LIGHTER_COL1), 172, 172, 172},	/* left color  gray87 */
    {NV(FL_DARKER_COL1), 150, 150, 150},	/* left color  gray87 */

    {NV(FL_SLATEBLUE), 113, 113, 198},
    {NV(FL_INDIANRED), 198, 113, 113},

    {NV(FL_RED), 255, 0, 0},
    {NV(FL_BLUE), 0, 0, 255},
    {NV(FL_GREEN), 0, 255, 0},
    {NV(FL_YELLOW), 255, 255, 0},
    {NV(FL_MAGENTA), 255, 0, 255},
    {NV(FL_CYAN), 0, 255, 255},
    {NV(FL_TOMATO), 255, 99, 71},

    {NV(FL_INACTIVE), 110, 110, 110},	/* gray43       */
    {NV(FL_TOP_BCOL), 204, 204, 204},	/* top  gray80  */

    {NV(FL_PALEGREEN), 113, 198, 113},
    {NV(FL_DARKGOLD), 205, 149, 10},
    {NV(FL_ORCHID), 205, 105, 201},
    {NV(FL_DARKCYAN), 40, 170, 175},
    {NV(FL_DARKTOMATO), 139, 54, 38},
    {NV(FL_WHEAT), 255, 231, 155},
    {NV(FL_DARKORANGE), 255, 128, 0},
    {NV(FL_DEEPPINK), 255, 0, 128},
    {NV(FL_CHARTREUSE), 128, 255, 0},
    {NV(FL_DARKVIOLET), 128, 0, 255},
    {NV(FL_SPRINGGREEN), 0, 255, 128},
    {NV(FL_DODGERBLUE), 0, 128, 255},
    {NV(FL_DOGERBLUE), 0, 128, 255},

    {NV(FL_FREE_COL1), 0, 0, 0},
    {NV(FL_FREE_COL2), 0, 0, 0},
    {NV(FL_FREE_COL3), 0, 0, 0},
    {NV(FL_FREE_COL4), 0, 0, 0},
    {NV(FL_FREE_COL5), 0, 0, 0},
    {NV(FL_FREE_COL6), 0, 0, 0},
};

#define flmapsize (int)(sizeof(fl_imap)/sizeof(fl_imap[0]))
#define builtin   FL_BUILT_IN_COLS


const char *
fl_query_colorname(FL_COLOR col)
{
    FL_IMAP *flmap = fl_imap, *flmape = flmap + builtin + 2;
    static char buf[32];

    for (; flmap < flmape; flmap++)
    {
	if (col == flmap->index)
	    return flmap->name;
    }

    /* not a built-in */
    if (col == FL_NoColor)
	return "FL_NoColor";

    if(col >= FL_FREE_COL1 && col <= FL_FREE_COL16)
       sprintf(buf,"FL_FREE_COL%ld",1+col-FL_FREE_COL1);
    else
       sprintf(buf, "%ld", col);

    return buf;
}

long
fl_query_namedcolor(const char *s)
{
    FL_IMAP *flmap = fl_imap, *flmape = flmap + builtin;

    for (; s && flmap < flmape; flmap++)
	if (strcmp(s, flmap->name) == 0)
	    return flmap->index;

    if(strstr(s,"FL_FREE_COL"))
    {
        return FL_FREE_COL1 + atoi(s+11) - 1;
    }

    if (strcmp("FL_NoColor", s) == 0)
	return FL_NoColor;

    /* a wild shot */
    return atoi(s);
}

#ifdef DO_GAMMA_CORRECTION	/* { */
#include <math.h>
void
fl_set_gamma(double r, double g, double b)
{
    FL_IMAP *fm = fl_imap, *fs;
    static float rgamma = 1.0f, ggamma = 1.0f, bgamma = 1.0f;

    if (fm[4].grayval)
    {
	/* too lazy to shuffle colormap around */
	M_err("Gamma", "Ignored. Please call set_gamma before fl_init");
	return;
    }

    if (r <= 1.e-3 || g <= 1.e-3 || b <= 1.e-3)
    {
	M_warn("Gamma", "BadValue %4.2f %4.2f %4.2f. Ignored", r, g, b);
	return;
    }

    for (fs = fm + builtin; fm < fs; fm++)
    {
	fm->r = (unsigned short) (0.5f + 255 * pow((double) fm->r / 255.0, rgamma / (double) r));
	fm->g = (unsigned short) (0.5f + 255 * pow((double) fm->g / 255.0, ggamma / (double) g));
	fm->b = (unsigned short) (0.5f + 255 * pow((double) fm->b / 255.0, bgamma / (double) b));
    }

    rgamma = (float)r;
    ggamma = (float)g;
    bgamma = (float)b;
}

#endif /* gamma_correction *} */

static XColor *defaultc;

/*
 * Copy the first several entries in the default colormap to avoid
 * flashing in case we are using a private colormap or non-default
 * visual (can't do anyting about TrueColor/DirectColor though)
 */
#define DEFAULT_SAVE   35
static int save_index[] =
{2, 3, 4, 5, 6, 7, 34};
#define NSAVE (sizeof(save_index)/sizeof(save_index[0]))

static void
be_nice(void)
{
    int i, save = FL_min((cols_in_default_visual - 210), DEFAULT_SAVE);
    XColor *dc;
    unsigned long newpixels[DEFAULT_SAVE], frees[DEFAULT_SAVE];
    int npixels, found, j, k, saved;
    FL_COLOR black = BlackPixel(flx->display, fl_screen);
    FL_COLOR white = WhitePixel(flx->display, fl_screen);

    for (saved = 0, dc = defaultc, i = 0; i < save; i++, dc++)
    {
	dc->flags = DoRed | DoGreen | DoBlue;
	if (XAllocColor(flx->display, fl_colormap(fl_vmode), dc))
	    newpixels[saved++] = dc->pixel;
    }

    /* only keep the pixels in the save_index. 0 and 1 are always saved */

    for (npixels = 0, i = 2; fl_depth(fl_vmode) > 4 && i < saved; i++)
    {
	k = newpixels[i];
	for (j = found = 0; !found && j < NSAVE; j++)
	    found = ((k == save_index[j]) || k == white || k == black);

	if (!found)
	    frees[npixels++] = k;
    }

    if (npixels)
	XFreeColors(flx->display, fl_colormap(fl_vmode), frees, npixels, 0);

    if (save <= 0)
    {
	XColor xc;

	M_warn("BeNice", "Black=%ld White=%ld", black, white);

	/* as a minimum, we should preserve black & white */
	xc.flags = DoRed | DoGreen | DoBlue;
	if (black == 0)
	{
	    xc.red = xc.green = xc.blue = 0;
	    xc.pixel = 0;
	    XAllocColor(flx->display, fl_colormap(fl_vmode), &xc);
	    M_warn(0, "   Get Black=%ld", xc.pixel);
	    if (white == 1)
	    {
		xc.pixel = white;
		xc.red = xc.green = xc.blue = 0xffff;
		XAllocColor(flx->display, fl_colormap(fl_vmode), &xc);
		M_warn(0, "   Get White=%ld", xc.pixel);
	    }
	}
	else if (white == 0)
	{
	    xc.red = xc.green = xc.blue = 0xffff;
	    XAllocColor(flx->display, fl_colormap(fl_vmode), &xc);
	    M_warn(0, "   Get White=%ld", xc.pixel);
	    if (black == 1)
	    {
		xc.red = xc.green = xc.blue = black;
		xc.pixel = 0;
		XAllocColor(flx->display, fl_colormap(fl_vmode), &xc);
		M_warn(0, "   Get Black=%ld", xc.pixel);
	    }
	}
    }

    M_warn("BeNice", "Total %d colors copied", save > 0 ? save : 2);
}

static int
alloc_direct_color(void)
{
    XColor xxc[builtin], *xc;
    FL_IMAP *fm;
    long pred = predefined_cols;

    for (xc = xxc, fm = fl_imap; fm < fl_imap + pred; fm++, xc++)
    {
	xc->red = (fm->r << 8) | 0xff;
	xc->green = (fm->g << 8) | 0xff;
	xc->blue = (fm->b << 8) | 0xff;
	xc->flags = (DoRed | DoGreen | DoBlue);
	xc->pixel = lut[fm->index] = rgb2pixel(fm->r, fm->g, fm->b);
    }

    XStoreColors(flx->display, fl_map(fl_vmode), xxc, pred);

    /* might want to do rest of the colors ( max_server < col < builtin) */
    return 1;
}

/* Do colormap allocation. DirectColor is handled seperately */
static int
fill_map(void)
{
    XColor xc;
    int ok;
    long pred = predefined_cols;
    unsigned r, g, b;
    FL_IMAP *fm, *fs;

    lut = fl_state[fl_vmode].lut;
    fl_dithered(fl_vmode) = (fl_depth(fl_vmode) <= 2);

    M_warn("FillMap", "Trying to get %d colors", pred);
    xc.flags = DoRed | DoGreen | DoBlue;
    for (ok = 1, fm = fl_imap, fs = fm + pred; ok && fm < fs; fm++)
    {
	/* if Xserver is grayscale, we are really walking the "gray" area as
	   the server can drive the the display with any of the r, g or b,
	   need to force a correct value */

	r = FL_is_gray(fl_vmode) ? fm->grayval : fm->r;
	g = FL_is_gray(fl_vmode) ? fm->grayval : fm->g;
	b = FL_is_gray(fl_vmode) ? fm->grayval : fm->b;

	xc.red = (r << 8) | 0xff;
	xc.green = (g << 8) | 0xff;
	xc.blue = (b << 8) | 0xff;
	ok = XAllocColor(flx->display, fl_map(fl_vmode), &xc);
	if (ok)
	{
	    lut[fm->index] = xc.pixel;
	    M_warn(0, "   got %5ld (%3d %3d %3d)", xc.pixel, r, g, b);
	}
    }

    if (fl_state[fl_vmode].pcm || fl_cntl.sharedColormap || fl_dithered(fl_vmode))
    {
	if (!ok && fm > fl_imap)
	    fm--;
	ok = 1;
    }

    /* get approx. for the rest of needed colors */
    for (fs = fl_imap + builtin; ok && fm < fs; fm++)
	fl_mapcolor(fm->index, fm->r, fm->g, fm->b);

    /* make rest of the entry invalid so we don't accidently free a valid
       pixel */
    memset((lut + builtin), 1, (FL_MAX_COLS - builtin) * sizeof(*lut));

    return ok;
}

/*
 * Try to get a private colormap. Return 1 for success. Typically
 * this is the last color strategy we will try and in case it
 * fails, caller probably should terminate the program.
 */

static int
get_private_cmap(int vmode)
{
    int ok, i;

    M_warn("PriavatMap", "getting private colormap");

    /* get a private colormap */
    fl_map(vmode) = XCreateColormap(flx->display, fl_root,
				    fl_visual(vmode),
				    vmode != FL_DirectColor ?
				    AllocNone : AllocAll);
    if (!fl_map(vmode))
    {
	M_err("PrivateMap", "Can't create Colormap!");
	exit(0);
    }

    lut = fl_state[vmode].lut;
    if (vmode == FL_DirectColor)
	return alloc_direct_color();

    /* copy some default entries */
    be_nice();

    /* fill colormap with predetermined colors */
    fl_state[vmode].pcm = 1;
    ok = fill_map();

    for (i = builtin; i < FL_MAX_COLS; i++)
	lut[i] = i;

    M_warn("PrivateMap", "%s %s succesful",
	   fl_vclass_name(vmode), ok ? "" : "not");
    return ok;
}

/*
 * use standard colormap only if user explicitly requests it as it
 * is the most un-flexible among shared,private and standard.
 */
static int
get_standard_cmap(int vmode)
{
    FL_IMAP *fm, *fs;
    XStandardColormap stdcmap;
    XStandardColormap *sc = &stdcmap;
    XColor xc;
    Atom mapid = (vmode == FL_GrayScale || vmode == FL_StaticGray) ?
    XA_RGB_GRAY_MAP : XA_RGB_DEFAULT_MAP;

#if (FL_DEBUG >= ML_ERR)
    M_warn("Stdcmap", "Getting standard colormap");
#endif

    if (!XGetStandardColormap(flx->display, fl_root, &stdcmap, mapid))
    {
	M_err("Stdcmap", "Can't get standard map");
	return 0;
    }

    lut = fl_state[vmode].lut;

    /* we got the map. now figure out the pixel values */
    fl_map(vmode) = sc->colormap;

    xc.flags = DoRed | DoGreen | DoBlue;
    for (fm = fl_imap, fs = fm + builtin; fm < fs; fm++)
    {
	xc.red = (fm->r << 8) | 0xff;
	xc.green = (fm->g << 8) | 0xff;
	xc.blue = (fm->b << 8) | 0xff;
	XAllocColor(flx->display, fl_colormap(vmode), &xc);
	lut[fm->index] = xc.pixel;
    }

    fl_state[vmode].pcm = 1;
    return 1;
}

static int
get_shared_cmap(int vmode)
{
    int ok;

    /* Share colormap only if requested visual is the same as the default
       visual */
    if (fl_visual(vmode) == DefaultVisual(flx->display, fl_screen))
    {
	fl_map(vmode) = DefaultColormap(flx->display, fl_screen);
	M_warn("ShareCmap", "Using default map %ld for %s",
	       fl_map(vmode), fl_vclass_name(vmode));
    }
    else
    {
	fl_map(vmode) = XCreateColormap(flx->display, fl_root,
					fl_visual(vmode),
					vmode != FL_DirectColor ?
					AllocNone : AllocAll);
	M_warn("ShareMap", " NewMap %ld(0x%lx) for %s(ID=0x%lx)",
	       fl_map(vmode), fl_map(vmode),
	       fl_vclass_name(vmode),
	       fl_visual(vmode)->visualid);
    }

    if (!fl_map(vmode))
    {
	M_err("ShareMap", "Error getting colormaps");
	exit(1);
    }

#define PD(v) \
     if((DefaultVisual(flx->display, fl_screen))->class==v) \
       fprintf(stderr,"DefaultVisual=%s CurrentVisual=%s\n",\
       #v,fl_vclass_name(fl_class(vmode)));

    if (fl_cntl.debug)
    {
	PD(TrueColor);
	PD(PseudoColor);
	PD(DirectColor);
	PD(GrayScale);
	PD(StaticGray);
	PD(StaticColor);
    }

    lut = fl_state[vmode].lut;

    if (vmode == FL_DirectColor)
	return alloc_direct_color();

    /* copy a few entries from the default colormap if we are using a map
       other than the defaulf */

    if (fl_visual(vmode) != DefaultVisual(flx->display, fl_screen))
	be_nice();

    ok = fill_map();

    /* if we can't do it, free the color we have already allocated so other
       applications may have an easier time getting colors */

    if (!ok)
    {
	M_warn("ShareCmap", "can't share for %s", fl_vclass_name(vmode));
	fl_map(vmode) = XCopyColormapAndFree(flx->display,
					     fl_map(vmode));
    }
    return ok;
}


/*
 * Create GCs for a particular visual and depth
 */
void
fl_create_gc(Window win)
{
    GC *flgcs, *flgce;
    FL_State *fs = fl_state + fl_vmode;

    /* if gc for this visual exists, do switch */
    if (fl_state[fl_vmode].gc[0])
    {
	flx->gc = fl_state[fl_vmode].gc[0];
	flx->textgc = fl_state[fl_vmode].textgc[0];
	if (fl_state[fl_vmode].cur_fnt)
	    XSetFont(flx->display, flx->textgc, fl_state[fl_vmode].cur_fnt->fid);
	return;
    }

    /* check if we need to dither */
    fl_dithered(fl_vmode) = (fl_depth(fl_vmode) <= 2);

    /* need to create new GCs */
    M_warn("CreateGC", "For %s", fl_vclass_name(fl_vmode));
    if (!fl_gray_pattern[1])
    {
	M_err("CreateGC", "gray pattern not initialized");
	exit(1);
    }

    flgcs = fs->gc;
    flgce = flgcs + sizeof(fs->gc) / sizeof(fs->gc[0]);
    for (; flgcs < flgce; flgcs++)
    {
	*flgcs = XCreateGC(flx->display, win, 0, 0);
	XSetStipple(flx->display, *flgcs, fl_inactive_pattern);
	XSetGraphicsExposures(flx->display, *flgcs, 0);
    }

    flx->gc = fl_state[fl_vmode].gc[0];

    /* initialize text gc */
    flgcs = fs->textgc;
    flgce = flgcs + sizeof(fs->textgc) / sizeof(fs->textgc[0]);
    for (; flgcs < flgce; flgcs++)
    {
	*flgcs = XCreateGC(flx->display, win, 0, 0);
	XSetStipple(flx->display, *flgcs, fl_inactive_pattern);
	XSetGraphicsExposures(flx->display, *flgcs, 0);
    }
    flx->textgc = fl_state[fl_vmode].textgc[0];

    /* initialize a dimmed GC */
    fl_state[fl_vmode].dimmedGC = XCreateGC(flx->display, win, 0, 0);
    XSetStipple(flx->display, fl_state[fl_vmode].dimmedGC, fl_inactive_pattern);
    XSetGraphicsExposures(flx->display, fl_state[fl_vmode].dimmedGC, 0);
    XSetFillStyle(flx->display, fl_state[fl_vmode].dimmedGC, FillStippled);

    /* special for B&W and 2bits displays */
    if (fl_dithered(fl_vmode))
    {
	fl_whitegc = XCreateGC(flx->display, win, 0, 0);
	XSetForeground(flx->display, fl_whitegc, fl_get_flcolor(FL_WHITE));

	fl_bwgc[0] = XCreateGC(flx->display, win, 0, 0);
	XSetStipple(flx->display, fl_bwgc[0], fl_gray_pattern[0]);
	XSetGraphicsExposures(flx->display, fl_bwgc[0], 0);
	XSetFillStyle(flx->display, fl_bwgc[0], FillStippled);

	fl_bwgc[1] = XCreateGC(flx->display, win, 0, 0);
	XSetStipple(flx->display, fl_bwgc[1], fl_gray_pattern[1]);
	XSetGraphicsExposures(flx->display, fl_bwgc[1], 0);
	XSetFillStyle(flx->display, fl_bwgc[1], FillStippled);

	fl_bwgc[2] = XCreateGC(flx->display, win, 0, 0);
	XSetStipple(flx->display, fl_bwgc[2], fl_gray_pattern[2]);
	XSetGraphicsExposures(flx->display, fl_bwgc[2], 0);
	XSetFillStyle(flx->display, fl_bwgc[2], FillStippled);
    }


    if (fl_state[fl_vmode].cur_fnt)
	XSetFont(flx->display, flx->textgc, fl_state[fl_vmode].cur_fnt->fid);
}

/*
 *  Global routine to initialize all things related visual and colors
 */


void
fl_init_colormap(int vmode)
{
    int i, ok;
    FL_IMAP *fm = fl_imap;
    Colormap defmap;

    /* if map for this mode already exists, leave it alone, not only
       efficient but also necessary as fl_win_open may call init map */
    if (fl_map(vmode))
    {
#if (FL_DEBUG >= ML_DEBUG)
	M_info("InitColormap", "%s is ok", fl_vclass_name(vmode));
#endif
	return;
    }

    /* get max colors we can have */
    max_server_cols = (1L << fl_depth(vmode));
    if (max_server_cols <= 0)
	max_server_cols = 80;

    predefined_cols = FL_min(builtin, max_server_cols);
    M_info("BestVisual", "MaxColors=%d PredefCol=%d",
	   max_server_cols, predefined_cols);

    fl_init_stipples();

    if (!defaultc)
	defaultc = fl_malloc(FL_MAX_COLS * sizeof(*defaultc));

    /* initialize secondary lookup table */
    for (fm = fl_imap, i = 0; i < FL_MAX_COLS; i++, fm++)
    {
	defaultc[i].pixel = i;
	fm->grayval = FL_RGB2GRAY(fm->r, fm->g, fm->b);
	if (i >= FL_BUILT_IN_COLS)
	    fm->index = i;
    }

    /* take a snapshot of the default colormap for later use by
       private_colormap */

    defmap = DefaultColormap(flx->display, fl_screen);
    cols_in_default_visual = (1L << DefaultDepth(flx->display, fl_screen)) - 1;

    /* some server may have a default visual with depth == 32 */
    if (cols_in_default_visual <= 0)
	cols_in_default_visual = 80;

    M_warn("DefaultColormap", "%ld (0x%lx)", defmap, defmap);

    XQueryColors(flx->display, defmap, defaultc,
		 FL_min(cols_in_default_visual, DEFAULT_SAVE));

    ok = 0;

    if (fl_cntl.privateColormap)
	ok = get_private_cmap(vmode);
    else if (fl_cntl.standardColormap)
	ok = get_standard_cmap(vmode);

    if (!ok && !(ok = get_shared_cmap(vmode)))
    {
	/* if unable to share colormaps, we can either get a private colormap
	   or force substitutions */
#if 1
	M_err(0, "Failed to share colors. Using private colormap");
	ok = get_private_cmap(vmode);
#else
	ok = 1;
	fl_cntl.sharedColormap = 1;
	get_shared_cmap(vmode);
#endif
    }

    if (!ok)
    {
	M_err("Init", "I screwed up or you have a weird workstatation");
	exit(1);
    }

    M_warn("InitCMap", "%s Done", fl_vclass_name(vmode));

#if (FL_DEBUG >= ML_WARN)
    fl_dump_state_info(vmode, "FLMap");
#endif
}

static unsigned long
  fl_get_rgb_pixel(FL_COLOR packed, int *newpix);

/*
 * input col is the color known to the world outside of FORMS,
 * FL_RED etc. Returned will be the true pixel X understands
 */
unsigned long
fl_get_pixel(FL_COLOR col)
{
    if (col == FL_NoColor)
	return fl_get_pixel(FL_COL1);

    if (flx->isRGBColor)
    {
	return fl_get_rgb_pixel(col, &flx->newpix);
    }
    else
    {
	if (col >= FL_MAX_COLS)
	{
	    Bark("FLGetPixel", "Bad request %lu", col);
	    return 0;
	}
	return fl_state[fl_vmode].lut[col];
    }
}

void
fl_color(FL_COLOR col)
{
    static int vmode = -1;
    long p;

    if (flx->color != col || vmode != fl_vmode)
    {
	flx->color = col;
	vmode = fl_vmode;
	p = fl_get_pixel(col);
	XSetForeground(flx->display, flx->gc, p);
	fl_free_newpixel(p);
    }
}

#ifndef FL_GETR
#define FL_GETR(p)  (p & 0xff)
#define FL_GETG(p)  ((p >> 8) & 0xff)
#define FL_GETB(p)  ((p >> 16) & 0xff)
#endif

/* given a RGB triplet, set the folor in GC */
static unsigned long
fl_get_rgb_pixel(FL_COLOR packed, int *newpix)
{
    FL_STATE *s = &fl_state[fl_vmode];
    unsigned long pixel;
    int max_col;
    static Colormap lastcolormap;
    static XColor *xcolor;
    static int new_col;
    XColor xc;

    unsigned r = FL_GETR(packed);
    unsigned g = FL_GETG(packed);
    unsigned b = FL_GETB(packed);

    *newpix = 0;
    if (s->vclass == TrueColor || s->vclass == DirectColor)
    {
	return rgb2pixel(r, g, b);
    }
    else
    {
	xc.flags = DoRed | DoGreen | DoBlue;
	xc.red = (r << 8) | 0xff;
	xc.green = (g << 8) | 0xff;
	xc.blue = (b << 8) | 0xff;

	new_col++;

	if ((*newpix = XAllocColor(flx->display, s->colormap, &xc)))
	    return xc.pixel;

	/* color allocation failed. Search for best match */
	if((max_col = FL_min(256, (1 << s->depth))) == 0)
            max_col = 256;

	if (!xcolor)
	    xcolor = fl_malloc(256 * sizeof(*xcolor));

	/* 
	 * not theoretically correct as colormap may have changed 
	 * since the last time we asked for colors. Take a chance for 
	 * performace.
	 */
	if (lastcolormap != s->colormap || new_col > 3)
	{
	    int i;
	    for (i = 0; i < max_col; i++)
		xcolor[i].pixel = i;
	    XQueryColors(flx->display, s->colormap, xcolor, max_col);
	    lastcolormap = s->colormap;
	    new_col = 0;
	}
	fl_find_closest_color(r, g, b, xcolor, max_col, &pixel);
	return pixel;
    }
}

void
fl_free_newpixel(unsigned long pixel)
{
    if (flx->newpix)
    {
	XFreeColors(flx->display, flx->colormap, &pixel, 1, 0);
	flx->newpix = 0;
    }
}

void
fl_textcolor(FL_COLOR col)
{
    static int vmode = -1;
    static int switched;
    static GC textgc;

    if (flx->textcolor != col || vmode != fl_vmode ||
	flx->textcolor == lastmapped)
    {

	lastmapped = FL_NoColor;

	flx->textcolor = col;

	vmode = fl_vmode;
	if (col == FL_INACTIVE_COL && fl_dithered(vmode))
	{
	    textgc = flx->textgc;
	    flx->textgc = fl_state[vmode].dimmedGC;
	    XSetFont(flx->display, flx->textgc, fl_state[vmode].cur_fnt->fid);
	    switched = 1;
	}
	else if (switched)
	{
	    flx->textgc = textgc;
	    XSetFont(flx->display, flx->textgc, fl_state[vmode].cur_fnt->fid);
	    switched = 0;
	}

	XSetForeground(flx->display, flx->textgc, col = fl_get_pixel(col));
	fl_free_newpixel(col);
    }
}

void
fl_bk_color(unsigned long col)
{
    if (flx->bkcolor != col)
    {
	flx->bkcolor = col;
	col = fl_get_pixel(col);
	XSetBackground(flx->display, flx->gc, col);
	fl_free_newpixel(col);
    }
}

void
fl_bk_textcolor(unsigned long col)
{
    if (flx->bktextcolor != col)
    {
	flx->bktextcolor = col;
	col = fl_get_pixel(col);
	XSetBackground(flx->display, flx->textgc, col);
	fl_free_newpixel(col);
    }
}


/*
 * map color: it involves changing the internal colormap as well as
 * requesting the acutal pixel value. In case a request fails,
 * we subsititute the closest color
 */
unsigned long
fl_mapcolor(FL_COLOR col, int r, int g, int b)
{
    XColor exact;
    int i, j = -1;
    static int totalcols;
    static XColor *cur_mapvals[6], *cur_map;
    unsigned long pixel;

    /* if requested color is reserved, warn */
    if (col < FL_BUILT_IN_COLS)
	M_warn("MapColor", "Changing reserved color");

    /* must invalidate color cache */
    if (col == flx->color)
	flx->color = BadPixel;

    lut = fl_state[fl_vmode].lut;

    if (col >= flmapsize)
    {
	M_err("MapColor", "Only %d indexed colors are supported", flmapsize);
	return 0;
    }


    /* col is the external colorname, FL_RED etc, which is kept in
       fl_imap[].index. */
    if (col == fl_imap[col].index)
	j = col;

    for (i = 0; j < 0 && i < flmapsize; i++)
	if (col == fl_imap[i].index)
	    j = i;

    if (j < 0)
	j = flmapsize - 1;

    /* in BW, too many colors collaps together */
    if (fl_imap[j].r == r && fl_imap[j].g == g && fl_imap[j].b == b &&
	r != 0 && !fl_dithered(fl_vmode) && lut[col])
    {
	return lut[col];
    }


    fl_imap[j].r = r;
    fl_imap[j].g = g;
    fl_imap[j].b = b;
    fl_imap[j].grayval = FL_RGB2GRAY(r, g, b);
    fl_imap[j].index = col;

    lastmapped = col;

    M_warn("fl_mapcolor", "mapping %ld (%d,%d,%d)", col, r, g, b);

    pixel = lut[col];
    exact.red = (r << 8) | 0xff;
    exact.green = (g << 8) | 0xff;
    exact.blue = (b << 8) | 0xff;
    exact.flags = DoRed | DoGreen | DoBlue;
    exact.pixel = pixel;

    if (fl_vmode == DirectColor)
    {
	exact.pixel = lut[col] = rgb2pixel(r, g, b);
	XStoreColors(flx->display, fl_map(fl_vmode), &exact, 1);
	return lut[col];
    }


    /* pixel value known by the server */
    if (!allow_leakage && fl_depth(fl_vmode) >= 4 && pixel != BadPixel)
	fl_free_pixels(&pixel, 1);

    if (XAllocColor(flx->display, fl_colormap(fl_vmode), &exact))
    {
	lut[col] = exact.pixel;
	return lut[col];
    }

    /* colormap is full. Warn once and substitute from now on */

    if (!cur_mapvals[fl_vmode])
    {
	totalcols = FL_min(FL_MAX_COLS, (1L << fl_depth(fl_vmode)));
	M_err("MapColor", "ColormapFull. Using substitutions");
	cur_map = fl_calloc(totalcols + 1, sizeof(*cur_map));
	cur_mapvals[fl_vmode] = cur_map;

	/* take a snapshot of the color map */
	for (i = 0; i < totalcols; i++)
	    cur_map[i].pixel = i;

	XQueryColors(flx->display, fl_map(fl_vmode), cur_map, totalcols);
    }

    /* search for the closest match */
    cur_map = cur_mapvals[fl_vmode];
    j = fl_find_closest_color(r, g, b, cur_map, totalcols, &pixel);
    if (j < 0)
    {
	M_err("fl_mapcolor", "Something is very wrong");
	exit(1);
    }

    /* j will be the stuff we want */
    lut[col] = cur_map[j].pixel;

    /* we still need to allocate the color, otherwise destroying the just
       requested color might accidentally free a reserved pixel */

    exact.red = cur_map[j].red;
    exact.green = cur_map[j].green;
    exact.blue = cur_map[j].blue;
    exact.pixel = cur_map[j].pixel;
    exact.flags = DoRed | DoGreen | DoBlue;

    if (!XAllocColor(flx->display, fl_colormap(fl_vmode), &exact))
	M_warn("MapColor", "Something is wrong - will proceed");


#if (FL_DEBUG >= M_DEBUG)
    M_warn("MapColor", "(%d %d %d)<->(%d %d %d)",
	   r, g, b, cur_map[j].red, cur_map[j].green, cur_map[j].blue);
#endif

    return lut[col];
}

long
fl_mapcolorname(FL_COLOR col, const char *name)
{
    XColor xc;

    if (XParseColor(flx->display, fl_colormap(fl_vmode), (char *) name, &xc))
    {
	xc.red = (xc.red >> 8) & 0xff;
	xc.green = (xc.green >> 8) & 0xff;
	xc.blue = (xc.blue >> 8) & 0xff;
	return fl_mapcolor(col, xc.red, xc.green, xc.blue);
    }
    return -1;
}


/*
 * change internal colormap before initialization. This way,
 * FORMS default color can be changed
 */

void
fl_set_icm_color(FL_COLOR col, int r, int g, int b)
{
    int i;

    for (i = 0; i < flmapsize; i++)
	if (col == fl_imap[i].index)
	{
	    if (FL_is_gray(fl_vmode))
	    {
		fl_imap[i].grayval = FL_RGB2GRAY(r, g, b);
	    }
	    else
	    {
		fl_imap[i].r = r;
		fl_imap[i].g = g;
		fl_imap[i].b = b;
	    }
	    return;
	}
}

/* query internal  colormap */
void
fl_get_icm_color(FL_COLOR col, int *r, int *g, int *b)
{
    int i;

    for (i = 0; i < flmapsize; i++)
	if (col == fl_imap[i].index)
	{
	    if (FL_is_gray(fl_vmode))
	    {
		*r = *g = *b = fl_imap[i].grayval;
	    }
	    else
	    {
		*r = fl_imap[i].r;
		*g = fl_imap[i].g;
		*b = fl_imap[i].b;
	    }
	    return;
	}
}

/* query real colormap. r,g,b returned is the current color used */
unsigned long
fl_getmcolor(FL_COLOR i, int *r, int *g, int *b)
{
    XColor exact;

    if ((exact.pixel = fl_get_pixel(i)) >= max_server_cols)
    {
	*r = *g = *b = 0;
	return (unsigned long) -1;
    }

    XQueryColor(flx->display, fl_map(fl_vmode), &exact);

    *r = (exact.red >> 8) & 0xff;
    *g = (exact.green >> 8) & 0xff;
    *b = (exact.blue >> 8) & 0xff;
    return exact.pixel;
}

/*** End of Colormap routines  *******}***************/

void
fl_set_graphics_mode(int mode, int doublebuf)
{
    if (mode >= 0 && mode < 6 && fl_mode_capable(mode, 1))
    {
	fl_vmode = mode;
	M_warn("GraphicsMode", "Changed to %s\n", fl_vclass_name(mode));
    }

    fl_cntl.doubleBuffer = doublebuf && fl_doublebuffer_capable(0);

#if 0
    if (!fl_state[mode].trailblazer)
    {
	fl_init_colormap(fl_vmode);
	fl_get_font_struct(FL_NORMAL_STYLE, FL_DEFAULT_FONT);
	fl_state[mode].trailblazer = fl_create_window(fl_root,
					   fl_map(fl_vmode), "trailblazer");
	XSync(flx->display, 1);
    }
#endif
}

int
fl_get_visual_depth(void)
{
    return fl_state[fl_vmode].depth;
}

#if (FL_DEBUG >= FL_WARN)	/* { */
/* print out the current state info. For debugging only */
void
fl_dump_state_info(int mode, const char *where)
{
    FL_State *fs = fl_state + mode;
    XVisualInfo xvi;

    if (fl_cntl.debug)
    {
	fprintf(stderr, "In %s", where);
	fprintf(stderr, " VClass:%s", fl_vclass_name(fl_class(mode)));
	fprintf(stderr, " VisualID:0x%lx", fs->xvinfo->visualid);
	fprintf(stderr, " Depth:%d %d",
		fl_depth(mode), fs->xvinfo->depth);
	fprintf(stderr, " Colormap:0x%lx\n", fl_map(mode));
    }

    /* some more checks */
    if (!XMatchVisualInfo(flx->display, fl_screen, fl_depth(mode),
			  fl_class(mode), &xvi))
    {
	M_err("StateInfo", "Can't match listed visual");
	exit(1);
    }

    if (fl_visual(mode)->visualid != xvi.visualid)
    {
	/* possible on SGI with OpenGL selected visuals */
	M_warn("StateInfo", "inconsistent visualID, probably OK");
    }

    /* state info consistency */
    if (fl_depth(mode) != fs->xvinfo->depth)
    {
	M_err("StateInfo", "Bad Depth");
	exit(1);
    }

    if (fl_class(mode) != fs->xvinfo->class)
    {
	fprintf(stderr, "Bad visual class\n");
	M_err("StateInfo", "Bad visual class");
	exit(1);
    }
}

#endif /* } */


int
fl_mode_capable(int mode, int warn)
{
    int cap;

    if (mode < 0 || mode > 5)
    {
	Bark("GraphicsMode", "Bad mode=%d", mode);
	return 0;
    }

    cap = (fl_depth(mode) >= FL_MINDEPTH && fl_visual(mode));

    if (!cap && warn)
	M_err("CheckGMode", "Not capable of %s at depth=%d",
	      fl_vclass_name(mode), fl_depth(mode));
    return cap;
}

/*
 * Convert a RGB triple into a pixel value usable in DirectColor
 * and TrueColor. Note the RGB triple is ONE-BYTE EACH.
 */

static FL_COLOR
rgb2pixel(register unsigned r, register unsigned g, register unsigned b)
{
    register FL_State *s = fl_state + fl_vmode;

    /* this one drops bits and looks bad if primary color resolution is less
       than 6, but server calculates color this way. A better way  should be
       r = ((float) r * ((1L << s->rbits) - 1) / 255.0 + 0.1); */

    if (s->rbits < 8)
    {
	r >>= (8 - s->rbits);
	g >>= (8 - s->gbits);
	b >>= (8 - s->bbits);
    }
    else if (s->rbits > 8)
    {
	r <<= (8 - s->rbits);
	g <<= (8 - s->gbits);
	b <<= (8 - s->bbits);
    }

    return (((unsigned long) r << s->rshift) & s->rmask) |
	(((unsigned long) g << s->gshift) & s->gmask) |
	(((unsigned long) b << s->bshift) & s->bmask);
}

/*
 * Create a colormap valid for the given visual. It also
 * fills the colormap with default XFORMS map
 */

#define MAXFILL  (FL_BUILT_IN_COLS + 1 )
#define MAXREAD   100

Colormap
fl_create_colormap(XVisualInfo * xv, int nfill)
{
    long black = BlackPixel(flx->display, fl_screen);
    long white = WhitePixel(flx->display, fl_screen);
    XColor xc;
    Colormap cmap;
    int depth = xv->depth==32 ? 24:xv->depth;
    int maxcolors = (1 << depth);
    int maxread = FL_min(MAXREAD, maxcolors);
    XColor *cur_entries = fl_malloc(sizeof(XColor) * maxread), *dc;
    int i, j, k, found;
    unsigned long pixels[MAXREAD], allocated[MAXREAD], p;
    int keep = maxcolors / 32;

    cmap = XCreateColormap(flx->display, fl_root, xv->visual,
			xv->class != FL_DirectColor ? AllocNone : AllocAll);

    /* as a minimum, try to preserve Black or White */
    xc.flags = DoRed | DoGreen | DoBlue;
    xc.pixel = 0;
    if (black == 0 && nfill >= 0)
    {
	xc.red = xc.green = xc.blue = 0;
	XAllocColor(flx->display, cmap, &xc);
    }
    else if (white == 0 && nfill >= 0)
    {
	xc.red = xc.green = xc.blue = 0xffff;
	XAllocColor(flx->display, cmap, &xc);
    }

    /* now if have the same visual, we can do event more */

    if (nfill > 0 && fl_vmode == xv->class && fl_vmode != DirectColor)
    {
	nfill = FL_min(MAXFILL, nfill);
	nfill = FL_min(nfill, maxcolors);
	if (nfill < 4)		/* potentially 8+1+4 colors */
	    nfill = 4;
	for (i = 0; i < maxread; i++)
	    cur_entries[i].pixel = i;

	XQueryColors(flx->display, fl_map(fl_vmode), cur_entries, maxread);

	dc = cur_entries;
	for (k = i = 0; i < maxread; i++, dc++)
	{
	    allocated[i] = FL_NoColor;
	    dc->flags = DoRed | DoGreen | DoBlue;
	    if (XAllocColor(flx->display, cmap, dc))
		allocated[k++] = dc->pixel;

	}

	/* now free the non-xforms colors, but keep some non-xforms entries */
	for (k = 0, i = keep; i < maxread; i++)
	{
	    p = allocated[i];
	    for (j = found = 0; !found && j < nfill; j++)
		found = (p == fl_get_pixel(j) || (p == white || p == black || p == 34));
	    if (!found && p != FL_NoColor)
		pixels[k++] = p;
	}

	if (k)
	{
	    M_warn("CreateColormap", "free %d\n", k);
	    XFreeColors(flx->display, cmap, pixels, k, 0);
	}
    }

    fl_free(cur_entries);
    return cmap;
}

void
fl_set_color_leak(int y)
{
    allow_leakage = y;
}

/*
 * if an index is being for the first time, the corresponding pixel
 * is either reserverd or bad, need to trap it
 */
static int
bad_color_handler(Display * d, XErrorEvent * xev)
{
    if (xev->error_code == BadAccess)
	M_warn("XFreeColor", "bad pixel");
    return 0;
}

void
fl_free_pixels(unsigned long *pix, int n)
{
    int (*oh) (Display *, XErrorEvent *);
    oh = XSetErrorHandler(bad_color_handler);
    XFreeColors(flx->display, fl_map(fl_vmode), pix, n, 0);
    XSync(flx->display, 0);
    XSetErrorHandler(oh);
}

void
fl_free_colors(FL_COLOR * c, int n)
{
    int i, k, j = -1;
    unsigned long *pixels = fl_malloc(n * sizeof(*pixels)), *pix;

    lut = fl_state[fl_vmode].lut;
    pix = pixels;

    for (k = 0; k < n; k++, c++, pix++)
    {
	/* if requested color is reserved, warn */
	if (*c < FL_FREE_COL1)
	    M_warn("MapColor", "Changing reserved color");

	/* must invalidate color cache */
	if (*c == flx->color)
	    flx->color = BadPixel;

	for (i = 0; j < 0 && i < flmapsize; i++)
	    if (*c == fl_imap[i].index)
		j = i;

	if (j < 0)
	    j = flmapsize - 1;

	/* pixel value known by the server */
	*pix = lut[*c];
	/* mark this pixel bad */
	lut[*c] = BadPixel;
    }

    fl_free_pixels(pixels, n);
    fl_free(pixels);
}

#define LINEAR_COLOR_DISTANCE  0

/* (r,g,b) input should be 8bit each */
int
fl_find_closest_color(int r, int g, int b, XColor * map, int len,
		      unsigned long *pix)
{
    long mindiff = 0x7fffffff, diff;
    int dr, dg, db;
    int i, k;

    for (k = i = 0; i < len; i++)
    {
	dr = (r - ((map[i].red >> 8) & 0xff));
	dg = (g - ((map[i].green >> 8) & 0xff));
	db = (b - ((map[i].blue >> 8) & 0xff));

	/* correct formula is (.299,.587,.114) */
#if LINEAR_COLOR_DISTANCE
	diff = 3 * FL_abs(dr) + 4 * FL_abs(dg) + 2 * FL_abs(db);
#else
	diff = 3 * (dr * dr) + 4 * (dg * dg) + 2 * (db * db);
#endif
	if (diff < 0)
	    fprintf(stderr, "dr=%d dg=%d db=%d diff = %ld\n", dr, dg, db, diff);
	if (diff < mindiff)
	{
	    mindiff = diff;
	    k = i;
	    *pix = map[i].pixel;
	}
    }
    return k;
}
