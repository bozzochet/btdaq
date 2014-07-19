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
 * $Id: pflps.h,v 0.89 1999/07/17 03:17:54 zhao Beta $
 *
 * private headers of postscript support. Must come
 * after forms.h
 *
 */

#ifndef PFLPS_H
#define PFLPS_H

#include "forms.h"

/* configuration infomation. contains all information needed for
 * both regular gui postscript and image_postscript members
 */
typedef struct
{
    FLPS_MEMBERS;
    FILE *fp;
    int lastc, literal, len;
    int pack;
    int verbose;
    /* cache */
    int cur_lw, last_lw;        /* line width                */
    int cur_style, cur_size;    /* font style and size       */
    long cur_color;             /* color cache               */
    int landscape;
    float final_xscale, final_yscale;
    /* private fields for regular flps  */
    int unit;			/* unit of measure           */
    int pages;			/* how many pages            */
    int page;			/* current page              */
    int epsf_import;		/* true if importing EPS     */
    int inverted;		/* take care of coord switch */
    int user_bw;		/* bw specified on cmdline   */
    int bw;			/* current border width      */
    const char *poly_name;	/* PS poly name              */
    /* private field for image postscript */
    char *prefix;               /* output file prefix        */
    int misct, miscl, miscb, miscr; /* misc. margins         */
    int lastr, lastg, lastb;
    int comment;
    int isRGBColor;
    int rotation;
    float s2px;             /* screen to paper scaling        */
    float s2py;             /* screen to paper scaling        */
    int preserved[2];
}
FLPSInfo;

extern FLPSInfo *flps;

extern void flps_color(long);
extern void flps_rgbcolor(int, int, int);
extern int flps_get_gray255(long);
extern void flps_emit_prolog(void);
extern void flps_emit_header(const char *, int, int, int, int, int);

extern void flps_switch_flps(FLPSInfo *info);
extern void flps_restore_flps(void);


/* basic drawing of simple geomtric figures */
extern void flps_draw_init(void);
extern void flps_rectangle(int, int, int, int, int, long);
extern void flps_roundrectangle(int, int, int, int, int, long);
extern void flps_lines(FL_POINT *, int, long);
extern void flps_line(int, int, int, int, long);
extern void flps_poly(int, FL_POINT *, int, long);
extern int flps_draw_symbol(const char *, int, int, int, int, long);
extern void flps_oval(int, int, int, int, int, long);
extern void flps_pieslice(int fill, int x, int y, int w, int h,
			  int t1, int t2, long col);
extern void flps_circ(int, int, int, int, long);
extern void flps_arc(int fill, int x, int y, int r, int t1, int t2, long col);


#define flps_rectf(x,y,w,h,c)   flps_rectangle(1,x,y,w,h,c)
#define flps_rect(x,y,w,h,c)    flps_rectangle(0,x,y,w,h,c)


extern void flps_draw_box(int, int, int, int, int, long, int);
extern void flps_draw_tbox(int, int, int, int, int, long, int);
extern void flps_draw_frame(int, int, int, int, int, long, int);
extern void flps_draw_checkbox(int, int, int, int, int, long, int);

/* basic text drawing routines */

extern void flps_draw_text(int, int, int, int, int, long, int, int,
			   const char *);
extern void flps_draw_text_beside(int, int, int, int, int, long, int,
				  int, const char *);
extern void flps_text_init(void);

extern int find_type_val(int, const char *);
extern void flps_reset_cache(void);
extern void flps_invalidate_color_cache(void);
extern void flps_invalidate_font_cache(void);
extern void flps_invalidate_linewidth_cache(void);
extern void flps_invalidate_symbol_cache(void);
extern void flps_linewidth(int);
extern int flps_get_linewidth(void);
extern void flps_reset_linewidth(void);
extern void flps_linestyle(int);
extern int flps_get_linestyle(void);

extern void flps_log(const char *s);
extern void flps_output(const char *s,...);
extern void flps_set_font(int, int);

extern int get_gray255(long);
extern void get_scale_unit(int, float *, float *);
extern void ps_invalidate_font_cache(void);
extern char *ps_literal(const char *);
extern void flps_set_clipping(int, int, int, int);
extern void flps_unset_clipping(void);
extern void flps_apply_gamma(float);
extern int flps_get_namedcolor(const char *);



#define PS_SPECIAL(c) ( c=='('||c==')'||c=='['||c==']' || \
                        c=='<'||c=='>'||c=='%' ||c=='#' ||c=='/')


#endif
