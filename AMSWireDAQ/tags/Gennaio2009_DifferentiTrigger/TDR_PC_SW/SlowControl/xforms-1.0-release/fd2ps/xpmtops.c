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
 * $Id: xpmtops.c,v 0.36 1999/07/17 03:32:50 zhao beta $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1997-2000  by T.C. Zhao
 *  All rights reserved.
 *.
 *
 *  Turn an xpm file into PostScript.
 *  Two methods are provided: one is to read the xpm file
 *  and convert directly. The other one is to use pbm+ package.
 *  The direct method suffers from the fact that it only handles
 *  xpm3 (not a big problem as xpm1 is very rare these days).
 *  The pbm+ has the drawback that it does not handle the
 *  grayscale colormap in the xpm file.
 *
 *  The default is to use the direct translation.
 *
 */

#include "forms.h"
#include "fd2ps.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>		/* unlink */
#include <signal.h>


/* get the dimension of the xpm file for alignment */
static int
get_xpm_size(const char *file, int *w, int *h)
{
    FILE *fp = fopen(file, "r");
    int c, j;

    *w = *h = -1;

    if (!fp)
    {
	fprintf(stderr, "Can't open xpm file %s\n", file);
	return -1;
    }

    while ((c = getc(fp)) != '"' && c != EOF)
	;

    if (c == EOF)
    {
	fprintf(stderr, "error reading xpm file\n");
	fclose(fp);
	return -1;
    }

    if (fscanf(fp, "%d %d %d %d", w, h, &j, &j) != 4)
    {
	fprintf(stderr, "Failed to get the size of the xpm\n");
	fclose(fp);
	*w = *h = 25;
	return -1;
    }

    fclose(fp);
    return 0;
}

void
draw_xpm(FL_OBJECT * ob)
{
    SPEC *sp = ob->spec;
    int xx, yy, w, h;

    if (get_xpm_size(sp->file, &w, &h) >= 0)
    {
	fl_get_align_xy(sp->align, ob->x, ob->y, ob->w, ob->h, w, h,
			FL_abs(ob->bw) + sp->dx, FL_abs(ob->bw) + sp->dy,
			&xx, &yy);

#if 0
	if (!flps->xpmtops_direct)
	    xpmtops_via_ppm(sp->file, xx, yy, ob->col1);
	else
#endif
	    xpmtops_direct(sp->file, xx, yy, ob->col1);
    }

}

#if 0
/*
 * Convert using pbmplus+ package {
 */

static char *bgn_eps =
"/BeginEPSF { /inc_state save def /dict_count countdictstack def\n"
"/op_count count 1 sub def userdict begin /showpage {} def\n"
"0 setgray 0 setlinecap 1 setlinewidth 0 setlinejoin \n"
"10 setmiterlimit [] 0 setdash newpath\n"
"/languagelevel where {pop languagelevel 1 ne\n"
"{false setstrokeadjust false setoverprint } if } if} BD\n";

static char *end_eps =
"/EndEPSF { count op_count sub {pop} repeat\n"
"countdictstack dict_count sub{end} repeat inc_state restore } BD\n";

void
emit_epsf_import_command(void)
{
    static int emitted;

    if (!emitted)
    {
	ps_output(bgn_eps);
	ps_output(end_eps);
	emitted = 1;
    }
}

static int
get_bounding_box(const char *file, int *x, int *y, int *w, int *h)
{
    FILE *fp;
    char buf[1024];

    if (!(fp = fopen(file, "r")))
    {
	return -1;
    }

    while (fgets(buf, sizeof(buf), fp))
    {
	if (strstr(buf, "BoundingBox:"))
	{
	    sscanf(buf, "%*s %d %d %d %d\n", x, y, w, h);
	    fclose(fp);
	    return 0;
	}
    }
    fclose(fp);

    return -1;
}

static char *logfile = "xpmtops.log";

static void
convert_to_ps(const char *file, long trans)
{
    FILE *fpr, *fpw;
    char *prefix = "/tmp/xpmtops.eps";
    char tempfile[128];
    char *cmd, buf[1024], tmpbuf[1024], *p, rgbstr[32];
    int r, g, b, xo, yo, w, h;

    if (!file || !*file)
    {
	fprintf(stderr, "null xpm filename\n");
	return;
    }

    sprintf(tempfile, "%s.%s", prefix, whoami());

    if (!(fpr = fopen(file, "r")))
    {
	fprintf(stderr, "Can't open %s\n", file);
	return;
    }

    if (psinfo.colorps)
	cmd = "(xpmtoppm | pnmdepth 255 | pnmtops >%s) >%s 2>&1";
    else
	cmd = "(xpmtoppm | pnmdepth 255 | ppmtopgm | pnmtops >%s) >%s 2>%1";

    signal(SIGPIPE, SIG_IGN);
    sprintf(buf, cmd, tempfile, logfile);
    if (!(fpw = popen(buf, "w")))
    {
	fprintf(stderr, "failed to convert xpm to PS. See %s file details\n",
		logfile);
	return;
    }

    /* fix None attributes on the fly */
    fl_query_imap(trans, &r, &g, &b);
    sprintf(rgbstr, "#%2x%2x%2x", r, g, b);

    while (fgets(buf, sizeof(buf) - 1, fpr))
    {
	while ((p = strstr(buf, "None")))
	{
	    *p = '\0';
	    strcat(strcpy(tmpbuf, buf), rgbstr);
	    strcat(tmpbuf, p + 4);
	    strcpy(buf, tmpbuf);
	}
	fputs(buf, fpw);
    }
    pclose(fpw);

    /* find the bounding box of the EPS */
    if (get_bounding_box(tempfile, &xo, &yo, &w, &h) < 0)
    {
	fprintf(stderr, "Failed to convert xpm to EPS. ");
	fprintf(stderr, "See %s for details\n", logfile);
	fprintf(stderr, "Rest of the files converted\n");
	return;
    }

    ps_output("-%d -%d translate\n", xo, yo);
    fpr = fopen(tempfile, "r");
    while (fgets(buf, sizeof(buf) - 1, fpr))
	ps_output(buf);
    fclose(fpr);
    unlink(tempfile);
    unlink(logfile);
}

static int
xpmtops_via_ppm(const char *file, int xx, int yy, long tran)
{
    emit_epsf_import_command();
    ps_verbatim("\n%% pixmap goes here\n");
    ps_output("BeginEPSF %d %d translate 1 1 scale\n", xx, yy);
    ps_verbatim("\n%%%%BeginDocument: %s\n", ps_literal(file));
    convert_to_ps(file, tran);
    ps_verbatim("\n%%%%EndDocument\n");
    ps_verbatim("EndEPSF\n");
    return 0;
}

/* End of convert using pbmplus package } */
#endif
